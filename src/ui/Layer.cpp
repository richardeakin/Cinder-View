/*
 Copyright (c) 2015, Richard Eakin - All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided
 that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this list of conditions and
 the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
 the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
 */

#include "ui/Layer.h"
#include "ui/Graph.h"
#include "ui/View.h"

#include "cinder/Log.h"
#include "cinder/gl/gl.h"
#include "cinder/app/Window.h"

#define ENABLE_FRAMEBUFFER_CACHING 1

using namespace ci;
using namespace std;

namespace ui {

FrameBuffer::FrameBuffer( const Format &format )
{
	auto fboFormat = gl::Fbo::Format();
	fboFormat.colorTexture(
		gl::Texture2d::Format()
 			.minFilter( GL_LINEAR ).magFilter( GL_LINEAR )
	);

	mFbo = gl::Fbo::create( format.mSize.x, format.mSize.y, fboFormat );
	CI_LOG_I( "created gl::Fbo of size: " << format.mSize );
}

bool FrameBuffer::Format::operator==(const Format &other) const
{
	return mSize == other.mSize;
}

// static
FrameBufferCache* FrameBufferCache::instance()
{
	static FrameBufferCache sInstance;
	return &sInstance;
}

// static
FrameBufferRef FrameBufferCache::getFrameBuffer( const ci::ivec2 &size )
{
	auto format = FrameBuffer::Format().size( size );
#if ENABLE_FRAMEBUFFER_CACHING
	auto it = instance()->mCache.find( format );
	if( it != instance()->mCache.end() ) {
		return it->second;
	}
	else {
		auto result = make_shared<FrameBuffer>( format );
		instance()->mCache.insert( make_pair( format, result ) );
		return result;
	}
#else
	return make_shared<FrameBuffer>( format );
#endif
}

Layer::Layer( View *view )
	: mRootView( view )
{
	CI_ASSERT( view );

	// TODO: View is constructing Layer in it's constructor, but it doesn't yet have a graph yet so no renderer
//	mRenderer = mRootView->getRenderer();

	CI_LOG_I( "view: " << view->getName() );
}

Layer::~Layer()
{
}

float Layer::getAlpha() const
{
	return mRootView->getAlpha();
}

// TODO: this assumes its always called at the top-level of the Layer's view tree, but that won't be the case when things like alpha or clip change
void Layer::configureViewList()
{
	CI_LOG_I( "mRootView: " << mRootView->getName() );

	function<void( View *view )> addViewToDrawList = [&] ( View *view ) {
		CI_LOG_I( "adding view to draw list: '" << view->getName() << "'" );
		mViews.push_back( view );
		view->mLayer = shared_from_this();
		for( const auto &subview : view->getSubviews() ) {
			if( subview->getLayer() ) {
				continue;
			}
			addViewToDrawList( subview.get() );
		}
	};

	mViews.clear();
	addViewToDrawList( mRootView );
	mNeedsLayout = false;

	CI_LOG_I( "mViews.size(): " << mViews.size() );
}

void Layer::update()
{
	if( getAlpha() < 0.9999f && mRenderTransparencyToFrameBuffer ) {
		if( ! mFrameBuffer ) {
			CI_LOG_I( "aquiring FrameBuffer for view '" << mRootView->getName() << "', size: " << mRootView->getSize() );
			mFrameBuffer = FrameBufferCache::getFrameBuffer( ivec2( mRootView->getSize() ) );
		}
	}
	else if( mFrameBuffer ) {
		CI_LOG_I( "removing FrameBuffer for view '" << mRootView->getName() << "'" );
		mFrameBuffer.reset();
	}

}

void Layer::draw()
{
	if( mFrameBuffer ) {
		gl::context()->pushFramebuffer( mFrameBuffer->mFbo );
		gl::pushViewport( mFrameBuffer->getSize() );
		gl::pushMatrices();
		gl::setMatricesWindow( mFrameBuffer->getSize() );

		gl::clear();
	}
	else {
//		gl::pushModelMatrix();
//		gl::translate( mRootView->getPos() );
	}

	beginClip();

	for( auto &view : mViews ) {
		CI_ASSERT( view );
		
		// TODO NEXT: this is now always happening for each View, whereas before it wasn't happening if there was a FrameBuffer.
		gl::ScopedModelMatrix modelScope;
		gl::translate( view->getPos() );

		view->drawImpl();
	}

	endClip();

	if( mFrameBuffer ) {
		gl::context()->popFramebuffer();
		gl::popViewport();
		gl::popMatrices();

		gl::ScopedColor colorScope( ColorA::gray( getAlpha() ) );

		auto destRect = Rectf( 0, 0, mFrameBuffer->mFbo->getWidth(), mFrameBuffer->mFbo->getHeight() ) + mRootView->getPos();
		gl::draw( mFrameBuffer->mFbo->getColorTexture(), destRect );

//		writeImage( "framebuffer.png", mFrameBuffer->mFbo->getColorTexture()->createSource() );
//
//		gl::color( 0, 1, 0 );
//		gl::drawStrokedRect( destRect, 2 );
	}
	else {
//		gl::popModelMatrix();
	}
}

void Layer::beginClip()
{
	if( mClipEnabled ) {
		auto window = mRootView->getGraph()->getWindow();

		// Search up the tree for a FrameBuffer
		const View *viewWithFrameBuffer = mRootView;
		bool hasFrameBufferInParentTree = false;
		while( viewWithFrameBuffer ) {
			if( viewWithFrameBuffer->getLayer()->getFrameBuffer() ) {
				hasFrameBufferInParentTree = true;
				break;
			}

			viewWithFrameBuffer = viewWithFrameBuffer->getParent();
		}

		Rectf viewWorldBounds = mRootView->getWorldBounds();
		vec2 lowerLeft = viewWorldBounds.getLowerLeft();
		if( hasFrameBufferInParentTree ) {
			// get bounds of view relative to framebuffer. // TODO: need a method like convertPointToView( view, point );
			Rectf frameBufferWorldBounds = viewWithFrameBuffer->getWorldBounds();
			Rectf boundsInFrameBuffer = viewWorldBounds - frameBufferWorldBounds.getUpperLeft();

			// Take lower left relative to FrameBuffer, still need to flip y
			lowerLeft = boundsInFrameBuffer.getLowerLeft();
			lowerLeft.y = frameBufferWorldBounds.getHeight() - lowerLeft.y;
		}
		else {
			// x is already in windows coordinates, flip y relative to window's bottom left
			lowerLeft.y = window->getHeight() - lowerLeft.y;
		}

		auto ctx = gl::context();
		ctx->pushBoolState( GL_SCISSOR_TEST, GL_TRUE );
		ctx->pushScissor( std::pair<ivec2, ivec2>( lowerLeft, mRootView->getSize() ) );
	}
}

void Layer::endClip()
{
	if( mClipEnabled ) {

		auto ctx = gl::context();
		ctx->popBoolState( GL_SCISSOR_TEST );
		ctx->popScissor();
	}
}

} // namespace ui

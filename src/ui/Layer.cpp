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

#include <cmath>

#define ENABLE_FRAMEBUFFER_CACHING 1

using namespace ci;
using namespace std;

namespace {

Rectf ceil( const Rectf &r )
{
	return Rectf( ceilf( r.x1 ), ceilf( r.y1 ), ceilf( r.x2 ), ceilf( r.y2 ) );
}

} // anonymous namesapce

namespace ui {

FrameBuffer::FrameBuffer( const Format &format )
{
	auto fboFormat = gl::Fbo::Format();
	fboFormat.colorTexture(
		gl::Texture2d::Format()
			.internalFormat( GL_RGBA )
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
	CI_ASSERT_MSG( view, "null pointer to View" );
	CI_LOG_I( "root view: " << view->getName() );
}

Layer::~Layer()
{
}

float Layer::getAlpha() const
{
	return mRootView->getAlpha();
}

void Layer::setNeedsLayout()
{
	mNeedsLayout = true;
}

void Layer::configureViewList()
{
	CI_LOG_I( "mRootView: " << mRootView->getName() );

	configureView( mRootView );
	mNeedsLayout = false;
}

void Layer::configureView( View *view )
{
	CI_LOG_I( "view: '" << view->getName() << "'" );
	view->mLayer = shared_from_this();

	if( view->isTransparent() ) {
		if( mRootView != view ) {
			// we need a new layer, which will configure its subtree
			auto layer = mGraph->makeLayer( view );
			layer->configureViewList();
			return;
		}
		else {
			if( ! mRootView->mRendersToFrameBuffer ) {
				CI_LOG_I( "enabling FrameBuffer for view '" << mRootView->getName() << "', size: " << mRootView->getSize() );
				CI_LOG_I( "\t- reason: alpha = " << mRootView->getAlpha() );
				mRootView->mRendersToFrameBuffer = true;
			}
		}
	}
	else {
		if( mRootView == view && mFrameBuffer ) {
			CI_LOG_I( "removing FrameBuffer for view '" << mRootView->getName() << "'" );
			CI_LOG_I( "\t- reason: alpha = " << mRootView->getAlpha() );
			mFrameBuffer.reset();
			mRootView->mRendersToFrameBuffer = false;
			mFrameBufferBounds = Rectf::zero();
			mGraph->removeLayer( shared_from_this() );
			return;
		}
	}

	if( mRootView->mRendersToFrameBuffer ) {
		Rectf frameBufferBounds = view->getBoundsForFrameBuffer();
		if( mFrameBufferBounds.getWidth() < frameBufferBounds.getWidth() && mFrameBufferBounds.getHeight() < frameBufferBounds.getHeight() ) {
			mFrameBufferBounds = ceil( frameBufferBounds );
			CI_LOG_I( "mFrameBufferBounds: " << mFrameBufferBounds );
		}
	}

	for( const auto &subview : view->getSubviews() ) {
		if( subview->isLayerRoot() ) {
			continue;
		}
		configureView( subview.get() );
	}
}

void Layer::draw()
{
	if( mRootView->mRendersToFrameBuffer ) {
		ivec2 frameBufferSize = ivec2( mFrameBufferBounds.getSize() );
		if( ! mFrameBuffer || mFrameBuffer->getSize().x < frameBufferSize.x || mFrameBuffer->getSize().y < frameBufferSize.y ) {
			CI_LOG_I( "aquiring FrameBuffer for view '" << mRootView->getName() << "', size: " << mFrameBufferBounds.getSize() );
			mFrameBuffer = FrameBufferCache::getFrameBuffer( frameBufferSize );
		}

		gl::context()->pushFramebuffer( mFrameBuffer->mFbo );
		gl::pushViewport( frameBufferSize );
		gl::pushMatrices();
		gl::setMatricesWindow( frameBufferSize );
		gl::translate( - mFrameBufferBounds.getUpperLeft() );

		gl::clear();
	}

	beginClip();

	drawView( mRootView );

	endClip();

	if( mRootView->mRendersToFrameBuffer ) {
		gl::context()->popFramebuffer();
		gl::popViewport();
		gl::popMatrices();

		auto ren = mRootView->getRenderer();
		ren->pushBlendMode( BlendMode::PREMULT_ALPHA );
		ren->pushColor( ColorA::gray( 1, getAlpha() ) );

		auto destRect = mFrameBufferBounds + mRootView->getPos();
		gl::draw( mFrameBuffer->mFbo->getColorTexture(), destRect );
		ren->popColor();
		ren->popBlendMode();

//		writeImage( "framebuffer.png", mFrameBuffer->mFbo->getColorTexture()->createSource() );

		// TODO: draw this in test by iterating over Graph's Layers
		// - need a way to get the Layer's position within View heirarchy
//		gl::color( 0, 1, 0 );
//		gl::drawStrokedRect( destRect );
	}
}

void Layer::drawView( View *view )
{
	gl::ScopedModelMatrix modelScope;

	if( view != mRootView )
		gl::translate( view->getPos() );

	view->drawImpl();

	auto thisRef = shared_from_this();
	for( auto &subview : view->getSubviews() ) {
		if( subview->getLayer() == thisRef ) {
			drawView( subview.get() );
		}
		else {
			subview->getLayer()->draw();
		}
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

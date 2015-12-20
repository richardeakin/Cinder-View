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

//#define LOG_LAYER( stream )	CI_LOG_I( stream )
#define LOG_LAYER( stream )	    ( (void)( 0 ) )

using namespace ci;
using namespace std;

namespace {

Rectf ceil( const Rectf &r )
{
	return Rectf( ceilf( r.x1 ), ceilf( r.y1 ), ceilf( r.x2 ), ceilf( r.y2 ) );
}

} // anonymous namesapce

namespace ui {

Layer::Layer( View *view )
	: mRootView( view )
{
	CI_ASSERT_MSG( view, "null pointer to View" );
	LOG_LAYER( "root view: " << view->getName() );
}

Layer::~Layer()
{
	LOG_LAYER( "bang" );
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
	LOG_LAYER( "mRootView: " << mRootView->getName() );

	configureView( mRootView );
	mNeedsLayout = false;
}

void Layer::configureView( View *view )
{
	LOG_LAYER( "view: '" << view->getName() << "'" );
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
				LOG_LAYER( "enabling FrameBuffer for view '" << mRootView->getName() << "', size: " << mRootView->getSize() );
				LOG_LAYER( "\t- reason: alpha = " << mRootView->getAlpha() );
				mRootView->mRendersToFrameBuffer = true;
			}
		}
	}
	else {
		if( mRootView == view && mFrameBuffer ) {
			LOG_LAYER( "removing FrameBuffer for view '" << mRootView->getName() << "'" );
			LOG_LAYER( "\t- reason: alpha = " << mRootView->getAlpha() );
			mFrameBuffer.reset();
			mRootView->mRendersToFrameBuffer = false;
			mFrameBufferBounds = Rectf::zero();
			mGraph->removeLayer( shared_from_this() );
			return;
		}
	}

	// TODO: handle runtime updates and disabling clip after it has been enabled
	if( view->isClipEnabled() ) {
		if( mRootView != view ) {
			// we need a new layer, which will configure its subtree
			LOG_LAYER( "enabling Layer for view '" << view->getName() );
			LOG_LAYER( "\t- reason: clip enabled" );
			auto layer = mGraph->makeLayer( view );
			layer->mClipEnabled = true;
			layer->configureViewList();
			return;
		}

	}

	if( mRootView->mRendersToFrameBuffer ) {
		Rectf frameBufferBounds = view->getBoundsForFrameBuffer();
		if( mFrameBufferBounds.getWidth() < frameBufferBounds.getWidth() && mFrameBufferBounds.getHeight() < frameBufferBounds.getHeight() ) {
			mFrameBufferBounds = ceil( frameBufferBounds );
			LOG_LAYER( "mFrameBufferBounds: " << mFrameBufferBounds );
		}
	}

	for( const auto &subview : view->getSubviews() ) {
		if( subview->isLayerRoot() ) {
			continue;
		}
		configureView( subview.get() );
	}
}

void Layer::draw( Renderer *ren )
{
	if( mRootView->mRendersToFrameBuffer ) {
		ivec2 frameBufferSize = ivec2( mFrameBufferBounds.getSize() );
		if( ! mFrameBuffer || mFrameBuffer->getSize().x < frameBufferSize.x || mFrameBuffer->getSize().y < frameBufferSize.y ) {
			LOG_LAYER( "aquiring FrameBuffer for view '" << mRootView->getName() << "', size: " << mFrameBufferBounds.getSize() );
			mFrameBuffer = ren->getFrameBuffer( frameBufferSize );
		}

		gl::context()->pushFramebuffer( mFrameBuffer->mFbo );
		gl::pushViewport( frameBufferSize );
		gl::pushMatrices();
		gl::setMatricesWindow( frameBufferSize );
		gl::translate( - mFrameBufferBounds.getUpperLeft() );

		gl::clear();
	}

	beginClip();

	drawView( mRootView, ren );

	endClip();

	if( mRootView->mRendersToFrameBuffer ) {
		gl::context()->popFramebuffer();
		gl::popViewport();
		gl::popMatrices();

		ren->pushBlendMode( BlendMode::PREMULT_ALPHA );
		ren->pushColor( ColorA::gray( 1, getAlpha() ) );

		auto destRect = mFrameBufferBounds + mRootView->getPos();
		gl::draw( mFrameBuffer->mFbo->getColorTexture(), destRect );
		ren->popColor();
		ren->popBlendMode();

//		writeImage( "framebuffer.png", mFrameBuffer->mFbo->getColorTexture()->createSource() );
	}
}

void Layer::drawView( View *view, Renderer *ren )
{
	gl::ScopedModelMatrix modelScope;

	if( view != mRootView || ! mRootView->mRendersToFrameBuffer )
		gl::translate( view->getPos() );

	view->drawImpl( ren );

	auto thisRef = shared_from_this();
	for( auto &subview : view->getSubviews() ) {
		if( subview->getLayer() == thisRef ) {
			drawView( subview.get(), ren );
		}
		else {
			subview->getLayer()->draw( ren );
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

Rectf Layer::getBoundsWorld() const
{
	return mRootView->getWorldBounds();
}

} // namespace ui

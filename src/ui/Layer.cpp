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
	CI_ASSERT_MSG( mRootView, "null pointer for root View" );
	LOG_LAYER( "root view: " << mRootView->getName() );
}

Layer::~Layer()
{
	LOG_LAYER( "bang" );
}

float Layer::getAlpha() const
{
	return mRootView->getAlpha();
}

void Layer::init()
{
	CI_ASSERT( ! mShouldRemove ); // TODO: can probably remove this as things changed a bit
	CI_ASSERT( mRootView->mLayer == shared_from_this() );

	LOG_LAYER( "mRootView: " << mRootView->getName() );

	if( mRootView->isTransparent() ) {
		if( ! mRootView->mRendersToFrameBuffer ) {
			LOG_LAYER( "enabling FrameBuffer for view '" << mRootView->getName() << "', size: " << mRootView->getSize() );
			LOG_LAYER( "\t- reason: alpha = " << mRootView->getAlpha() );
			mRootView->mRendersToFrameBuffer = true;
		}
	}
	else if( ! mRootView->mFilters.empty() ) {
		LOG_LAYER( "enabling FrameBuffer for view '" << mRootView->getName() << "', size: " << mRootView->getSize() );
		LOG_LAYER( "\t- reason: num filters = " << mRootView->mFilters.size() );
		mRootView->mRendersToFrameBuffer = true;

		CI_ASSERT( mFilterPassInfoList.empty() );
		for( auto &filter : mRootView->mFilters ) {
			Filter::PassInfo info;
			filter->configure( ivec2( mRootView->getSize() ), &info );
			mFilterPassInfoList.push_back( info );
		}
	}
	else {
		if( mFrameBuffer ) {
			// TODO: Consider removing, this path currently isn't reached as the Layer will be removed when View calls Graph::removeLayer().
			LOG_LAYER( "removing FrameBuffer for view '" << mRootView->getName() << "'" );
			LOG_LAYER( "\t- reason: alpha = " << mRootView->getAlpha() );
			mFrameBuffer.reset();
			mRootView->mRendersToFrameBuffer = false;
			mFrameBufferBounds = Rectf::zero();
			mGraph->removeLayer( shared_from_this() );
			return;
		}
	}
}

void Layer::update()
{
	updateView( mRootView );
}

void Layer::updateView( View *view )
{
	view->mIsIteratingSubviews = true;

	// update parents before children
	view->updateImpl();

	for( auto &subview : view->getSubviews() ) {
		if( subview->mMarkedForRemoval )
			continue;

		if( ! subview->mGraph )
			subview->mGraph = mGraph;

		updateView( subview.get() );
	}

	if( view->mLayer && view->mLayer.get() != this && ! view->mMarkedForRemoval ) {
		view->mLayer->update();
	}

	// make sure we have the right FrameBuffer size
	if( mRootView->mRendersToFrameBuffer ) {
		Rectf frameBufferBounds = view->getBoundsForFrameBuffer();
		if( mFrameBufferBounds.getWidth() < frameBufferBounds.getWidth() && mFrameBufferBounds.getHeight() < frameBufferBounds.getHeight() ) {
			mFrameBufferBounds = ceil( frameBufferBounds );
			LOG_LAYER( "mFrameBufferBounds: " << mFrameBufferBounds );
		}
	}

	view->mIsIteratingSubviews = false;
	view->clearViewsMarkedForRemoval();
}

void Layer::draw( Renderer *ren )
{
	if( mRootView->mRendersToFrameBuffer ) {
		ivec2 frameBufferSize = ivec2( mFrameBufferBounds.getSize() );
		if( ! mFrameBuffer || ! mFrameBuffer->isUsable() || mFrameBuffer->getSize().x < frameBufferSize.x || mFrameBuffer->getSize().y < frameBufferSize.y ) {
			mFrameBuffer = ren->getFrameBuffer( frameBufferSize );
			LOG_LAYER( "acquiring FrameBuffer for view '" << mRootView->getName() << ", size: " << mFrameBuffer->getSize()
			           << "', mFrameBufferBounds: " << mFrameBufferBounds << ", view bounds:" << mRootView->getBounds() );
		}

		ren->pushFrameBuffer( mFrameBuffer );
		gl::pushViewport( 0, mFrameBuffer->getHeight() - frameBufferSize.y, frameBufferSize.x, frameBufferSize.y );
		gl::pushMatrices();
		gl::setMatricesWindow( frameBufferSize );
		gl::translate( - mFrameBufferBounds.getUpperLeft() );

		gl::clear( ColorA::zero() );
	}

	drawView( mRootView, ren );

	if( mRootView->mRendersToFrameBuffer ) {
		ren->popFrameBuffer( mFrameBuffer );
		gl::popViewport();
		gl::popMatrices();

		// process all Filters before drawing FrameBuffer
		auto frameBuffer = mFrameBuffer;
		for( size_t i = 0; i < mRootView->mFilters.size(); i++ ) {
			auto &filter = mRootView->mFilters[i];
			const auto &passInfo = mFilterPassInfoList.at( i );
			Filter::Pass pass;
			pass.setIndex( i );
			// TODO: attach specified FrameBuffers
			filter->process( ren, pass );
		}

		ren->pushBlendMode( BlendMode::PREMULT_ALPHA );
		ren->pushColor( ColorA::gray( 1, getAlpha() ) );

		auto destRect = mFrameBufferBounds + mRootView->getPos();
		ren->draw( frameBuffer, Area( 0, 0, mFrameBufferBounds.getWidth(), mFrameBufferBounds.getHeight() ), destRect );
		ren->popColor();
		ren->popBlendMode();
	}
}

void Layer::drawView( View *view, Renderer *ren )
{
	if( view->isHidden() )
		return;

	if( view->isClipEnabled() )
		beginClip( view, ren );

	gl::ScopedModelMatrix modelScope;

	if( view != mRootView || ! mRootView->mRendersToFrameBuffer )
		gl::translate( view->getPos() );

	view->drawImpl( ren );

	for( auto &subview : view->getSubviews() ) {
		auto subviewLayer = subview->getLayer();
		if( subviewLayer ) {
			subviewLayer->draw( ren );
		}
		else {
			drawView( subview.get(), ren );
		}
	}

	if( view->isClipEnabled() )
		endClip();
}

void Layer::beginClip( View *view, Renderer *ren )
{
	Rectf viewWorldBounds = view->getWorldBounds();
	vec2 lowerLeft = viewWorldBounds.getLowerLeft();
	if( mRootView->mRendersToFrameBuffer ) {
		// get bounds of view relative to framebuffer. // TODO: need a method like convertPointToView( view, point );
		Rectf frameBufferWorldBounds = mRootView->getWorldBounds();
 		Rectf viewBoundsInFrameBuffer = viewWorldBounds - frameBufferWorldBounds.getUpperLeft();

		// Take lower left relative to FrameBuffer, which might actually be larger than mFrameBufferBounds
		lowerLeft = viewBoundsInFrameBuffer.getLowerLeft();
		lowerLeft.y = mFrameBuffer->getHeight() - lowerLeft.y;

		// TODO: reason through if this makes sense in general
		// - needed to add it when rendering to virtual canvas but stroked rect went beyond borders
		lowerLeft.y += mFrameBufferBounds.y1;
	}
	else {
		// rendering to window, flip y relative to Graph's bottom left using its clipping size
		ivec2 clippingSize = mRootView->getGraph()->getClippingSize();
		lowerLeft.y = clippingSize.y - lowerLeft.y;
	}

	auto ctx = gl::context();
	ctx->pushBoolState( GL_SCISSOR_TEST, GL_TRUE );
	ctx->pushScissor( std::pair<ivec2, ivec2>( lowerLeft, view->getSize() ) );
}

void Layer::endClip()
{
	auto ctx = gl::context();
	ctx->popBoolState( GL_SCISSOR_TEST );
	ctx->popScissor();
}

Rectf Layer::getBoundsWorld() const
{
	return mRootView->getWorldBounds();
}

} // namespace ui

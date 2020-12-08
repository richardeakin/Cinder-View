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

#include "vu/Layer.h"
#include "vu/Graph.h"
#include "vu/View.h"

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

} // anonymous namespace

namespace vu {

Layer::Layer( View *view )
	: mRootView( view )
{
	CI_ASSERT_MSG( mRootView, "null pointer for root View" );
	LOG_LAYER( hex << this << dec << " root view: " << mRootView->getName() );
}

Layer::~Layer()
{
	LOG_LAYER( hex << this << dec );
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

	bool needsFrameBuffer = false;
	if( mRootView->isTransparent() ) {
		needsFrameBuffer = true;
		if( ! mRootView->mRendersToFrameBuffer ) {
			LOG_LAYER( "enabling FrameBuffer for view '" << mRootView->getName() << "', size: " << mRootView->getSize() );
			LOG_LAYER( "\t- reason: alpha = " << mRootView->getAlpha() );
			mRootView->mRendersToFrameBuffer = true;
		}
	}
	if( ! mRootView->mFilters.empty() ) {
		LOG_LAYER( "enabling FrameBuffer for view '" << mRootView->getName() << "', size: " << mRootView->getSize() );
		LOG_LAYER( "\t- reason: num filters = " << mRootView->mFilters.size() );
		needsFrameBuffer = true;
		mRootView->mRendersToFrameBuffer = true;
		mFiltersNeedConfiguration = true;
	}

	if( ! needsFrameBuffer && mFrameBuffer ) {
		// TODO: Consider removing, this path currently isn't reached as the Layer will be removed when View calls Graph::removeLayer().
		LOG_LAYER( "removing FrameBuffer for view '" << mRootView->getName() << "'" );
		LOG_LAYER( "\t- reason: alpha = " << mRootView->getAlpha() );
		mFrameBuffer.reset();
		mRootView->mRendersToFrameBuffer = false;
		mRenderBounds = Rectf::zero();
		mGraph->removeLayer( shared_from_this() );
		return;
	}
}

void Layer::update()
{
	updateView( mRootView );
}

void Layer::updateView( View *view )
{
	// update parents before children
	const bool willLayout = view->needsLayout();
	if( willLayout && ! mRootView->mFilters.empty() ) {
		mFiltersNeedConfiguration = true;
	}

	view->updateImpl();

	view->mIsIteratingSubviews = true;
	for( auto &subview : view->getSubviews() ) {
		if( subview->mMarkedForRemoval )
			continue;

		if( ! subview->mGraph )
			subview->mGraph = mGraph;

		updateView( subview.get() );
	}
	view->mIsIteratingSubviews = false;
	view->clearViewsMarkedForRemoval();

	if( view->mLayer && view->mLayer.get() != this && ! view->mMarkedForRemoval ) {
		view->mLayer->update();
	}

	// If View::layout() was called, make sure we have the right FrameBuffer size
	if( mRootView->mRendersToFrameBuffer ) {
		Rectf frameBufferBounds = view->getBoundsForFrameBuffer();
		if( mRenderBounds.getWidth() < frameBufferBounds.getWidth() || mRenderBounds.getHeight() < frameBufferBounds.getHeight() ) {
			mRenderBounds = ceil( frameBufferBounds );
			LOG_LAYER( "mRenderBounds: " << mRenderBounds );
		}
	}
}

// rules for when we need a new main RenderBuffer:
// 1. We don't have one already
// 2. The one we have is currently in use (being rendered to)
// 3. The one we have isn't large enough (a View was resized)
void Layer::draw( Renderer *ren )
{
	if( mRootView->mRendersToFrameBuffer ) {
		ivec2 renderSize = ivec2( mRenderBounds.getSize() );
		if( renderSize.x == 0 || renderSize.y == 0 )
			return; // don't try to draw to a FrameBuffer if we don't have a valid bounds

		if( ! mFrameBuffer || mFrameBuffer->isInUse() || mFrameBuffer->getSize().x < renderSize.x || mFrameBuffer->getSize().y < renderSize.y ) {
			// acquire necessary FrameBuffers. TODO: setup Filter framebuffers here too?
			mFrameBuffer = ren->getFrameBuffer( renderSize );
			LOG_LAYER( "acquired main FrameBuffer for view '" << mRootView->getName() << "', size: " << mFrameBuffer->getSize()
			           << "', mRenderBounds: " << mRenderBounds << ", view bounds:" << mRootView->getBounds() );

			mFrameBuffer->setInUse( true ); // note: only so that the following LOG_LAYER prints correctly, this will be marked in use during the pushFrameBuffer()
			LOG_LAYER( "current frame buffers:\n" << ren->printCurrentFrameBuffersToString() );
		}

		ren->pushFrameBuffer( mFrameBuffer );
		gl::pushViewport( 0, mFrameBuffer->getHeight() - renderSize.y, renderSize.x, renderSize.y );

		// if scissor stack not empty, adjust and push another for the current viewport
		if( ! ren->mScissorStack.empty() ) {
			auto currentScissor = ren->mScissorStack.back();

			// convert mRenderBounds to world
			Rectf viewWorldBounds = mRootView->getWorldBounds();
			ivec2 clipLowerLeft = ivec2( viewWorldBounds.getLowerLeft() );

			// figure out current clip coordinates in our view space
			// rendering to window, flip y relative to Graph's bottom left using its clipping size
			ivec2 clippingSize = mRootView->getGraph()->getClippingSize();
			clipLowerLeft.y = clippingSize.y - clipLowerLeft.y;

			ivec2 translatedScissorLowerLeft = currentScissor.first - clipLowerLeft;
			translatedScissorLowerLeft = glm::max( ivec2( 0 ), translatedScissorLowerLeft ); // clamp to >= zero
			ivec2 clipSize = mRenderBounds.getSize(); // TODO: clamp to <= currentScissor

			ren->pushClip( translatedScissorLowerLeft, clipSize );
		}

		gl::pushMatrices();
		gl::setMatricesWindow( renderSize );
		gl::translate( - mRenderBounds.getUpperLeft() );

		gl::clear( ColorA::zero() );
	}

	// draw the subtree of Views that this Layer is responsible for
	drawView( mRootView, ren );

	// Do any necessary Filter processing and compositing
	if( mRootView->mRendersToFrameBuffer ) {
		gl::popMatrices();

		if( ren->mScissorStack.size() > 1 ) {
			// we pushed our own clip on the stack so pop that off
			ren->popClip();
		}

		gl::popViewport();
		ren->popFrameBuffer( mFrameBuffer );

		FrameBufferRef frameBuffer;
		if( ! mRootView->mFilters.empty() ) {
			processFilters( ren, mFrameBuffer );
			// set the FrameBuffer that should be drawn as texture to the last Pass of the last Filter
			frameBuffer = mRootView->mFilters.back()->mPasses.back().mFrameBuffer;
		}
		else {
			frameBuffer = mFrameBuffer;
		}

		ren->pushBlendMode( BlendMode::PREMULT_ALPHA );
		ren->pushColor( ColorA::gray( 1, getAlpha() ) );

		auto sourceArea = Area( ivec2( 0 ), ivec2( mRenderBounds.getSize() ) );
		auto destRect = mRenderBounds + mRootView->getPos();
		ren->draw( frameBuffer, sourceArea, destRect );
		ren->popColor();
		ren->popBlendMode();
	}
}

void Layer::drawView( View *view, Renderer *ren )
{
	if( view->isHidden() )
		return;

	if( view->isClipEnabled() ) {
		//CI_LOG_I( "beginClip: " << view->getName() );
		pushClip( view, ren );
	}

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

	if( view->isClipEnabled() ) {
		//CI_LOG_I( "endClip: " << view->getName() );
		ren->popClip();
	}
}

void Layer::processFilters( Renderer *ren, const FrameBufferRef &renderFrameBuffer )
{
	// mark the main FrameBuffer as in use while processing Filters, so it doesn't seem available when configuring
	renderFrameBuffer->setInUse( true );

	// call configure() for any Filters, updating its Pass information
	for( auto &filter : mRootView->mFilters ) {
		if( mFiltersNeedConfiguration ) {
			LOG_LAYER( "configuring Filters for View: '" << mRootView->getName() << "', Filter: '" << System::demangleTypeName( typeid( *filter ).name() ) << "'" );
			filter->mPasses.clear();
			Filter::PassInfo info;
			filter->configure( ivec2( mRenderBounds.getSize() ), &info );
			for( int i = 0; i < info.getCount(); i++ ) {
				filter->mPasses.push_back( Filter::Pass() );
				auto &pass = filter->mPasses.back();
				pass.setIndex( i );

				ivec2 requiredSize = info.getSize( i );
				pass.mFrameBuffer = ren->getFrameBuffer( requiredSize );
				pass.mSize = requiredSize;

				LOG_LAYER( "\t- acquired FrameBuffer for pass: " << i << ", size: " << pass.mFrameBuffer->getSize() << ", required size: " << requiredSize );

				// this marks the Pass's FrameBuffer as in use until after it is processed (end of for loop below)
				pass.mFrameBuffer->setInUse( true );

				LOG_LAYER( "\t- current frame buffers:\n" << ren->printCurrentFrameBuffersToString() );
				int blarg = 2;
			}
		}

		filter->mRenderFrameBuffer = renderFrameBuffer;

		for( auto &pass : filter->mPasses ) {
			ren->pushFrameBuffer( pass.mFrameBuffer );

			gl::ScopedViewport viewport( 0, pass.mFrameBuffer->getHeight() - pass.getSize().y, pass.getSize().x, pass.getSize().y );
			gl::ScopedMatrices matScope;
			gl::setMatricesWindow( pass.getSize() );
			
			// TODO: For each pass, need to specify how much padding is necessary
			// - things like blur need to go larger than mRenderBounds
			//gl::translate( - mRenderBounds.getUpperLeft() );

			filter->process( ren, pass );

			ren->popFrameBuffer( pass.mFrameBuffer );
		}
	}

	mFiltersNeedConfiguration = false;

	renderFrameBuffer->setInUse( false );
}

void Layer::pushClip( View *view, Renderer *ren )
{
	Rectf viewWorldBounds = view->getClipWorldBounds();
	vec2 clipLowerLeft = viewWorldBounds.getLowerLeft();
	vec2 clipSize = viewWorldBounds.getSize();
	if( mRootView->mRendersToFrameBuffer ) {
		// get bounds of view relative to framebuffer. // TODO: need a method like convertPointToView( view, point );
 		Rectf viewBoundsInFrameBuffer = viewWorldBounds - mRootView->getWorldPos();

		// Take lower left relative to FrameBuffer, which might actually be larger than mRenderBounds
		clipLowerLeft = viewBoundsInFrameBuffer.getLowerLeft();
		clipLowerLeft.y = mFrameBuffer->getHeight() - clipLowerLeft.y;

		// - needed to add it when rendering to virtual canvas but stroked rect went beyond borders
		clipLowerLeft.y += mRenderBounds.y1;
	}
	else {
		// rendering to window, flip y relative to Graph's bottom left using its clipping size
		ivec2 windowClippingSize = mRootView->getGraph()->getClippingSize();
		clipLowerLeft.y = windowClippingSize.y - clipLowerLeft.y;

		// TODO: make this general for both axes, and nested clip
		float rootX = mRootView->getPosX();
		if( clipLowerLeft.x < mRootView->getPosX() ) {

			float viewToLeft = mRootView->getPosX() - viewWorldBounds.x1;
			clipSize.x -= viewToLeft;
			clipLowerLeft.x = mRootView->getPosX();

		}
		float windowClipRight = rootX + mRootView->getWidth();
		if( viewWorldBounds.x2 > windowClipRight ) {
			clipSize.x -= viewWorldBounds.x2 - windowClipRight;
		}

		//CI_LOG_I( "view: " << view->getName() << ", clipLowerLeft: " << clipLowerLeft << ", root x: " << rootX << ", size: " << clipSize );

		clipSize.x = glm::max( clipSize.x, 0.0f );
		clipSize.y = glm::max( clipSize.y, 0.0f );
	}


	ren->pushClip( clipLowerLeft, clipSize );
}

Rectf Layer::getBoundsWorld() const
{
	return mRootView->getWorldBounds();
}

} // namespace vu

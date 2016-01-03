/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "ui/Graph.h"
#include "ui/View.h"

#include "cinder/app/AppBase.h"
#include "cinder/Log.h"

#define LOG_TOUCHES( stream )	CI_LOG_I( stream )
//#define LOG_TOUCHES( stream )	( (void)( 0 ) )

using namespace ci;
using namespace std;

namespace ui {

Graph::Graph( const ci::app::WindowRef &window )
	: mWindow( window )
{
	mGraph = this;

	// The Graph always gets a Layer because it is root.
	mLayer = makeLayer( this );

	if( ! mWindow ) {
		auto app = app::AppBase::get();
		if( ! app ) {
			throw GraphExc( "Running app-less, must provide an app::Window" );
		}

		mWindow = app->getWindow();
		mMultiTouchEnabled = app->isMultiTouchEnabled();
		setBounds( mWindow->getBounds() );
	}

	mRenderer = make_shared<ui::Renderer>();
}

Graph::~Graph()
{
	if( ! mEventConnections.empty() )
		disconnectEvents();
}

void Graph::setNeedsLayer( View *view )
{
	makeLayer( view );
}

LayerRef Graph::makeLayer( View *rootView )
{
	auto result = make_shared<Layer>( rootView );
	result->mGraph = this;
	rootView->mLayer = result;
	mLayers.push_back( result );

	result->init();
	return result;
}

void Graph::removeLayer( const LayerRef &layer )
{
	layer->markForRemoval();
	layer->getRootView()->mLayer = nullptr;
}

void Graph::layout()
{
	if( isFillParentEnabled() ) {
		setSize( mWindow->getSize() );
	}
}

void Graph::propagateUpdate()
{
	mLayer->update();

	for( auto layerIt = mLayers.begin(); layerIt != mLayers.end(); /* */ ) {
		auto &layer = *layerIt;

		if( layer->getShouldRemove() ) {
			CI_LOG_I( "removing layer" );
			layerIt = mLayers.erase( layerIt );
			continue;
		}
		else {
			++layerIt;
		}
	}
}

void Graph::propagateDraw()
{
	CI_ASSERT( getLayer() );

	mLayer->draw( mRenderer.get() );
}

// ----------------------------------------------------------------------------------------------------
// Events
// ----------------------------------------------------------------------------------------------------

void Graph::connectTouchEvents( int priority )
{
	mEventSlotPriority = priority;

	if( ! mEventConnections.empty() )
		disconnectEvents();

	if( mMultiTouchEnabled ) {
		mEventConnections.push_back( mWindow->getSignalTouchesBegan().connect( mEventSlotPriority,	[&]( app::TouchEvent &event ) {
			propagateTouchesBegan( event );
		} ) );
		mEventConnections.push_back( mWindow->getSignalTouchesMoved().connect( mEventSlotPriority,	[&]( app::TouchEvent &event ) {
			propagateTouchesMoved( event );
		} ) );
		mEventConnections.push_back( mWindow->getSignalTouchesEnded().connect( mEventSlotPriority,	[&]( app::TouchEvent &event ) {
			propagateTouchesEnded( event );
		} ) );
	}
	else {
		mEventConnections.push_back( mWindow->getSignalMouseDown().connect( mEventSlotPriority, [&]( app::MouseEvent &event ) {
			app::TouchEvent touchEvent( event.getWindow(), vector<app::TouchEvent::Touch>( 1, app::TouchEvent::Touch( event.getPos(), vec2( 0 ), 0, 0, &event ) ) );
			propagateTouchesBegan( touchEvent );
			event.setHandled( touchEvent.isHandled() );
		} ) );
		mEventConnections.push_back( mWindow->getSignalMouseDrag().connect( mEventSlotPriority, [&]( app::MouseEvent &event ) {
			app::TouchEvent touchEvent( event.getWindow(), vector<app::TouchEvent::Touch>( 1, app::TouchEvent::Touch( event.getPos(), vec2( 0 ), 0, 0, &event ) ) );
			propagateTouchesMoved( touchEvent );
			event.setHandled( touchEvent.isHandled() );
		} ) );
		mEventConnections.push_back( mWindow->getSignalMouseUp().connect( mEventSlotPriority, [&]( app::MouseEvent &event ) {
			app::TouchEvent touchEvent( event.getWindow(), vector<app::TouchEvent::Touch>( 1, app::TouchEvent::Touch( event.getPos(), vec2( 0 ), 0, 0, &event ) ) );
			propagateTouchesEnded( touchEvent );
			event.setHandled( touchEvent.isHandled() );
		} ) );
	}
}

void Graph::disconnectEvents()
{
	for( auto &connection : mEventConnections )
		connection.disconnect();

	mEventConnections.clear();
}

void Graph::propagateTouchesBegan( app::TouchEvent &event )
{
	mCurrentTouchEvent = event;
	auto thisRef = shared_from_this();
	propagateTouchesBegan( thisRef, event, 0 );
}

void Graph::propagateTouchesMoved( app::TouchEvent &event )
{
	mCurrentTouchEvent = event;
	auto thisRef = shared_from_this();
	propagateTouchesMoved( thisRef, event, 0 );
}

void Graph::propagateTouchesEnded( app::TouchEvent &event )
{
	mCurrentTouchEvent = event;
	auto thisRef = shared_from_this();
	propagateTouchesEnded( thisRef, event, 0 );
}

void Graph::propagateTouchesBegan( ViewRef &view, app::TouchEvent &event, size_t numTouchesHandled )
{
	if( view->isHidden() || ! view->isInteractive() )
		return;

	LOG_TOUCHES( view->getName() << " | num touches A: " << event.getTouches().size() );

	vector<app::TouchEvent::Touch> touchesInside;
	touchesInside.reserve( event.getTouches().size() );

	for( const auto &touch : event.getTouches() ) {
		vec2 pos = view->toLocal( touch.getPos() );
		if( view->hitTest( pos ) ) {
			touchesInside.push_back( touch );
		}
	}

	LOG_TOUCHES( view->getName() << " | num touchesInsde: " << touchesInside.size() ); // TODO: why is this 0 for TouchOverlayView?

	if( touchesInside.empty() )
		return;

	event.getTouches() = touchesInside;

	for( auto rIt = view->mSubviews.rbegin(); rIt != view->mSubviews.rend(); ++rIt ) {
		propagateTouchesBegan( *rIt, event, numTouchesHandled );
		if( event.isHandled() )
			return;
	}

	if( view->touchesBegan( event ) ) {
		// Only allow this View to handle this touch in other UI events.
		auto &touches = event.getTouches();
		for( auto &touch : touches ) {
			if( touch.isHandled() ) {
				view->mActiveTouches[touch.getId()] = touch;
				numTouchesHandled++;
			}

			LOG_TOUCHES( view->getName() << " | numTouchesHandled: " << numTouchesHandled );
		}

		// Remove active touches. Note: I'm having to do this outside of the above loop because I can't invalidate the vector::iterator
		touches.erase( remove_if( touches.begin(), touches.end(),
		                          [&view]( auto &touch ) {
			                          if( touch.isHandled() ) {
				                          LOG_TOUCHES( view->getName() << " | handled touch: " << touch.getId() );
				                          int blarg = 2;
			                          }
			                          return touch.isHandled();
		                          } ),
		               touches.end() );

		LOG_TOUCHES( view->getName() << " | num touches C: " << event.getTouches().size() );

		if( numTouchesHandled == mCurrentTouchEvent.getTouches().size() ) {
			event.setHandled();
		}

		LOG_TOUCHES( "handled: " << event.isHandled() );
	}
}

void Graph::propagateTouchesMoved( ViewRef &view, ci::app::TouchEvent &event, size_t numTouchesHandled )
{
	if( view->isHidden() || ! view->isInteractive() )
		return;

	// TODO NEXT: use mActiveTouches to avoid doing Touch copies for Views that don't have any active touches
	// - this is different to touchesBegan() since a parent view could not have any active touches, but it's child does

	LOG_TOUCHES( view->getName() << " | num touches A: " << event.getTouches().size() );

	// Filter touches to just those that are in mActiveTouches list
	if( ! view->mActiveTouches.empty() ) {
		vector<app::TouchEvent::Touch> touchesContinued;
		touchesContinued.reserve( view->mActiveTouches.size() );

		for( const auto &touch : event.getTouches() ) {
			vec2 pos = view->toLocal( touch.getPos() );
			if( view->mActiveTouches.count( touch.getId() ) != 0 ) {
				touchesContinued.push_back( touch );
			}
		}

		LOG_TOUCHES( view->getName() << " | num touchesContinued: " << touchesContinued.size() );

		if( touchesContinued.empty() )
			return;

		event.getTouches() = touchesContinued;
	}

	for( auto rIt = view->mSubviews.rbegin(); rIt != view->mSubviews.rend(); ++rIt ) {
		propagateTouchesMoved( *rIt, event, numTouchesHandled );
		if(	event.isHandled() )
			return;
	}

	// Remove active touches. Note: I'm having to do this outside of the above loop because I can't invalidate the vector::iterator
	// TODO: do I need this? touchesContinued should already be correctly filtered
//	auto &touches = event.getTouches();
//	touches.erase( remove_if( touches.begin(), touches.end(),
//	                          [&view]( auto &touch ) {
//		                          view->mActiveTouches[touch.getId()] = touch;
//		                          return touch.isHandled();
//	                          } ),
//	               touches.end() );

	if( touchesMoved( event ) ) {
		for( auto &touch : event.getTouches() ) {
			if( touch.isHandled() )
				numTouchesHandled++;
		}

		if( numTouchesHandled == mCurrentTouchEvent.getTouches().size() ) {
			event.setHandled();
		}

		LOG_TOUCHES( "handled: " << event.isHandled() );
	}
}

void Graph::propagateTouchesEnded( ViewRef &view, ci::app::TouchEvent &event, size_t numTouchesHandled )
{
	if( mHidden || ! mInteractive )
		return;

	auto &touches = event.getTouches();
	touches.erase( remove_if( touches.begin(), touches.end(),
	                          [&view]( auto &touch ) {
		                          return view->mActiveTouches.find( touch.getId() ) != view->mActiveTouches.end();
	                          } ),
	               touches.end() );

	if( touches.empty() )
		return;

	for( auto rIt = view->mSubviews.rbegin(); rIt != view->mSubviews.rend(); ++rIt ) {
		propagateTouchesEnded( *rIt, event, numTouchesHandled );
		if(	event.isHandled() )
			return;
	}

	touches.erase( remove_if( touches.begin(), touches.end(),
	                          [&view]( auto &touch ) {
		                          view->mActiveTouches[touch.getId()] = touch;
		                          return touch.isHandled();
	                          } ),
	               touches.end() );

	bool handled = touchesEnded( event );
	event.setHandled( handled );
}

} // namespace ui

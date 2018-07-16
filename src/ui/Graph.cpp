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

#include "cinder/app/AppBase.h"
#include "ui/Debug.h"

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
		setBounds( mWindow->toPixels( mWindow->getBounds() ) );
	}

	mRenderer = make_shared<ui::Renderer>();
}

Graph::~Graph()
{
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

void Graph::setClippingSize( const ci::ivec2 &size )
{
	mClippingSize = size;
	mClippingSizeSet = true;
}

//! Returns the size used for clipping operations in pixel coordinates. Defaults to the size of the window
ci::ivec2 Graph::getClippingSize() const
{
	return ( mClippingSizeSet ? mClippingSize : mWindow->toPixels( mWindow->getSize() ) );
}

void Graph::layout()
{
	if( isFillParentEnabled() ) {
		setSize( mWindow->toPixels( mWindow->getSize() ) );
	}
}

void Graph::propagateUpdate()
{
	// TODO: see note in Time section on allowing this to be customized.
	mCurrentTime = app::getElapsedSeconds();
	mCurrentFrame = app::getElapsedFrames();

	// Check if views should release their intercepting touches
	// - if yes, will allow subviews a chance at touchesBegan()
	for( auto viewIt = mViewsWithTouches.begin(); viewIt != mViewsWithTouches.end(); /* */ ) {
		const auto &view = *viewIt;
		if( ! view->mInterceptedTouchEvent.getTouches().empty() ) {
			UI_LOG_TOUCHES( view->getName() << " | updating intercepted touch" );
			if( handleInterceptingTouches( view, false ) ) {
				// view has released its intercepted event
				UI_LOG_TOUCHES( view->getName() << " | erasing." );
				view->mInterceptedTouchEvent = {};
				view->mActiveTouches.clear();
				viewIt = mViewsWithTouches.erase( viewIt ); // TODO (intercept): consider marking for removal and erasing later
				continue;
			}
		}

		++viewIt;
	}

	// Update the Layer tree, starting with the root
	mLayer->update();

	// Remove Layers marked for removal
	for( auto layerIt = mLayers.begin(); layerIt != mLayers.end(); /* */ ) {
		auto &layer = *layerIt;

		if( layer->getShouldRemove() ) {
			layerIt = mLayers.erase( layerIt );
			continue;
		}
		else {
			++layerIt;
		}
	}

	// Remove Views marked for removal
	mViewsWithTouches.erase(
			remove_if( mViewsWithTouches.begin(), mViewsWithTouches.end(),
			           []( const ViewRef &view ) {
				           return view->mMarkedForRemoval;
			           } ),
			mViewsWithTouches.end() );
}

void Graph::propagateDraw()
{
	CI_ASSERT( getLayer() );

	mLayer->draw( mRenderer.get() );
}

// ----------------------------------------------------------------------------------------------------
// Time
// ----------------------------------------------------------------------------------------------------
// TODO: offer way to customize the framerate for the graph, and possibly update functionality with
// fixed timestep ensured
// - could keep member vars for these and make them settable

double Graph::getTargetFrameRate() const
{
	return app::getFrameRate();
}

size_t Graph::getCurrentFrame() const
{
	return mCurrentFrame;
}

double Graph::getCurrentTime() const
{
	return mCurrentTime;
}

// ----------------------------------------------------------------------------------------------------
// Events
// ----------------------------------------------------------------------------------------------------

void Graph::connectEvents( const EventOptions &options )
{
	mEventSlotPriority = options.mPriority;
	mEventConnections.clear();

	if( options.mMouse ) {
		mEventConnections += mWindow->getSignalMouseDown().connect( mEventSlotPriority, [&]( app::MouseEvent &event ) {
			app::TouchEvent touchEvent( event.getWindow(), vector<app::TouchEvent::Touch>( 1, app::TouchEvent::Touch( event.getPos(), vec2( 0 ), 0, 0, &event ) ) );
			propagateTouchesBegan( touchEvent );
			event.setHandled( touchEvent.isHandled() );
			mPrevMousePos = event.getPos();
		} );
		mEventConnections += mWindow->getSignalMouseDrag().connect( mEventSlotPriority, [&]( app::MouseEvent &event ) {
			app::TouchEvent touchEvent( event.getWindow(), vector<app::TouchEvent::Touch>( 1, app::TouchEvent::Touch( event.getPos(), mPrevMousePos, 0, 0, &event ) ) );
			propagateTouchesMoved( touchEvent );
			event.setHandled( touchEvent.isHandled() );
			mPrevMousePos = event.getPos();
		} );
		mEventConnections += mWindow->getSignalMouseUp().connect( mEventSlotPriority, [&]( app::MouseEvent &event ) {
			app::TouchEvent touchEvent( event.getWindow(), vector<app::TouchEvent::Touch>( 1, app::TouchEvent::Touch( event.getPos(), mPrevMousePos, 0, 0, &event ) ) );
			propagateTouchesEnded( touchEvent );
			event.setHandled( touchEvent.isHandled() );
			mPrevMousePos = event.getPos();
		} );
	}

	if( options.mTouches ) {
		mEventConnections += mWindow->getSignalTouchesBegan().connect( mEventSlotPriority, [&]( app::TouchEvent &event ) {
			propagateTouchesBegan( event );
		} );
		mEventConnections += mWindow->getSignalTouchesMoved().connect( mEventSlotPriority, [&]( app::TouchEvent &event ) {
			propagateTouchesMoved( event );
		} );
		mEventConnections += mWindow->getSignalTouchesEnded().connect( mEventSlotPriority, [&]( app::TouchEvent &event ) {
			propagateTouchesEnded( event );
		} );
	}

	if( options.mKeyboard ) {
		mEventConnections += mWindow->getSignalKeyDown().connect( mEventSlotPriority, [&]( app::KeyEvent &event ) {
			propagateKeyDown( event );
		} );
		mEventConnections += mWindow->getSignalKeyUp().connect( mEventSlotPriority, [&]( app::KeyEvent &event ) {
			propagateKeyUp( event );
		} );
	}
}

void Graph::disconnectEvents()
{
	mEventConnections.clear();
}

void Graph::propagateTouchesBegan( app::TouchEvent &event )
{
	mCurrentTouchEvent = event;
	for( const auto &touch : event.getTouches() )
		mActiveTouches[touch.getId()] = touch;

	size_t numTouchesHandled = 0;
	ViewRef firstResponder;
	auto thisRef = shared_from_this();
	propagateTouchesBegan( thisRef, event, numTouchesHandled, firstResponder );

	UI_LOG_RESPONDER( "mFirstResponder: " << ( ! mFirstResponder ? "(none)" : mFirstResponder->getName() )
		<< ", firstResponder, : " << ( ! firstResponder ? "(none)" : firstResponder->getName() ) );
	
	if( mFirstResponder && mFirstResponder != firstResponder ) {
		Graph::resignFirstResponder(); // TODO: consider renaming this, as View also has a non-virtual resignFirstResponder() method.
	}

	if( firstResponder ) {
		setFirstResponder( firstResponder );
	}
}

void Graph::propagateTouchesBegan( const ViewRef &view, app::TouchEvent &event, size_t &numTouchesHandled, ViewRef &firstResponder )
{
	if( view->isHidden() || ! view->isInteractive() )
		return;

	UI_LOG_TOUCHES( view->getName() << " | num touches A: " << event.getTouches().size() );

	vector<app::TouchEvent::Touch> touchesInside;
	touchesInside.reserve( event.getTouches().size() );

	for( const auto &touch : event.getTouches() ) {
		vec2 pos = view->toLocal( touch.getPos() );
		if( view->hitTest( pos ) ) {
			touchesInside.push_back( touch );
			
			if( view->getAcceptsFirstResponder() )
				firstResponder = view;
		}
	}

	UI_LOG_TOUCHES( view->getName() << " | num touchesInsde: " << touchesInside.size() );

	if( touchesInside.empty() )
		return;

	// First allow current View to intercept the event (skip if already intercepted)
	event.getTouches() = touchesInside;
	bool intercepting = false;
	// TODO (intercept): I think the check for intercepted touches empty is problematic
	// - if another touch comes before that one is over, it won't be allowed the chance to intercept
	if( view->getInterceptsTouches() && view->mInterceptedTouchEvent.getTouches().empty() && view->shouldInterceptTouches( event ) ) {
		// store touches inside in separate 'intercepted location
		// TODO (intercept): might want to erase individual touches depending on if they were marked as handled
		view->mInterceptedTouchEvent = event;
		intercepting = true;
	}

	// Allow children views to handle non-intercepted event before the current view
	if( ! intercepting ) {
		// TODO (optimization): this copy is currently necessary to prevent bad iterators if a view is added during the subview touchesBegan()
		// - Might defer adding but need to think through how the ordering will be handled
		auto subviews = view->mSubviews;
		for( auto rIt = subviews.rbegin(); rIt != subviews.rend(); ++rIt ) {
			propagateTouchesBegan( *rIt, event, numTouchesHandled, firstResponder );
			if( event.isHandled() )
				return;
		}
	}

	if( view->touchesBegan( event ) ) {
		// Only allow this View to handle this touch in other UI events.
		auto &touches = event.getTouches();
		size_t numTouchesHandledThisView = 0;
		for( auto &touch : touches ) {
			if( touch.isHandled() ) {
				view->mActiveTouches[touch.getId()] = touch;
				numTouchesHandled++;
				numTouchesHandledThisView++;
				UI_LOG_TOUCHES( view->getName() << " | handled touch with id: " << touch.getId() << ", total handled: " << numTouchesHandled << ", in this view: " << numTouchesHandledThisView );
			}
		}

		// Remove active touches. Note: I'm having to do this outside of the above loop because I can't invalidate the vector::iterator
		touches.erase(
			remove_if( touches.begin(), touches.end(),
				[]( const app::TouchEvent::Touch &touch ) { return touch.isHandled(); } ),
			touches.end()
		);
		
		if( numTouchesHandledThisView != 0 && find( mViewsWithTouches.begin(), mViewsWithTouches.end(), view ) == mViewsWithTouches.end() ) {
			mViewsWithTouches.push_back( view );
		}

		// Allow other app signal connections to respond to the event with remaining touches if not all were handled
		event.setHandled( numTouchesHandled == mCurrentTouchEvent.getTouches().size() );

		UI_LOG_TOUCHES( view->getName() << " | num touches C: " << event.getTouches().size() << ", event handled: " << boolalpha << event.isHandled() << dec );
	}
}

void Graph::propagateTouchesMoved( app::TouchEvent &event )
{
	mCurrentTouchEvent = event;
	for( const auto &touch : event.getTouches() )
		mActiveTouches[touch.getId()] = touch;

	size_t numTouchesHandled = 0;

	for( auto &view : mViewsWithTouches ) {
		UI_LOG_TOUCHES( view->getName() << " | num touches A: " << event.getTouches().size() );

		// update intercepted touches
		if( ! view->mInterceptedTouchEvent.getTouches().empty() ) {
			for( auto &touch : view->mInterceptedTouchEvent.getTouches() ) {
				auto it = find_if( mCurrentTouchEvent.getTouches().begin(), mCurrentTouchEvent.getTouches().end(),
					[&touch]( const auto &t ) { return touch.getId() == t.getId(); }
				);

				if( it != mCurrentTouchEvent.getTouches().end() ) {
					touch = *it;
					UI_LOG_TOUCHES( view->getName() << " | intercepted touch updated with id: " << touch.getId() << ", pos: " << touch.getPos() );
				}
			}

			// call touchesMoved() on view intercepting the event
			auto interceptEvent = event;
			interceptEvent.getTouches() = view->mInterceptedTouchEvent.getTouches();
			//view->touchesMoved( interceptEvent );
		}

		// Update active touches
		if( ! view->mActiveTouches.empty() ) {
			vector<app::TouchEvent::Touch> touchesContinued;
			for( const auto &touch : mCurrentTouchEvent.getTouches() ) {
				auto it = view->mActiveTouches.find( touch.getId() );
				if( it == view->mActiveTouches.end() )
					continue;

				view->mActiveTouches[touch.getId()] = touch;
				touchesContinued.push_back( touch );
			}

			//UI_LOG_TOUCHES( view->getName() << " | num touchesContinued: " << touchesContinued.size() );

			if( ! touchesContinued.empty() ) {
				event.getTouches() = touchesContinued;
				view->touchesMoved( event );

				// for now always updating the active touch in touch map
				//for( auto &touch : event.getTouches() ) {
				//	if( touch.isHandled() ) {
				//		numTouchesHandled++;
				//		view->mActiveTouches.at( touch.getId() ) = touch;
				//	}
				//}
			}
		}
	}

	// Allow other app signal connections to respond to the event with remaining touches if not all were handled
	// TODO: probably need above commented out for loop to be enabled for this to work
	//		 - and to also account for intercepted touches
	//event.setHandled( numTouchesHandled == mCurrentTouchEvent.getTouches().size() );

	//UI_LOG_TOUCHES( "event handled: " << event.isHandled() );
}

void Graph::propagateTouchesEnded( app::TouchEvent &event, const ui::ViewRef &interceptingView )
{
	mCurrentTouchEvent = event; // TODO (intercept): may want to only set this if it isn't an intercepting event
//	size_t numTouchesHandled = 0;

	for( auto viewIt = mViewsWithTouches.begin(); viewIt != mViewsWithTouches.end(); /* */ ) {
		auto &view = *viewIt;
		UI_LOG_TOUCHES( view->getName() << " | num active touches: " << view->mActiveTouches.size() << ", intercepting touches: " << view->mInterceptedTouchEvent.getTouches().size() );
		//CI_ASSERT( ! view->mActiveTouches.empty() );

		// Update touches on view's mInterceptedTouchEvent
		bool intercepting = false;
		if( ! view->mInterceptedTouchEvent.getTouches().empty() ) {
			for( auto &touch : view->mInterceptedTouchEvent.getTouches() ) {
				auto it = find_if( mCurrentTouchEvent.getTouches().begin(), mCurrentTouchEvent.getTouches().end(),
					[&touch]( const auto &t ) { return touch.getId() == t.getId(); }
				);

				if( it != mCurrentTouchEvent.getTouches().end() ) {
					intercepting = true;
					touch = *it;
					UI_LOG_TOUCHES( view->getName() << " | intercepted touch updated with id: " << touch.getId() << ", pos: " << touch.getPos() );
				}
			}
		}

		// Update active touches
		if( ! view->mActiveTouches.empty() ) {
			vector<app::TouchEvent::Touch> touchesEnded;
			for( const auto &touch : mCurrentTouchEvent.getTouches() ) {
				auto touchIt = view->mActiveTouches.find( touch.getId() );
				if( touchIt == view->mActiveTouches.end() )
					continue;

				view->mActiveTouches[touch.getId()] = touch;
				touchesEnded.push_back( touch );
			}

			UI_LOG_TOUCHES( view->getName() << " | num touchesEnded: " << touchesEnded.size() );

			if( ! touchesEnded.empty() ) {
				event.getTouches() = touchesEnded;
				view->touchesEnded( event );

				for( const auto &touch : touchesEnded ) {
					view->mActiveTouches.erase( touch.getId() );
				}
			}

			// - if the view is intercepting a touch, then updateViewsInterceptingTouches() will clear it later
			if( intercepting ) {
				handleInterceptingTouches( view, true );
				//view->mInterceptedTouchEvent = {};
			}

		}

		// remove View from container once all its active touches have ended
		if( view != interceptingView && view->mActiveTouches.empty() ) {
			UI_LOG_TOUCHES( view->getName() << " | erasing." );
			viewIt = mViewsWithTouches.erase( viewIt );
		}
		else {
			++viewIt;
		}
	}

	for( const auto &touch : mCurrentTouchEvent.getTouches() ) {
		size_t numRemoved = mActiveTouches.erase( touch.getId() );
		//CI_VERIFY( numRemoved != 0 );
		if( numRemoved == 0 ) {
			CI_LOG_W( "stray touch attempted to be removed" );
		}
		UI_LOG_TOUCHES( "touch id: " << touch.getId() << ", num removed: " << numRemoved );
	}

	mCurrentTouchEvent.getTouches().clear();
}

// If eventEnding is true, will call propagateTouchesEnded(). Otherwise, released touches will allow subviews a chance at touchesBegan
// Returns true if view should be erased from mViewsWithTouches and the intercepted event was released.
// TODO (intercept): call this from touchesMoved() too
bool Graph::handleInterceptingTouches( const ViewRef &view, bool eventEnding )
{
	if( view->shouldStopInterceptingTouches( view->mInterceptedTouchEvent ) ) {
		// if the view claimed ownership of any touches, make it handle a touchesMoved()
		vector<app::TouchEvent::Touch>	handledTouches;
		for( const auto &touch : view->mInterceptedTouchEvent.getTouches() ) {
			if( touch.isHandled() ) {
				handledTouches.push_back( touch );
			}
		}

		UI_LOG_TOUCHES( view->getName() << " | intercepted touch stopped, handled touches: " << handledTouches.size() );

		// TODO (intercept): should only fire a touchesMoved if event isn't ending
		if( ! handledTouches.empty() ) {
			// TODO: I think I need to add the touches to this view's active touches first
			UI_LOG_TOUCHES( "\t- touches claimed: " << handledTouches.size() );
			auto claimedEvent = view->mInterceptedTouchEvent;
			claimedEvent.getTouches() = handledTouches;
			propagateTouchesMoved( claimedEvent );
		}

		// TODO (intercept): I think this doesn't work well if a view tries to give the touch to children but no one takes it
		// - in that case, probably wants to be a touchesMoved() on this view (if it isn't ending)
		auto beganEvent = view->mInterceptedTouchEvent;
		if( handledTouches.size() != beganEvent.getTouches().size() ) {
			// Erase any handled touches
			auto &touches = beganEvent.getTouches();
			touches.erase(
				remove_if( touches.begin(), touches.end(),
					[]( const app::TouchEvent::Touch &touch ) { return touch.isHandled(); } ),
				touches.end()
			);

			UI_LOG_TOUCHES( "\t- touches unclaimed: " << touches.size() );

			// give subview hierarchy a chance at the touches
			size_t numTouchesHandled = 0;
			ViewRef firstResponder;
			auto subviews = view->mSubviews;
			for( auto rIt = subviews.rbegin(); rIt != subviews.rend(); ++rIt ) {
				propagateTouchesBegan( *rIt, beganEvent, numTouchesHandled, firstResponder );
				if( beganEvent.isHandled() )
					break;
			}

		}

		// Handle event ending with original intercepted touches. TODO (intercept): use those handled in touches began only?
		// TODO (intercept): only make copy if eventEnding is true
		auto endedEvent = view->mInterceptedTouchEvent;
		view->mInterceptedTouchEvent = {}; // clear intercepted event for the view

		// If the intercept event was claimed by a child, call touches ended on it here.
		// TODO (intercept): should do this for any touches right below propagateTouchesBegan() above
		if( beganEvent.isHandled() ) {
			// If a subview handled the event from the intercepting view's touchesEnded, subview also gets a touchesEnded
			if( eventEnding ) {
				UI_LOG_TOUCHES( "\t- touches ending: " << endedEvent.getTouches().size() );
				propagateTouchesEnded( endedEvent, view );
			}
			return true;
		}
	}

	return false;
}

void Graph::propagateKeyDown( ci::app::KeyEvent &event )
{
	if( mFirstResponder ) {
		// tab / shift-tab to navigate next and previous responders
		if( event.getCode() == app::KeyEvent::KEY_TAB ) {
			if( event.isShiftDown() ) {
				moveToPreviousResponder();
			}
			else {
				moveToNextResponder();
			}
		}
		else {
			if( mFirstResponder->keyDown( event ) )
				event.setHandled();
		}
	}
	//auto thisRef = shared_from_this();
	//propagateKeyDown( thisRef, event );
}

void Graph::propagateKeyUp( ci::app::KeyEvent &event )
{
	if( mFirstResponder ) {
		if( mFirstResponder->keyUp( event ) )
			event.setHandled();
	}
	//auto thisRef = shared_from_this();
	//propagateKeyUp( thisRef, event );
}

// TODO: refactor, this is pretty kludgy in that it was moved from the ad-hoc Responder class that will soon be removed.
void Graph::setFirstResponder( const ViewRef &view )
{
	UI_LOG_RESPONDER( "view: " << view->getName() << ", current first responder: " << ( mFirstResponder ? mFirstResponder->getName() : "(none)" ) );
	if( view->willBecomeFirstResponder() ) {
		if( mFirstResponder && mFirstResponder != view ) {
			UI_LOG_RESPONDER( "\t\t- resigning first responder." );
			mFirstResponder->willResignFirstResponder(); // TODO: should this return false here if mFirstResponder returns false?
		}

		auto previousFirstResponder = mFirstResponder;
		mFirstResponder = view;
		mPreviousFirstResponder = previousFirstResponder;
	}
	else {
		// if view declines to become first responder, try the next in the chain.
		CI_LOG_I( "\t- view declined first responder: " << view->getName() );
		if( view->getNextResponder() ) {
			setFirstResponder( view->getNextResponder() );
		}
	}
}

void Graph::moveToNextResponder()
{
	UI_LOG_RESPONDER( "current first responder: " << ( mFirstResponder ? mFirstResponder->getName() : "(none)" ) );
	if( ! mFirstResponder )
		return;

	// find the next responder willing to accept responder status
	auto nextResponder = mFirstResponder->getNextResponder();
	while( nextResponder ) {
		UI_LOG_RESPONDER( "\t- next responder: " << nextResponder->getName() << ", accepts first responder: " << nextResponder->getAcceptsFirstResponder() );

		if( nextResponder->getAcceptsFirstResponder() )
			break;

		nextResponder = nextResponder->getNextResponder();
	}

	if( nextResponder ) {
		setFirstResponder( nextResponder );
	}
	else {
		UI_LOG_RESPONDER( "\t- no next responder, resigning current." );
		resignFirstResponder();
	}
}

void Graph::moveToPreviousResponder()
{
	UI_LOG_RESPONDER( "current first responder: " << ( mFirstResponder ? mFirstResponder->getName() : "(none)" ) << ", previous first responder: " <<  ( mPreviousFirstResponder ? mPreviousFirstResponder->getName() : "(none)" ) );
	
	// FIXME: this isn't good enough, only goes back one level
	// - this also needs to resign the current responder
	auto previousFirstResponder = mPreviousFirstResponder.lock();
	if( previousFirstResponder ) {
		setFirstResponder( previousFirstResponder );
	}
}

void Graph::resignFirstResponder()
{
	UI_LOG_RESPONDER( "current first responder: " << ( mFirstResponder ? mFirstResponder->getName() : "(none)" ) );
	if( ! mFirstResponder )
		return;

	UI_LOG_RESPONDER( "\t- resigning current responder." );
	if( mFirstResponder->getParent() ) {
		mPreviousFirstResponder = mFirstResponder;
	}
	mFirstResponder->willResignFirstResponder();
	mFirstResponder = nullptr;
}

#if 0
void Graph::propagateKeyDown( ViewRef &view, ci::app::KeyEvent &event )
{
	if( view->isHidden() || ! view->isInteractive() )
		return;

	for( auto rIt = view->mSubviews.rbegin(); rIt != view->mSubviews.rend(); ++rIt ) {
		propagateKeyDown( *rIt, event );
		if( event.isHandled() )
			return;
	}

	if( view->keyDown( event ) ) {
		event.setHandled();
	}
}

void Graph::propagateKeyUp( ViewRef &view, ci::app::KeyEvent &event )
{
	if( view->isHidden() || ! view->isInteractive() )
		return;

	for( auto rIt = view->mSubviews.rbegin(); rIt != view->mSubviews.rend(); ++rIt ) {
		propagateKeyUp( *rIt, event );
		if( event.isHandled() )
			return;
	}

	if( view->keyUp( event ) ) {
		event.setHandled();
	}
}
#endif
} // namespace ui

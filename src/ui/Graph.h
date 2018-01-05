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

#pragma once

#include "ui/Renderer.h"
#include "ui/Layer.h"
#include "ui/View.h"

#include "cinder/Cinder.h"
#include "cinder/Exception.h"
#include "cinder/Signals.h"

namespace cinder { namespace app {

typedef std::shared_ptr<class Window>   WindowRef;

} } // namespace ci::app

namespace ui {

typedef std::shared_ptr<class Graph>	GraphRef;
typedef std::shared_ptr<class View>		ViewRef;

//! This is where it all starts! Construct a Graph as the root of your UI scene graph, add other views to it.
class CI_UI_API Graph : public View {
  public:
	Graph( const ci::app::WindowRef &window = nullptr );
	~Graph();

	RendererRef getRenderer()       { return mRenderer; }
	RendererRef getRenderer() const { return mRenderer; }

	ci::app::WindowRef	getWindow() const	{ return mWindow; }

	void    setNeedsLayer( View *view );
	void    removeLayer( const LayerRef &layer );

	const std::list<LayerRef>& getLayers() const   { return mLayers; }

	void propagateUpdate();
	void propagateDraw();

	void propagateTouchesBegan( ci::app::TouchEvent &event );
	void propagateTouchesMoved( ci::app::TouchEvent &event );
	void propagateTouchesEnded( ci::app::TouchEvent &event );

	void propagateKeyDown( ci::app::KeyEvent &event );
	void propagateKeyUp( ci::app::KeyEvent &event );

	//! Sets the View that current receives Responder events (ex. keys)
	void setFirstResponder( const ViewRef &view );
	//! Moves to the next responder in the responder chain if there is one, resigning any current responder.
	void moveToNextResponder();
	//! Moves to the previous responder in the responder chain if there is one, resigning any current responder.
	void moveToPreviousResponder();
	//! Resigns current View that receives Responder events, ex. when Esc or Return keys are hit.
	void resignFirstResponder();
	//! Returns the View that currently receives first responder status
	 const ViewRef&	getFirstResponder() const	{ return mFirstResponder; }

	struct EventOptions {
		EventOptions() {}

		EventOptions& mouse( bool enable = true )		{ mMouse = enable; return *this; }
		EventOptions& touches( bool enable = true )		{ mTouches = enable; return *this; }
		EventOptions& keyboard( bool enable = true )	{ mKeyboard = enable; return *this; }
		EventOptions& priority( int priority )			{ mPriority = priority; return *this; }

	  private:
		bool	mMouse		= true;
		bool	mTouches	= true;
		bool	mKeyboard	= true;
		int		mPriority	= -1;

		friend class Graph;
	};

	//! Connects this View's event propagation methods to the Window's event signals
	void connectEvents( const EventOptions &options = EventOptions() );
	//! Disconnects all event propagation methods.
	void disconnectEvents();

	//! Returns a map of all current touches in the window (key = touch id).
	const std::map<uint32_t, ci::app::TouchEvent::Touch>&  getAllTouchesInWindow() const   { return mActiveTouches; }
	//! Returns the current TouchEvent, if one is currently being processed.
	const ci::app::TouchEvent&  getCurrentTouchEvent() const    { return mCurrentTouchEvent; }
	//! Returns all Views that currently have active touches.
	const std::list<ViewRef>&	    getViewsWithTouches() const { return mViewsWithTouches; }

	//! Sets the size used for clipping operations.
	void setClippingSize( const ci::ivec2 &size );
	//! Returns the size used for clipping operations. Defaults to the size of the window
	ci::ivec2 getClippingSize() const;

	//!
	double	getTargetFrameRate() const;
	//!
	double	getElapsedSeconds() const;

  protected:
	void layout() override;

  private:
	LayerRef makeLayer( View *rootView );

	void propagateTouchesBegan( ViewRef &view, ci::app::TouchEvent &event, size_t &numTouchesHandled, ViewRef &firstResponder );

#if 0
	void propagateKeyDown( ViewRef &view, ci::app::KeyEvent &event );
	void propagateKeyUp( ViewRef &view, ci::app::KeyEvent &event );
#endif

	RendererRef         mRenderer;
	ci::app::WindowRef  mWindow;
	bool                mMultiTouchEnabled = false;
	ci::app::TouchEvent mCurrentTouchEvent;
	int					mEventSlotPriority = 1;
	ci::ivec2			mClippingSize;
	bool				mClippingSizeSet = false;

	ci::signals::ConnectionList				mEventConnections;
	ci::vec2								mPrevMousePos;

	std::list<LayerRef>	    mLayers;
	std::list<ViewRef>	    mViewsWithTouches;
	ViewRef					mFirstResponder, mPreviousFirstResponder;

	friend class Layer;
};

class CI_UI_API GraphExc : public ci::Exception {
  public:
	GraphExc( const std::string &description )
		: Exception( description )
	{}
};

} // namespace ui

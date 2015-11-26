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
class Graph : public View {
  public:
	Graph( const ci::app::WindowRef &window = nullptr );
	~Graph();

	RendererRef getRenderer()       { return mRenderer; }
	RendererRef getRenderer() const { return mRenderer; }

	void propagateUpdate();
	void propagateDraw();

	//! Connects this View's touches propagation methods to the Window's touch event signals
	void connectTouchEvents( int prioririty = 1 );
	//! Disconnects touches propagation methods.
	void disconnectEvents();

  private:
	RendererRef         mRenderer;
	ci::app::WindowRef  mWindow;
	bool                mMultiTouchEnabled = false;
	int					mEventSlotPriority = 1;

	std::vector<ci::signals::Connection>	mEventConnections;
};

class GraphExc : public ci::Exception {
  public:
	GraphExc( const std::string &description )
		: Exception( description )
	{}
};

} // namespace ui

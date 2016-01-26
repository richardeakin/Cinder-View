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
#include "ui/Filter.h"

#include <memory>

namespace ui {

class Graph;
class View;

typedef std::shared_ptr<class Layer>		LayerRef;
typedef std::shared_ptr<class FrameBuffer>	FrameBufferRef;

//! A Layer controls specific rendering capabilities for Views, such as compositing with translucency.
class Layer : public std::enable_shared_from_this<Layer> {
  public:
	Layer( View *view );
	virtual ~Layer();

	float	getAlpha() const;

	FrameBufferRef  getFrameBuffer() const      { return mFrameBuffer; }

	View*   getRootView() const         { return mRootView; }

	bool getShouldRemove() const         { return mShouldRemove; }

	ci::Rectf   getBoundsWorld() const;

  private:

	void update();
	void draw( Renderer *ren );

	void markForRemoval()               { mShouldRemove = true; }
	void init();
	void updateView( View *view );
	void drawView( View *view, Renderer *ren );
	void beginClip( View *view, Renderer *ren );
	void endClip();

	View*           mRootView;
	Graph*          mGraph;
	FrameBufferRef	mFrameBuffer;
	ci::Rectf       mFrameBufferBounds = ci::Rectf::zero();

	bool            mShouldRemove = false;

	friend class Graph;
};

} // namespace ui

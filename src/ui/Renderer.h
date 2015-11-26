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

#include "cinder/Cinder.h"
#include "cinder/Color.h"
#include "cinder/Rect.h"

namespace ui {

typedef std::shared_ptr<class Renderer>     RendererRef;

class Renderer {
  public:
	//! Sets the current color used for rendering
	void setColor( const ci::ColorA &color );
	//! Stores the current color.
	void pushColor();
	//! Makes \a color the current color used when rendering, first storing the current color.
	void pushColor( const ci::ColorA &color );
	//! Restores the color to what was previously set before the last pushColor().
	void popColor();
	//! Draws a solid rectangle with dimensions \a rect.
	void drawSolidRect( const ci::Rectf &rect );
	//! Draws a stroked rectangle with dimensions \a rect.
	void drawStrokedRect( const ci::Rectf &rect );
	//! Draws a stroked rectangle centered around \a rect, with a line width of \a lineWidth
	void drawStrokedRect( const ci::Rectf &rect, float lineWidth );

  private:
	std::vector<ci::ColorA>		mColorStack;
};

} // namespace ui

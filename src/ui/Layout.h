/*
 Copyright (c) 2016, The Cinder Project, All rights reserved.

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

#pragma once

#include "cinder/Rect.h"
#include <memory>

namespace ui {

class View;

typedef std::shared_ptr<class Layout>	LayoutRef;

class Layout {
  public:

	virtual void layout( View *view )	{}

  private:
};

enum class Alignment : uint8_t {
	Minimum = 0,
	Middle,
	Maximum,
	Fill
};

/// The direction of data flow for a layout.
enum class Orientation {
	Horizontal = 0,
	Vertical
};

class BoxLayout : public Layout {
public:
	BoxLayout( Orientation orientation, Alignment alignment = Alignment::Middle );

	Orientation orientation() const { return mOrientation; }
	void setOrientation( Orientation orientation ) { mOrientation = orientation; }

	Alignment alignment() const { return mAlignment; }
	void setAlignment( Alignment alignment ) { mAlignment = alignment; }

	void setMargin( const ci::Rectf &margin ) { mMargin = margin; }
	void setPadding( float padding ) { mPadding = padding; }

	void layout( View *view ) override;
protected:
	Orientation	mOrientation;
	Alignment	mAlignment;
	ci::Rectf	mMargin = ci::Rectf( 0, 0, 0, 0 );
	float		mPadding = 0;
};

} // namespace ui

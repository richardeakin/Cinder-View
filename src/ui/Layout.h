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

typedef std::shared_ptr<class Layout>				LayoutRef;
typedef std::shared_ptr<class BoxLayout>			BoxLayoutRef;
typedef std::shared_ptr<class LinearLayout>			LinearLayoutRef;
typedef std::shared_ptr<class VerticalLayout>		VerticalLayoutRef;
typedef std::shared_ptr<class HorizontalLayout>		HorizontalLayoutRef;

class Layout {
  public:

	virtual void layout( View *view )	{}

  private:
};

/// The direction of data flow for a layout.
enum class Orientation {
	Horizontal = 0,
	Vertical
};

enum class Alignment : uint8_t {
	Minimum = 0,
	Middle,
	Maximum,
	Fill,
	NumAlignments
};

class LinearLayout : public Layout {
public:
	enum class Mode : uint8_t {
		Increment = 0, //! Each successive View is placed after the previous one + margin
		Fill, //! Spreads and expands subiews, overriding each subview's size.
		Distribute, //! Spreads subviews equally based on their center. Ignores padding.
		NumModes
	};

	explicit LinearLayout( Orientation orientation, Mode mode = Mode::Increment, Alignment alignment = Alignment::Minimum );

	Orientation orientation() const { return mOrientation; }
	void setOrientation( Orientation orientation ) { mOrientation = orientation; }

	Alignment getAlignment() const { return mAlignment; }
	void setAlignment( Alignment alignment ) { mAlignment = alignment; }

	//! Sets the mode used for laying out views
	void setMode( Mode mode )	{ mMode = mode; }
	//! Returns the mode used for laying out views
	Mode	getMode() const		{ return mMode; }

	//! Sets the margin around the bounds of the Layout object
	void setMargin( const ci::Rectf &margin ) { mMargin = margin; }
	//! Sets the padding between successive Views.
	void setPadding( float padding ) { mPadding = padding; }

	void layout( View *view ) override;
protected:
	Orientation	mOrientation;
	Alignment	mAlignment;
	Mode		mMode;
	//! Spacing surrounding the suviews.
	ci::Rectf	mMargin = ci::Rectf( 0, 0, 0, 0 );
	//! Spacing between the subviews. Used by the linear layout mode.
	float		mPadding = 0;
};

class VerticalLayout : public LinearLayout {
  public:
	explicit VerticalLayout( Mode mode = Mode::Increment, Alignment alignment = Alignment::Minimum )
		: LinearLayout( Orientation::Vertical, mode, alignment )
	{}
};

class HorizontalLayout : public LinearLayout {
  public:
	explicit HorizontalLayout( Mode mode = Mode::Increment, Alignment alignment = Alignment::Minimum )
		: LinearLayout( Orientation::Horizontal, mode, alignment )
	{}
};

} // namespace ui

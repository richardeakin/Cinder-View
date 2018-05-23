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

#pragma once

#include "ui/View.h"
#include "ui/Image.h"

namespace ui {

typedef std::shared_ptr<class ImageView>	ImageViewRef;

enum ImageScaleMode {
	FIT,
	FIT_HORIZONTAL,
	FILL,
	CENTER,
	CENTER_HORIZONTAL,
	NONE,
	NUM_MODES
};

//! Draws an Image with various scale modes. Non-interactive by default.
class CI_UI_API ImageView : public View {
  public:

	ImageView( const ci::Rectf &bounds = ci::Rectf::zero() );

	void			setImage( const ImageRef &image );
	ImageRef		getImage() const	{ return mImage; }

	void			setScaleMode( ImageScaleMode mode )	{ mScaleMode = mode; }
	ImageScaleMode	getScaleMode() const				{ return mScaleMode; }

	//! Returns the destination Rect in this ImageView's coordinate system.
	ci::Rectf		getDestRectLocal() const;

	void					setColor( const ci::Color &color )	{ mColor = color; }
	const ci::Color&		getColor() const					{ return mColor; }
	ci::Anim<ci::Color>*	getColorAnim()						{ return &mColor; }

  protected:
	void draw( Renderer *ren ) override;

  private:
	ImageRef				mImage;
	ImageScaleMode			mScaleMode = ImageScaleMode::FIT;
	ci::Anim<ci::Color>		mColor = ci::Color::white();
};


//! Returns scaleMode in string representation.
CI_UI_API std::string toString( const ImageScaleMode &scaleMode );
//! Stream support for ScaleMode
CI_UI_API std::ostream& operator<<( std::ostream &os, const ImageScaleMode &rhs );

} // namespace ui
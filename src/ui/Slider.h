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

#include "ui/Control.h"
#include "ui/TextManager.h"

namespace ui {

typedef std::shared_ptr<class HSlider>	HSliderRef;
typedef std::shared_ptr<class VSlider>	VSliderRef;

class SliderBase : public Control {
public:
	SliderBase( const ci::Rectf &bounds = ci::Rectf::zero() );

	void setMin( float min );
	void setMax( float max );

	float getMin() const	{ return mMin; }
	float getMax() const	{ return mMax; }

	void				setTitle( const std::string &title )	{ mTitle = title; }
	const std::string&	getTitle() const						{ return mTitle; }

	float getValue() const	{ return mValue; }

	void setValue( float value, bool emitChanged = true );

	void setValueColor( const ci::ColorA &color )	{ mValueColor = color; }
	void setTitleColor( const ci::ColorA &color )	{ mTitleColor = color; }

	void setSnapToIntEnabled( bool enable )	{ mSnapToInt = enable; }
	bool isSnapToIntEnabled() const			{ return mSnapToInt; }

protected:
	void draw( Renderer *ren )	override;

	bool touchesBegan( ci::app::TouchEvent &event )	override;
	bool touchesMoved( ci::app::TouchEvent &event )	override;
	bool touchesEnded( ci::app::TouchEvent &event )	override;

	virtual float getValuePercentage( const ci::vec2 &pos )							= 0;
	virtual ci::Rectf	getValueRect( float sliderPos, float sliderRadius ) const	= 0;

	std::string	getTitleLabel() const;

	void updateSliderPos();
	void updateValue( const ci::vec2 &pos );

private:
	float	mValue = 0;
	float	mMin = 0;
	float	mMax = 1;

	float		mSliderPos = 0;
	float		mValueThickness = 2;
	bool        mSnapToInt = false;
	ci::ColorA	mValueColor = ci::ColorA::gray( 1, 0.4f );
	ci::ColorA	mTitleColor = ci::ColorA::gray( 1, 0.6f );
	std::string	mTitle;
	TextRef		mTextLabel;
};

class HSlider : public SliderBase {
public:
	HSlider( const ci::Rectf &bounds = ci::Rectf::zero() ) : SliderBase( bounds )	{}

protected:
	ci::Rectf	getValueRect( float sliderPos, float sliderRadius ) const	override;
	float		getValuePercentage( const ci::vec2 &pos )					override;
};

class VSlider : public SliderBase {
public:
	VSlider( const ci::Rectf &bounds = ci::Rectf::zero() ) : SliderBase( bounds )	{}

protected:
	ci::Rectf	getValueRect( float sliderPos, float sliderRadius ) const	override;
	float		getValuePercentage( const ci::vec2 &pos )					override;
};

} // namespace ui

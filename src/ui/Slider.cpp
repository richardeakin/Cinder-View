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

#include "ui/Slider.h"

#include "cinder/Log.h"
#include "cppformat/format.h"

using namespace std;
using namespace ci;

namespace ui {

SliderBase::SliderBase( const Rectf &bounds )
	: Control( bounds )
{
	mTextLabel = TextManager::loadText( FontFace::NORMAL );

	// set a default background color
	getBackground()->setColor( Color::black() );
}

void SliderBase::setMin( float min )
{
	mMin = min;
	if( mValue < min )
		mValue = min;

	updateSliderPos();
}

void SliderBase::setMax( float max )
{
	mMax = max;
	if( mValue > max )
		mValue = max;

	updateSliderPos();
}

void SliderBase::setValue( float value, bool emitChanged )
{
	mValue = value;
	if( mSnapToInt )
		mValue = roundf( mValue );

	updateSliderPos();

	if( emitChanged )
		mSignalValueChanged.emit();
}

void SliderBase::draw( Renderer *ren )
{
	const float sliderRadius = mValueThickness / 2;
	const Rectf valRect = getValueRect( mSliderPos, sliderRadius );
	const float padding = 6;

	ren->setColor( mValueColor );
	ren->drawSolidRect( valRect );

	ren->setColor( mTitleColor );
	mTextLabel->drawString( getTitleLabel(), vec2( padding, getCenterLocal().y + mTextLabel->getDescent() ) );
}

std::string	SliderBase::getTitleLabel() const
{
	std::string result = mTitle;
	if( ! result.empty() )
		result += ": ";

	result += fmt::format( "{}", getValue() );
	return result;
}


bool SliderBase::touchesBegan( app::TouchEvent &event )
{
	for( auto &touch : event.getTouches() ) {
		vec2 pos = toLocal( touch.getPos() );
		if( hitTest( pos ) ) {
			setTouchCanceled( false );

			CI_LOG_V( "[" << getName() << "] pos: " << pos << ", num touches: " << event.getTouches().front() );

			updateValue( pos );
			touch.setHandled();
			return true;
		}

	}

	return false;
}

bool SliderBase::touchesMoved( app::TouchEvent &event )
{
	if( isTouchCanceled() )
		return false;

	vec2 pos = toLocal( event.getTouches().front().getPos() );
	if( ! hitTestInsideCancelPadding( pos ) ) {
		setTouchCanceled( true );
		return false;
	}

	CI_LOG_V( "[" << getName() << "] pos: " << pos << ", num touches: " << event.getTouches().front() );

	updateValue( pos );
	return true;
}

bool SliderBase::touchesEnded( app::TouchEvent &event )
{
	if( isTouchCanceled() )
		return false;

	vec2 pos = toLocal( event.getTouches().front().getPos() );
	if( ! hitTestInsideCancelPadding( pos ) ) {
		setTouchCanceled( true );
		return false;
	}

	CI_LOG_V( "[" << getName() << "] pos: " << pos << ", num touches: " << event.getTouches().front() );

	updateValue( pos );
	return true;
}

void SliderBase::updateSliderPos()
{
	float range = mMax - mMin;
	if( fabs( range ) <= 0.00001f )
		mSliderPos = 0;
	else
		mSliderPos = constrain<float>( ( mValue - mMin ) / range, 0, 1 );
}

void SliderBase::updateValue( const ci::vec2 &pos )
{
	mSliderPos = constrain<float>( getValuePercentage( pos ), 0, 1 );

	float prevValue = mValue;
	mValue = ( mMax - mMin ) * mSliderPos + mMin;
	if( mSnapToInt )
		mValue = roundf( mValue );

	if( mValue != prevValue )
		mSignalValueChanged.emit();
}

float HSlider::getValuePercentage( const ci::vec2 &pos )
{
	return getWidth() < 0.00001f ? 0 : pos.x / getWidth();
}

float VSlider::getValuePercentage( const ci::vec2 &pos )
{
	return getHeight() < 0.00001f ? 0 : ( 1 - pos.y / getHeight() );
}

Rectf HSlider::getValueRect( float sliderPos, float sliderRadius ) const
{
	float offset = sliderPos * getWidth();
	return Rectf( offset - sliderRadius, 0, offset + sliderRadius, getHeight() );

}

Rectf VSlider::getValueRect( float sliderPos, float sliderRadius ) const
{
	float offset = ( 1 - sliderPos ) * getHeight();
	return Rectf( 0, offset - sliderRadius, getWidth(), offset + sliderRadius );
}

} // namespace ui

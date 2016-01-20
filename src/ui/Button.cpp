/*
 Copyright (c) 2014-15, Richard Eakin - All rights reserved.

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

#include "ui/Button.h"

using namespace ci;
using namespace std;

namespace ui {

Button::Button( const Rectf &bounds )
	: Control( bounds )
{
	mTextTitle = TextManager::loadText( FontFace::NORMAL );
}

const ColorA& Button::getColorForState( Button::State state ) const
{
	switch( state ) {
		case State::NORMAL:
			return mColorNormal;
			break;
		case State::ENABLED:
			return mColorEnabled;
			break;
		case State::PRESSED:
			return mColorPressed;
			break;
	}

	return mColorNormal;
}

const string& Button::getTitleForState( State state ) const
{
	if( state == State::ENABLED && ! mTitleEnabled.empty() )
		return mTitleEnabled;
	else
		return mTitleNormal;
}

const ColorA& Button::getTitleColorForState( State state ) const
{
	if( state == State::ENABLED && mHasColorTitleEnabled )
		return mColorTitleEnabled;
	else
		return mColorTitleNormal;
}

void Button::draw( Renderer *ren )
{
	if( mHasBackgroundColor ) {
		// draw background
		ren->setColor( getColor() );
		ren->drawSolidRect( getBoundsLocal() );
	}

	if( mImageNormal ) {
		// draw image
		ren->draw( mImageNormal, getBoundsLocal() );
	}
	else {
		// draw title
		const float padding = 6;
		ren->setColor( getTitleColor() );
		mTextTitle->drawString( getTitle(), vec2( padding, getCenterLocal().y + mTextTitle->getDescent() ) );
	}
}

void Button::setEnabled( bool enabled )
{
	mEnabled = enabled;
	mState = enabled ? State::ENABLED : State::NORMAL;
}

void Button::setTitle( const string &title, State state )
{
	if( state == State::NORMAL )
		mTitleNormal = title;
	else
		mTitleEnabled = title;
}

void Button::setTitleColor( const ci::ColorA &color, State state )
{
	if( state == State::ENABLED ) {
		mColorTitleEnabled = color;
		mHasColorTitleEnabled = true;
	}
	else
		mColorTitleNormal = color;
}

void Button::setColor( const ci::ColorA &color, State state )
{
	mHasBackgroundColor = true;

	switch( state ) {
		case State::NORMAL: mColorNormal = color; return;
		case State::ENABLED: mColorEnabled = color; return;
		case State::PRESSED: mColorPressed = color; return;
		default: CI_ASSERT_NOT_REACHABLE();
	}
}

void Button::setImage( const ui::ImageRef &image, State state )
{
	mImageNormal = image;
}

bool Button::touchesBegan( app::TouchEvent &event )
{
	mState = State::PRESSED;
	setTouchCanceled( false );
	
	mSignalPressed.emit();
	event.getTouches().front().setHandled();
	return true;
}

bool Button::touchesMoved( app::TouchEvent &event )
{
	if( isTouchCanceled() )
		return false;

	vec2 pos = toLocal( event.getTouches().front().getPos() );
	if( ! hitTestInsideCancelPadding( pos ) ) {
		setTouchCanceled( true );
		mState = State::NORMAL;
	}

	return true;
}

bool Button::touchesEnded( app::TouchEvent &event )
{
	if( isTouchCanceled() )
		return false;
	
	vec2 pos = toLocal( event.getTouches().front().getPos() );
	if( ! hitTestInsideCancelPadding( pos ) ) {
		setTouchCanceled( true );
		mState = State::NORMAL;
	}
	else {
		bool enable = isToggle() ? ! isEnabled() : false;
		setEnabled( enable );

		mSignalReleased.emit();
	}

	return true;
}

} // namespace ui

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

#pragma once

#include "ui/Control.h"
#include "ui/Image.h"
#include "ui/TextManager.h"

namespace ui {

typedef std::shared_ptr<class Button>	ButtonRef;
typedef std::shared_ptr<class CheckBox>	CheckBoxRef;

class Button : public Control {
public:
	enum class State { NORMAL, ENABLED, PRESSED };

	Button( const ci::Rectf &bounds = ci::Rectf::zero() );

	void setEnabled( bool enabled = true );
	bool isEnabled() const					{ return mEnabled; }
	bool getValue() const					{ return isEnabled(); }

	void setAsToggle( bool toggle = true )	{ mIsToggle = toggle; }
	bool isToggle() const					{ return mIsToggle; }

	State	getState() const	{ return mState; }

	void setTitle( const std::string &title, State state = State::NORMAL );
	void setTitleColor( const ci::ColorA &color, State state = State::NORMAL );
	void setColor( const ci::ColorA &color, State state = State::NORMAL );

	//! Sets an Image to represent the Button. If added then title won't be used.
	//! TODO: only State::Normal is supported right now, add enabled and pressed too
	void setImage( const ui::ImageRef &image, State state = State::NORMAL );

	const ci::ColorA&	getColorForState( State state ) const;
	const std::string&	getTitleForState( State state ) const;
	const ci::ColorA&	getTitleColorForState( State state ) const;
	ImageRef			getImageForState( State state ) const;

	const ci::ColorA&	getColor() const		{ return getColorForState( getState() ); }
	const std::string&	getTitle() const		{ return getTitleForState( getState() ); }
	const ci::ColorA&	getTitleColor() const	{ return getTitleColorForState( getState() ); }
	//! Returns the image that will be used to draw for the current state, if any. If there is an image for State::Normal but not the current button state, then it will be used.
	ImageRef			getImage() const;

	ci::signals::Signal<void ()>&	getSignalPressed()	{ return mSignalPressed; }
	ci::signals::Signal<void ()>&	getSignalReleased()	{ return mSignalReleased; }

protected:
	void draw( Renderer *ren )	override;

	bool touchesBegan( ci::app::TouchEvent &event )	override;
	bool touchesMoved( ci::app::TouchEvent &event )	override;
	bool touchesEnded( ci::app::TouchEvent &event )	override;

	bool        mEnabled = false;
	bool        mIsToggle = false;
	State       mState = State::NORMAL;

	ci::ColorA	mColorNormal = ci::ColorA::gray( 0.5f );
	ci::ColorA	mColorEnabled = ci::ColorA::gray( 0.38f );
	ci::ColorA	mColorPressed = ci::ColorA::gray( 0.3f );
	ci::ColorA	mColorTitleNormal = ci::ColorA::gray( 0.2f, 0.6f );
	ci::ColorA	mColorTitleEnabled = ci::ColorA::gray( 0.2f, 0.6f );
	bool		mHasColorTitleEnabled = false; // keep track of when user hasn't set the title color for enabled, will use normal color otherwise

	TextRef			mTextTitle;
	std::string		mTitleNormal, mTitleEnabled;

	ImageRef   mImageNormal, mImageEnabled, mImagePressed;

	ci::signals::Signal<void ()>	mSignalPressed, mSignalReleased; // TODO: look at other frameworks (like html5, dart) and see what they name these, and how they organize the events
};

//! Toggle Button with text off to the right side.
class CheckBox : public Button {
  public:
	CheckBox( const ci::Rectf &bounds = ci::Rectf::zero() );

  protected:
	void draw( Renderer *ren )	override;

};

} // namespace ui

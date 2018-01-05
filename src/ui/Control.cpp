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

#include "ui/Control.h"
#include "ui/Layout.h"
#include "ui/Graph.h"
#include "ui/Debug.h"
#include "fmt/format.h"
#include <array>

using namespace ci;
using namespace std;

namespace ui {

// ----------------------------------------------------------------------------------------------------
// Control
// ----------------------------------------------------------------------------------------------------

bool Control::hitTestInsideCancelPadding( const vec2 &localPos ) const
{
	return	( localPos.x >= - mCancelPadding.x1 )					&&
			( localPos.y >= - mCancelPadding.y1 )					&&
			( localPos.x <= ( getSize().x + mCancelPadding.x2 ) )	&&
			( localPos.y <= ( getSize().y + mCancelPadding.y2 ) );
}

// ----------------------------------------------------------------------------------------------------
// Button
// ----------------------------------------------------------------------------------------------------

Button::Button( const Rectf &bounds )
	: Control( bounds )
{
	mTextTitle = TextManager::loadText( FontFace::NORMAL );
}

const ColorA& Button::getColorForState( Button::State state ) const
{
	switch( state ) {
		case State::NORMAL:		return mColorNormal;
		case State::ENABLED:	return mColorEnabled;
		case State::PRESSED:	return mColorPressed;
		default:				break;
	}

	return mColorNormal;
}

ImageRef Button::getImageForState( State state ) const
{
	switch( state ) {
		case State::NORMAL:		return mImageNormal;
		case State::ENABLED:	return mImageEnabled;
		case State::PRESSED:	return mImagePressed;
		default:				break;
	}

	return mImageNormal;
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
	auto image = getImage();
	if( image ) {
		// draw image
		ren->draw( image, getBoundsLocal() );
	}
	else {
		// draw background solid color
		ren->setColor( getColor() );
		ren->drawSolidRect( getBoundsLocal() );

		// draw title
		const float padding = 6;
		ren->setColor( getTitleColor() );
		mTextTitle->drawString( getTitle(), vec2( padding, getCenterLocal().y + mTextTitle->getDescent() ) );
	}
}

void Button::setEnabled( bool enabled )
{
	if( mEnabled == enabled )
		return;

	mEnabled = enabled;
	mState = enabled ? State::ENABLED : State::NORMAL;

	getSignalValueChanged().emit();
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
	switch( state ) {
		case State::NORMAL:		mColorNormal = color; return;
		case State::ENABLED:	mColorEnabled = color; return;
		case State::PRESSED:	mColorPressed = color; return;
		default: CI_ASSERT_NOT_REACHABLE();
	}
}

void Button::setImage( const ui::ImageRef &image, State state )
{
	switch( state ) {
		case State::NORMAL:		mImageNormal = image; return;
		case State::ENABLED:	mImageEnabled = image; return;
		case State::PRESSED:	mImagePressed = image; return;
		default: CI_ASSERT_NOT_REACHABLE();
	}
}

ImageRef Button::getImage() const
{
	switch( getState() ) {
		case State::NORMAL:
		return mImageNormal;
		case State::ENABLED: {
			if( mImageEnabled )
				return mImageEnabled;
		}
							 break;
		case State::PRESSED: {
			if( mImagePressed )
				return mImagePressed;
		}
							 break;
		default:
		break;
	}

	return mImageNormal;
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

// ----------------------------------------------------------------------------------------------------
// CheckBox
// ----------------------------------------------------------------------------------------------------

CheckBox::CheckBox( const Rectf &bounds )
	: Button( bounds )
{
	setAsToggle();
}

void CheckBox::draw( Renderer *ren )
{
	// draw background solid color
	//	ren->setColor( getColor() );
	//	ren->drawSolidRect( getBoundsLocal() );

	const float padding = 6;
	float r = glm::max( getHeight() - padding * 2, 2.0f );

	auto checkBorder = Rectf( padding, padding, padding + r, padding + r );
	auto checkFilled = Rectf( padding * 2, padding * 2, r, r );

	ren->setColor( Color( 1, 1, 1 ) );
	ren->drawStrokedRect( checkBorder, 2 );

	if( isEnabled() ) {
		ren->drawSolidRect( checkFilled );
	}

	// draw title
	ren->setColor( getTitleColor() );
	const float offsetY = 4;
	mTextTitle->drawString( getTitle(), vec2( r + padding * 2, getCenterLocal().y + mTextTitle->getDescent() + offsetY ) );
}

// ----------------------------------------------------------------------------------------------------
// TextField
// ----------------------------------------------------------------------------------------------------

TextField::TextField( const ci::Rectf &bounds )
	: Control( bounds )
{
	setAcceptsFirstResponder( true );
	setClipEnabled( true );

	mText = TextManager::loadText( FontFace::NORMAL );
}

void TextField::setBorderColor( const ci::ColorA &color, State state )
{
	switch( state ) {
		case State::NORMAL:		mBorderColorNormal = color; return;
		case State::SELECTED:	mBorderColorSelected = color; return;
			//case State::PRESSED:	mColorPressed = color; return;
		default: CI_ASSERT_NOT_REACHABLE();
	}
}

void TextField::setTextColor( const ci::ColorA &color, State state )
{
	switch( state ) {
		case State::NORMAL:		mTextColorNormal = color; return;
		case State::SELECTED:	mTextColorSelected = color; return;
			//case State::PRESSED:	mColorPressed = color; return;
		default: CI_ASSERT_NOT_REACHABLE();
	}
}

void TextField::setPlaceholderText( const std::string &text )
{
	mPlaceholderString = text;
	if( getLabel().empty() )
		setLabel( "TextField ('" + text + "')" );
}

void TextField::draw( Renderer *ren )
{
	const float padding = 6;

	// draw text
	if( ! mInputString.empty() ) {
		auto color = isFirstResponder() ? mTextColorSelected : mTextColorNormal;
		ren->setColor( color );
		mText->drawString( mInputString, vec2( padding, getCenterLocal().y + mText->getDescent() ) );
	}
	else if( ! isFirstResponder() && ! mPlaceholderString.empty() ) {
		auto color = Color::gray( 0.5f ); // TODO: make color a property
		ren->setColor( color );
		mText->drawString( mPlaceholderString, vec2( padding, getCenterLocal().y + mText->getDescent() ) );
	}

	// draw cursor bar
	if( isFirstResponder() ) {
		const float cursorThickness = 1;
		const float nextCharOffset = 6;
		vec2 cursorLoc = { 0, 0 };
		// measure where the cursor should be drawn (where the next character will be inserted)
		string stringUntilCursor = mInputString.substr( 0, mCursorPos );
		cursorLoc = mText->measureString( stringUntilCursor );
		cursorLoc.x += nextCharOffset;
		Rectf cursorRect = { cursorLoc.x - cursorThickness / 2, 0, cursorLoc.x + cursorThickness / 2, getHeight() };

		ColorA cursorColor = mBorderColorSelected;
		cursorColor.a *= (float)( 1.0 - glm::pow( cos( getGraph()->getElapsedSeconds() * 2 ), 4 ) );

		// TODO: does this need more care to be correctly composited?
		ren->pushBlendMode( ui::BlendMode::PREMULT_ALPHA );
		ren->setColor( cursorColor );
		ren->drawSolidRect( cursorRect );
		ren->popBlendMode();
	}

	// draw border
	if( mBorderMode != BorderMode::DISABLED ) {
		auto color = isFirstResponder() ? mBorderColorSelected : mBorderColorNormal;
		ren->setColor( color );
		ren->drawStrokedRect( getBoundsLocal(), 2 );
	}
}

bool TextField::willBecomeFirstResponder()
{
	UI_LOG_TEXT( getName() );

	// emit begin signal first, which might be used to update the initial string
	mSignalTextInputBegin.emit();

	// If the current cursor position is invalid, place it at the end of the current input string
	// TODO: place it according to touch pos
	if( mCursorPos < 0 || mCursorPos > (int)mInputString.size() ) {
		mCursorPos = (int)mInputString.size();
	}

	// store the input string, in case input is canceled and we need to revert.
	mInputStringBeforeInput = mInputString;
	return true;
}

bool TextField::willResignFirstResponder()
{
	UI_LOG_TEXT( getName() );
	mSignalTextInputCompleted.emit();
	return true;
}

bool TextField::keyDown( ci::app::KeyEvent &event )
{   
	//UI_LOG_TEXT( "char: " << ( event.getChar() ? event.getChar() : 0 ) << ", char utf32: " << event.getCharUtf32() << ", code: " << event.getCode() 
	//	<< ", shift down: " << event.isShiftDown() << ", alt down: " << event.isAltDown() << ", ctrl down: " << event.isControlDown()
	//	<< ", meta down: " << event.isMetaDown() << ", accel down: " << event.isAccelDown() << ", native code: " << event.getNativeKeyCode() );

	bool handled = true;
	if( event.getCode() == app::KeyEvent::KEY_RETURN ) {
		// text completed
		UI_LOG_TEXT( "(enter) text completed." );
		mSignalTextInputCompleted.emit();
		resignFirstResponder();
	}
	else if( event.getCode() == app::KeyEvent::KEY_ESCAPE ) {
		// cancel input text, reverting it to previous when we became first responder
		UI_LOG_TEXT( "(escape) text canceled." );
		mInputString = mInputStringBeforeInput;
		mCursorPos = std::min( mCursorPos, (int)mInputString.size() );
		mSignalTextInputCanceled.emit();
		resignFirstResponder();
	}
	else if( event.getCode() == app::KeyEvent::KEY_BACKSPACE ) {
		// delete character before cursor, if possible
		if( mCursorPos > 0 && mCursorPos - 1 < mInputString.size() ) {
			mInputString.erase( mCursorPos - 1, 1 );
			mCursorPos -= 1;
			getSignalValueChanged().emit();
		}
		UI_LOG_TEXT( "(backspace) string size: " << mInputString.size() << ", cursor pos: " << mCursorPos );
	}
	else if( event.getCode() == app::KeyEvent::KEY_DELETE ) {
		// delete character after cursor, if possible
		if( mCursorPos < mInputString.size() ) {
			mInputString.erase( mCursorPos, 1 );
			getSignalValueChanged().emit();
		}
		UI_LOG_TEXT( "(delete) string size: " << mInputString.size() << ", cursor pos: " << mCursorPos );
	}
	else if( event.getCode() == app::KeyEvent::KEY_RIGHT ) {
		// move cursor to the right, if possible
		if( mCursorPos < mInputString.size() ) {
			mCursorPos += 1;
		}

		UI_LOG_TEXT( "(right) string size: " << mInputString.size() << ", cursor pos: " << mCursorPos );
	}
	else if( event.getCode() == app::KeyEvent::KEY_LEFT ) {
		// move cursor to the right, if possible
		if( mCursorPos > 0 ) {
			mCursorPos -= 1;
		}

		UI_LOG_TEXT( "(left) string size: " << mInputString.size() << ", cursor pos: " << mCursorPos );
	}
	else if( event.getChar() ) {
		if( ! checkCharIsValid( event.getChar() ) ) {
			UI_LOG_TEXT( "(enter char) rejecting char: " << event.getChar() );
			handled = false;
		}
		else {
			mInputString.insert( mCursorPos, 1, event.getChar() );
			mCursorPos += 1;
			UI_LOG_TEXT( "(enter char) string size: " << mInputString.size() << ", cursor pos: " << mCursorPos );
			getSignalValueChanged().emit();
		}
	}
	else
		handled = false;

	return handled;
}

bool TextField::checkCharIsValid( char c ) const
{
	if( mInputMode == InputMode::NUMERIC ) {
		if( c == '.' ) {
			// not valid if there is already a decimal in the input string
			if( mInputString.find( '.' ) != string::npos )
				return false;
		}
		else if( c == '-' ) {
			// reject minus sign if it isn't at the beginning
			if( mCursorPos != 0 )
				return false;
		}
		else if( ! isdigit( c ) )
			return false;
	}

	return true;
}

// ----------------------------------------------------------------------------------------------------
// SliderBase
// ----------------------------------------------------------------------------------------------------

SliderBase::SliderBase( const Rectf &bounds )
	: Control( bounds )
{
	mTextLabel = TextManager::loadText( FontFace::NORMAL );

	mTapTracker.getSignalGestureDetected().connect( signals::slot( this, &SliderBase::onDoubleTap ) );

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
		getSignalValueChanged().emit();
}

void SliderBase::onDoubleTap()
{
	CI_LOG_I( "bang" );
	getBackground()->setColor( Color( 0, 0.5f, 0.5f ) );
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
	setTouchCanceled( false );
	mTapTracker.processTouchesBegan( event, getGraph()->getElapsedSeconds() );

	auto &firstTouch = event.getTouches().front();
	vec2 pos = toLocal( firstTouch.getPos() );

	UI_LOG_TOUCHES( "[" << getName() << "] pos: " << pos << ", num touches: " << event.getTouches().size() );

	updateValue( pos );
	firstTouch.setHandled();
	return true;
}

bool SliderBase::touchesMoved( app::TouchEvent &event )
{
	if( isTouchCanceled() )
		return false;

	auto &firstTouch = event.getTouches().front();
	vec2 pos = toLocal( firstTouch.getPos() );
	if( ! hitTestInsideCancelPadding( pos ) ) {
		UI_LOG_TOUCHES( "[" << getName() << "] CANCELED| pos: " << pos << ", num touches: " << event.getTouches().size() );
		setTouchCanceled( true );
		return false;
	}

	UI_LOG_TOUCHES( "[" << getName() << "] pos: " << pos << ", num touches: " << event.getTouches().size() );

	updateValue( pos );
	firstTouch.setHandled();
	return true;
}

bool SliderBase::touchesEnded( app::TouchEvent &event )
{
	if( isTouchCanceled() )
		return false;

	mTapTracker.processTouchesEnded( event, getGraph()->getElapsedSeconds() );

	auto &firstTouch = event.getTouches().front();
	vec2 pos = toLocal( firstTouch.getPos() );
	if( ! hitTestInsideCancelPadding( pos ) ) {
		UI_LOG_TOUCHES( "[" << getName() << "] CANCELED| pos: " << pos << ", num touches: " << event.getTouches().size() );
		setTouchCanceled( true );
		return false;
	}

	UI_LOG_TOUCHES( "[" << getName() << "] pos: " << pos << ", num touches: " << event.getTouches().size() );

	updateValue( pos );
	firstTouch.setHandled();
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
		getSignalValueChanged().emit();
}

// ----------------------------------------------------------------------------------------------------
// HSlider
// ----------------------------------------------------------------------------------------------------

float HSlider::getValuePercentage( const ci::vec2 &pos )
{
	return getWidth() < 0.00001f ? 0 : pos.x / getWidth();
}

Rectf HSlider::getValueRect( float sliderPos, float sliderRadius ) const
{
	float offset = sliderPos * getWidth();
	return Rectf( offset - sliderRadius, 0, offset + sliderRadius, getHeight() );

}


// ----------------------------------------------------------------------------------------------------
// VSlider
// ----------------------------------------------------------------------------------------------------

float VSlider::getValuePercentage( const ci::vec2 &pos )
{
	return getHeight() < 0.00001f ? 0 : ( 1 - pos.y / getHeight() );
}

Rectf VSlider::getValueRect( float sliderPos, float sliderRadius ) const
{
	float offset = ( 1 - sliderPos ) * getHeight();
	return Rectf( 0, offset - sliderRadius, getWidth(), offset + sliderRadius );
}

// ----------------------------------------------------------------------------------------------------
// SelectorBase
// ----------------------------------------------------------------------------------------------------

SelectorBase::SelectorBase( const Rectf &bounds )
	: Control( bounds )
{
	setBlendMode( BlendMode::PREMULT_ALPHA );

	mTextLabel = TextManager::loadText( FontFace::NORMAL );
}

void SelectorBase::draw( Renderer *ren )
{
	const float padding = 10;

	float sectionHeight = getHeight() / (float)mSegments.size();
	Rectf section( 0, 0, getWidth(), sectionHeight );

	ren->setColor( mUnselectedColor );
	for( size_t i = 0; i < mSegments.size(); i++ ) {
		if( i != mSelectedIndex ) {
			ren->drawStrokedRect( section );
			mTextLabel->drawString( mSegments[i], vec2( section.x1 + padding, section.getCenter().y + mTextLabel->getDescent() ) );
		}
		section += vec2( 0.0f, sectionHeight );
	}

	ren->setColor( mSelectedColor );

	section.y1 = mSelectedIndex * sectionHeight;
	section.y2 = section.y1 + sectionHeight;
	ren->drawStrokedRect( section );

	if( ! mSegments.empty() ) {
		mTextLabel->drawString( mSegments[mSelectedIndex], vec2( section.x1 + padding, section.getCenter().y + mTextLabel->getDescent() ) );
	}

	if( ! mTitle.empty() ) {
		ren->setColor( mTitleColor );
		mTextLabel->drawString( mTitle, vec2( padding, - mTextLabel->getDescent() ) );
	}
}


bool SelectorBase::touchesBegan( app::TouchEvent &event )
{
	setTouchCanceled( false );
	auto &firstTouch = event.getTouches().front();
	vec2 pos = toLocal( firstTouch.getPos() );
	updateSelection( pos );
	firstTouch.setHandled();
	return true;
}

bool SelectorBase::touchesMoved( app::TouchEvent &event )
{
	if( isTouchCanceled() )
		return false;

	vec2 pos = toLocal( event.getTouches().front().getPos() );
	if( ! hitTestInsideCancelPadding( pos ) ) {
		setTouchCanceled( true );
		return false;
	}

	updateSelection( pos );
	return true;
}

bool SelectorBase::touchesEnded( app::TouchEvent &event )
{
	if( isTouchCanceled() )
		return false;

	vec2 pos = toLocal( event.getTouches().front().getPos() );
	if( ! hitTestInsideCancelPadding( pos ) ) {
		setTouchCanceled( true );
		return false;
	}

	updateSelection( pos );
	return true;
}

void SelectorBase::updateSelection( const vec2 &pos )
{
	int offset = int( pos.y - getPos().y );
	int sectionHeight = int( getHeight() / (float)mSegments.size() );
	size_t selectedIndex = std::min<size_t>( offset / sectionHeight, mSegments.size() - 1 );

	if( mSelectedIndex != selectedIndex ) {
		mSelectedIndex = selectedIndex;
		getSignalValueChanged().emit();
	}
}

const std::string& SelectorBase::getSelectedLabel() const
{
	return mSegments.at( mSelectedIndex );
}

void SelectorBase::select( size_t index )
{
	CI_ASSERT( index < mSegments.size() );

	if( mSelectedIndex != index ) {
		mSelectedIndex = index;
		getSignalValueChanged().emit();
	}
}

void SelectorBase::select( const string &label )
{
	for( size_t i = 0; i < mSegments.size(); i++ ) {
		if( mSegments[i] == label )
			select( i );
	}

	CI_LOG_E( "unknown label: " << label );
}

// ----------------------------------------------------------------------------------------------------
// NumberBox
// ----------------------------------------------------------------------------------------------------

NumberBox::NumberBox( const Rectf &bounds )
	: Control( bounds ), mMin( std::numeric_limits<float>::lowest() ), mMax( std::numeric_limits<float>::max() )
{
	// TODO: add this to ui::Control to indicate that this control doesn't use touch cancelling
	// - otherwise the public isCancelled() will be called which is no good
	//setTouchCancellingEnabled( false );

	// TODO: can avoid this by clipping the actual text string
	setClipEnabled( true ); 

	setNumDigits( 3 );

	mTextLabel = TextManager::loadText( FontFace::NORMAL );
	mTextField = make_shared<TextField>();
	mTextField->setInputMode( TextField::InputMode::NUMERIC );
	mTextField->setBorderMode( TextField::BorderMode::DISABLED );
	mTextField->setHidden();
	addSubview( mTextField );
	View::setNextResponder( mTextField );

	mTapTracker.getSignalGestureDetected().connect( signals::slot( this, &NumberBox::onDoubleTap ) );
	mTextField->getSignalValueChanged().connect( signals::slot( this, &NumberBox::onTextInputUpdated ) );
	mTextField->getSignalmSignalTextInputBegin().connect( signals::slot( this, &NumberBox::onTextInputBegin ) );
	mTextField->getSignalTextInputCompleted().connect( signals::slot( this, &NumberBox::onTextInputCompleted ) );
	mTextField->getSignalTextInputCanceled().connect( signals::slot( this, &NumberBox::onTextInputCompleted ) );

	// set a default background color
	getBackground()->setColor( Color::black() );
}

void NumberBox::setMin( float min )
{
	mMin = min;
	if( mValue < min )
		setValue( min );
}

void NumberBox::setMax( float max )
{
	mMax = max;
	if( mValue > max )
		setValue( max );
}

void NumberBox::setValue( float value, bool emitChanged )
{
	mValue = value;
	if( mSnapToInt )
		mValue = roundf( mValue );

	if( emitChanged )
		getSignalValueChanged().emit();
}

void NumberBox::setTitle( const std::string &title )
{ 
	mTitle = title;
	if( getLabel().empty() )
		setLabel( "NumberBox ('" + title + "')" );

	mTextField->setLabel( "TextField (" + getLabel() + ")" );

	setNeedsLayout();
}

void NumberBox::layout()
{
	Rectf textFieldBounds = getBoundsLocal();
	if( ! mTitle.empty() ) {
		// TODO: use getTitleLabel() instead
		string title = mTitle + ": ";
		// offset the TextField bounds by the title label
		vec2 titleSize = mTextLabel->measureString( title );
		textFieldBounds.x1 += mPadding + titleSize.x;
	}

	mTextField->setBounds( textFieldBounds );
}

void NumberBox::draw( Renderer *ren )
{
	// TODO: add option to draw to right, like CheckBox
	ren->setColor( mTitleColor );
	mTextLabel->drawString( getTitleLabel(), vec2( mPadding, getCenterLocal().y + mTextLabel->getDescent() ) );

	ren->setColor( mBorderColor );
	ren->drawStrokedRect( getBoundsLocal(), 2 );
}

std::string	NumberBox::getTitleLabel() const
{
	std::string result = mTitle;
	if( ! result.empty() )
		result += ": ";

	// If we're in text entry mode, the TextField will display the value. Otherwise, append it here.
	if( mTextField->isHidden() ) {
		result += getValueAsString();
	}
	return result;
}

std::string	NumberBox::getValueAsString() const
{
	return fmt::format( mFormatStr, getValue() );
}

void NumberBox::setNumDigits( size_t numDigits )
{
	mNumDigits = numDigits;
	mFormatStr = "{0:." + to_string( numDigits ) + "f}";
}

void NumberBox::updateValueFromTextField()
{
	float value = 0;
	if( ! mTextField->getText().empty() ) {
		// TODO: remove try catch and don't call stof() unless we know it is a number
		// - don't want these tripping exception breakpoints
		// - might want to do a custom stof that returns false if it couldn't convert
		try {
			value = stof( mTextField->getText() );
		}
		catch( std::exception &exc ) {
			CI_LOG_W( "couldn't convert string '" << mTextField->getText() << "' to float" );
		}
	}

	setValue( value );
}

void NumberBox::setNextResponder( const ViewRef &view )
{
	// responder chain goes: NumberBox -> mTextField -> view
	mTextField->setNextResponder( view );
}

void NumberBox::onDoubleTap()
{
	mTextField->becomeFirstResponder();
}

void NumberBox::onTextInputBegin()
{
	UI_LOG_RESPONDER( "(" << getName() << ") text: " << mTextField );

	mTextField->setText( getValueAsString() );	
	mTextField->setHidden( false );
}

void NumberBox::onTextInputUpdated()
{
	UI_LOG_RESPONDER( "(" << getName() << ") text: " << mTextField->getText() );
	updateValueFromTextField();
}

void NumberBox::onTextInputCompleted()
{
	UI_LOG_RESPONDER( "(" << getName() << ") text: " << mTextField->getText() );
	updateValueFromTextField();

	// text input is finished so hide the TextField
	mTextField->setHidden( true );
}

bool NumberBox::touchesBegan( app::TouchEvent &event )
{
	setTouchCanceled( false );
	mTapTracker.processTouchesBegan( event, getGraph()->getElapsedSeconds() );

	auto &firstTouch = event.getTouches().front();
	mDragStartPos = toLocal( firstTouch.getPos() );
	mDragStartValue = mValue;

	firstTouch.setHandled();
	return true;
}

bool NumberBox::touchesMoved( app::TouchEvent &event )
{
	setTouchCanceled( false );

	auto &firstTouch = event.getTouches().front();
	vec2 pos = toLocal( firstTouch.getPos() );

	updateValue( pos );
	firstTouch.setHandled();
	return true;
}

bool NumberBox::touchesEnded( app::TouchEvent &event )
{
	setTouchCanceled( false );
	mTapTracker.processTouchesEnded( event, getGraph()->getElapsedSeconds() );

	auto &firstTouch = event.getTouches().front();
	vec2 pos = toLocal( firstTouch.getPos() );

	updateValue( pos );
	firstTouch.setHandled();
	return true;
}

// Summing the delta change in both axes, so that dragging:
// - right increases, left decreases
// - up increases, down decreases
void NumberBox::updateValue( const ci::vec2 &pos )
{
	vec2 deltaPos = mDragStartPos - pos;
	float deltaValue = ( deltaPos.y - deltaPos.x ) * mStep;

	float prevValue = mValue;
	mValue = glm::clamp( mDragStartValue + deltaValue, mMin, mMax );

	if( mSnapToInt )
		mValue = roundf( mValue );

	if( mValue != prevValue )
		getSignalValueChanged().emit();
}


// ----------------------------------------------------------------------------------------------------
// NumberBoxT
// ----------------------------------------------------------------------------------------------------

template <typename T>
NumberBoxT<T>::NumberBoxT( const Rectf &bounds )
	: Control( bounds )
{
	const array<string, 4> titles = { "x", "y", "z", "w" };

	mControlContainer = make_shared<ui::View>();

	addSubview( mControlContainer );

	for( size_t i = 0; i < getSize(); i++ ) {
		auto nbox = make_shared<ui::NumberBox>();
		nbox->setTitle( titles[i] );
		nbox->setBackgroundEnabled( false );
		nbox->getSignalValueChanged().connect( signals::slot( this, &NumberBoxT::onValueChanged ) );

		// link up responder chain
		if( i == 0 ) {
			View::setNextResponder( nbox );
		}
		else {
			mNumberBoxes.back()->setNextResponder( nbox );
		}

		mNumberBoxes.push_back( nbox );
		mControlContainer->addSubview( nbox );
	}

	// Use a HorizontalLayout to evenly space out NumberBoxes
	{
		auto layout = make_shared<ui::HorizontalLayout>();
		layout->setMode( ui::LinearLayout::Mode::FILL );
		layout->setAlignment( Alignment::FILL );

		mControlContainer->setLayout( layout );
	}

	// Use a VerticalLayout to stack the title label and control container on top of each other.
	// TODO: This will have to change if label is to be drawn to the side
	{
		auto layout = make_shared<ui::VerticalLayout>();
		layout->setMode( ui::LinearLayout::Mode::FILL );
		layout->setAlignment( Alignment::FILL );

		setLayout( layout );
	}

	// set a default background color
	getBackground()->setColor( Color::black() );
}

template <typename T>
void NumberBoxT<T>::setNextResponder( const ViewRef &view )
{ 
	// responder chain goes: NumBerBoxT -> last NumberBox -> view
	mNumberBoxes.back()->setNextResponder( view );
}

template <typename T>
void NumberBoxT<T>::setTitle( const std::string &title, TitlePosition position )
{ 
	if( ! mTitleLabel ) {
		mTitleLabel = make_shared<ui::Label>();
		insertSubview( mTitleLabel, 0 ); // insert at the beginning so it is drawn above
	}

	mTitleLabel->setText( title );
	mTitleLabel->setTextColor( mTitleColor );

	//mTitlePosition = position; // ignoring position for now
}

template <typename T>
const std::string& NumberBoxT<T>::getTitle() const
{
	if( ! mTitleLabel ) {
		static std::string sEmptyString;
		return sEmptyString;
	}

	return mTitleLabel->getText();
}

//! Sets the minimum value, defaults to smallest (negative) possible float
template <typename T>
void NumberBoxT<T>::setMin( float min )
{
	for( const auto &nbox : mNumberBoxes )
		nbox->setMin( min );
}

//! Sets the maximum value, defaults to largest possible float
template <typename T>
void NumberBoxT<T>::setMax( float max )
{
	for( const auto &nbox : mNumberBoxes )
		nbox->setMax( max );
}

template <typename T>
void NumberBoxT<T>::setStep( float step )
{
	for( const auto &nbox : mNumberBoxes )
		nbox->setStep( step );
}

template <typename T>
void NumberBoxT<T>::setValue( const T &value )
{
	for( int i = 0; i < getSize(); i++ ) {
		mValue[i] = value[i];
		mNumberBoxes[i]->setValue( value[i] );
	}

	getSignalValueChanged().emit();
}

template <typename T>
void NumberBoxT<T>::setBorderColor( const ci::ColorA &color )
{
	for( const auto &nbox : mNumberBoxes )
		nbox->setBorderColor( color );
}

template <typename T>
void NumberBoxT<T>::onValueChanged()
{
	for( int i = 0; i < getSize(); i++ )
		mValue[i] = mNumberBoxes[i]->getValue();

	getSignalValueChanged().emit();
}

// Specializing float type:
template<>
int NumberBoxT<float>::getSize() const
{
	return 1;
}

template <>
void NumberBoxT<float>::setValue( const float &value )
{
	mValue = value;
	mNumberBoxes[0]->setValue( value );

	getSignalValueChanged().emit();
}

template <>
void NumberBoxT<float>::onValueChanged()
{
	mValue = mNumberBoxes[0]->getValue();

	getSignalValueChanged().emit();
}

template class CI_UI_API NumberBoxT<float>;
template class CI_UI_API NumberBoxT<vec2>;
template class CI_UI_API NumberBoxT<vec3>;
template class CI_UI_API NumberBoxT<vec4>;

} // namespace ui

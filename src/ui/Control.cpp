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
#include "ui/TextField.h"
#include "cinder/Log.h"
#include "fmt/format.h"
#include <array>

//#define LOG_TOUCHES( stream )	CI_LOG_I( stream )
#define LOG_TOUCHES( stream )	( (void)( 0 ) )

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

	LOG_TOUCHES( "[" << getName() << "] pos: " << pos << ", num touches: " << event.getTouches().size() );

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
		LOG_TOUCHES( "[" << getName() << "] CANCELED| pos: " << pos << ", num touches: " << event.getTouches().size() );
		setTouchCanceled( true );
		return false;
	}

	LOG_TOUCHES( "[" << getName() << "] pos: " << pos << ", num touches: " << event.getTouches().size() );

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
		LOG_TOUCHES( "[" << getName() << "] CANCELED| pos: " << pos << ", num touches: " << event.getTouches().size() );
		setTouchCanceled( true );
		return false;
	}

	LOG_TOUCHES( "[" << getName() << "] pos: " << pos << ", num touches: " << event.getTouches().size() );

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

	mTextLabel = TextManager::loadText( FontFace::NORMAL );
	mTextField = make_shared<TextField>();
	mTextField->setInputMode( TextField::InputMode::NUMERIC );
	mTextField->setBorderMode( TextField::BorderMode::DISABLED );
	mTextField->setHidden();
	addSubview( mTextField );
	setNextResponder( mTextField );

	mTapTracker.getSignalGestureDetected().connect( signals::slot( this, &NumberBox::onDoubleTap ) );
	mTextField->getSignalValueChanged().connect( signals::slot( this, &NumberBox::onTextInputUpdated ) );
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
	return fmt::format( "{}", getValue() );
}

ViewRef	NumberBox::getNextResponder() const
{
	// TODO: is this a hack? or explain
	CI_LOG_I( "(" << getName() << ") mTextField hidden: " << mTextField->isHidden() );
	if( mTextField->isHidden() ) {
		mTextField->setHidden( false );
		mTextField->setText( getValueAsString() );
		return mTextField;
	}
	else
		return View::getNextResponder();
}

void NumberBox::onDoubleTap()
{
	//mTextField->getBackground()->setColor( Color( 0.1f, 0.3f, 0.3f ) );
	mTextField->setText( getValueAsString() );
	mTextField->setHidden( false );
	
	mTextField->becomeFirstResponder();
}

void NumberBox::onTextInputUpdated()
{
	CI_LOG_I( "(" << getName() << ") text: " << mTextField );
	float updatedValue = stof( mTextField->getText() );
	setValue( updatedValue );
}

void NumberBox::onTextInputCompleted()
{
	CI_LOG_I( "(" << getName() << ") text: " << mTextField );
	float updatedValue = stof( mTextField->getText() );
	setValue( updatedValue );

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

	LOG_TOUCHES( "[" << getName() << "] pos: " << mDragStartPos << ", num touches: " << event.getTouches().size() );

	firstTouch.setHandled();
	return true;
}

bool NumberBox::touchesMoved( app::TouchEvent &event )
{
	setTouchCanceled( false );

	auto &firstTouch = event.getTouches().front();
	vec2 pos = toLocal( firstTouch.getPos() );

	LOG_TOUCHES( "[" << getName() << "] pos: " << pos << ", num touches: " << event.getTouches().size() );

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

	LOG_TOUCHES( "[" << getName() << "] pos: " << pos << ", num touches: " << event.getTouches().size() );

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
	//nbox->setSize( 0, 30 );

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
	for( size_t i = 0; i < getSize(); i++ ) {
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
	for( size_t i = 0; i < getSize(); i++ )
		mValue[i] = mNumberBoxes[i]->getValue();

	getSignalValueChanged().emit();
}

// Specializing float type:
template<>
size_t NumberBoxT<float>::getSize() const
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

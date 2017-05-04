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
// NumberBox
// ----------------------------------------------------------------------------------------------------

NumberBox::NumberBox( const Rectf &bounds )
	: Control( bounds ), mMin( std::numeric_limits<float>::lowest() ), mMax( std::numeric_limits<float>::max() )
{
	// TODO: add this to ui::Control to indicate that this control doesn't use touch cancelling
	// - otherwise the public isCancelled() will be called which is no good
	//setTouchCancellingEnabled( false );

	mTextLabel = TextManager::loadText( FontFace::NORMAL );

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

void NumberBox::draw( Renderer *ren )
{
	const float padding = 6;

	// TODO: add option to draw to right, like CheckBox
	ren->setColor( mTitleColor );
	mTextLabel->drawString( getTitleLabel(), vec2( padding, getCenterLocal().y + mTextLabel->getDescent() ) );

	ren->setColor( mBorderColor );
	ren->drawStrokedRect( getBoundsLocal(), 2 );
}

std::string	NumberBox::getTitleLabel() const
{
	std::string result = mTitle;
	if( ! result.empty() )
		result += ": ";

	result += fmt::format( "{}", getValue() );
	return result;
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

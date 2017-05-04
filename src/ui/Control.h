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
#include "ui/Label.h"
#include "ui/GestureTracker.h"

namespace ui {

typedef std::shared_ptr<class Control>		ControlRef;
typedef std::shared_ptr<class NumberBox>	NumberBoxRef;

//! Base class for all Controls, which usually are meant to be interacted with by a user.
class CI_UI_API Control : public View {
  public:
	Control( const ci::Rectf &bounds = ci::Rectf::zero() )	: View( bounds ) {}

	void setTouchCanceled( bool cancel )	{ mTouchCanceled = cancel; }
	bool isTouchCanceled() const			{ return mTouchCanceled; }

	void setCancelPadding( const ci::Rectf &padding )   { mCancelPadding = padding; }
	const ci::Rectf& getCancelPadding() const           { return mCancelPadding; }

	//! Signal that is emitted whenever a Control's value changes
	ci::signals::Signal<void ()>&	getSignalValueChanged()	{ return mSignalValueChanged; }

  protected:
	bool hitTestInsideCancelPadding( const ci::vec2 &localPos ) const;

  private:
	ci::Rectf	mCancelPadding = ci::Rectf( 40, 40, 40, 40 );
	bool		mTouchCanceled = false;

	ci::signals::Signal<void ()>	mSignalValueChanged;
};

class CI_UI_API NumberBox : public Control {
  public:
	NumberBox( const ci::Rectf &bounds = ci::Rectf::zero() );

	//! Sets the minimum value, defaults to smallest (negative) possible float
	void setMin( float min );
	//! Sets the maximum value, defaults to largest possible float
	void setMax( float max );

	float getMin() const	{ return mMin; }
	float getMax() const	{ return mMax; }

	//! Sets the amount that the value is changed per pixel step when dragged. Default is 1.
	void setStep( float step )	{ mStep = step; }
	//! Returns the amount that the value is changed per pixel step when dragged.
	float getStep() const	{ return mStep; }

	void				setTitle( const std::string &title )	{ mTitle = title; }
	const std::string&	getTitle() const						{ return mTitle; }

	float getValue() const	{ return mValue; }

	void setValue( float value, bool emitChanged = true );

	void setBorderColor( const ci::ColorA &color )	{ mBorderColor = color; }
	void setTitleColor( const ci::ColorA &color )	{ mTitleColor = color; }

	void setSnapToIntEnabled( bool enable )	{ mSnapToInt = enable; }
	bool isSnapToIntEnabled() const			{ return mSnapToInt; }

  protected:
	void draw( Renderer *ren )	override;

	bool touchesBegan( ci::app::TouchEvent &event )	override;
	bool touchesMoved( ci::app::TouchEvent &event )	override;
	bool touchesEnded( ci::app::TouchEvent &event )	override;

	std::string	getTitleLabel() const;

	void updateValue( const ci::vec2 &pos );

  private:
	float	mValue = 0;
	float	mMin;
	float	mMax;
	float	mStep = 1;

	ci::vec2	mDragStartPos;
	float		mDragStartValue = 0;
	bool        mSnapToInt = false;
	ci::ColorA	mBorderColor = ci::ColorA::gray( 1, 0.4f );
	ci::ColorA	mTitleColor = ci::ColorA::gray( 1, 0.6f );
	std::string	mTitle;
	TextRef		mTextLabel;
	TapTracker	mTapTracker;
};

template <typename T>
class CI_UI_API NumberBoxT : public Control {
  public:
	NumberBoxT( const ci::Rectf &bounds = ci::Rectf::zero() );

	//!
	void setValue( const T &value );
	//!
	const T&	getValue() const	{ return mValue; }

	//! Returns the number of components
	size_t getSize() const	{ return mValue.size(); }


	//! Sets the minimum value, defaults to smallest (negative) possible float
	void setMin( float min );
	//! Sets the maximum value, defaults to largest possible float
	void setMax( float max );
	//! Sets the amount that the value is changed per pixel step when dragged. Default is 1.
	void setStep( float step );
	//!
	void setBorderColor( const ci::ColorA &color );

	enum TitlePosition { TOP, LEFT,	RIGHT };

	void				setTitle( const std::string &title, TitlePosition position = TitlePosition::TOP );
	const std::string&	getTitle() const;

  private:
	void onValueChanged();

	ui::LabelRef				mTitleLabel;
	ui::ViewRef					mControlContainer;
	std::vector<NumberBoxRef>	mNumberBoxes;
	T							mValue;

	ci::ColorA					mTitleColor	= ci::ColorA::gray( 1, 0.6f );
	TitlePosition				mTitlePosition = TitlePosition::TOP; // TODO: use this, currently only supporting TOP

};

typedef NumberBoxT<float>		NumberBox1;
typedef NumberBoxT<ci::vec2>	NumberBox2;
typedef NumberBoxT<ci::vec3>	NumberBox3;
typedef NumberBoxT<ci::vec4>	NumberBox4;

typedef std::shared_ptr<NumberBox1>	NumberBox1Ref;
typedef std::shared_ptr<NumberBox2>	NumberBox2Ref;
typedef std::shared_ptr<NumberBox3>	NumberBox3Ref;
typedef std::shared_ptr<NumberBox4>	NumberBox4Ref;

} // namespace ui

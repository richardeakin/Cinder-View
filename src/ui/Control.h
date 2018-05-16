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
#include "ui/ImageView.h"
#include "ui/Label.h"
#include "ui/GestureTracker.h"

namespace ui {

typedef std::shared_ptr<class Control>		ControlRef;
typedef std::shared_ptr<class Button>		ButtonRef;
typedef std::shared_ptr<class CheckBox>		CheckBoxRef;
typedef std::shared_ptr<class TextField>    TextFieldRef;

typedef std::shared_ptr<class HSlider>		HSliderRef;
typedef std::shared_ptr<class VSlider>		VSliderRef;
typedef std::shared_ptr<class VSelector>	VSelectorRef;
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

class CI_UI_API Button : public Control {
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
	void update() override;

	bool touchesBegan( ci::app::TouchEvent &event )	override;
	bool touchesMoved( ci::app::TouchEvent &event )	override;
	bool touchesEnded( ci::app::TouchEvent &event )	override;

	bool        mEnabled = false;
	bool        mIsToggle = false;
	State       mState = State::NORMAL;

	ci::ColorA	mColorNormal = ci::ColorA::gray( 0.5f );
	ci::ColorA	mColorEnabled = ci::ColorA::gray( 0.38f );
	ci::ColorA	mColorPressed = ci::ColorA::gray( 0.3f );
	ci::ColorA	mColorTitleNormal = ci::ColorA::gray( 0.2f );
	ci::ColorA	mColorTitleEnabled = ci::ColorA::gray( 0.2f );
	bool		mHasColorTitleEnabled = false; // keep track of when user hasn't set the title color for enabled, will use normal color otherwise

	ImageViewRef	mImageView;
	LabelRef		mTitleLabel;
	std::string		mTitleNormal, mTitleEnabled;

	ImageRef   mImageNormal, mImageEnabled, mImagePressed;

	ci::signals::Signal<void ()>	mSignalPressed, mSignalReleased; // TODO: look at other frameworks (like html5, dart) and see what they name these, and how they organize the events
};

//! Toggle Button with text off to the right side.
class CI_UI_API CheckBox : public Button {
  public:
	CheckBox( const ci::Rectf &bounds = ci::Rectf::zero() );

  protected:
	void draw( Renderer *ren )	override;

  private:
	TextRef      mTextTitle;
};

//! Text input Control
class CI_UI_API TextField : public Control {
  public:
	enum class State { NORMAL, SELECTED /*, PRESSED */ };
	enum class InputMode { TEXT, NUMERIC };
	enum class BorderMode { LINE, DISABLED };

	TextField( const ci::Rectf &bounds = ci::Rectf::zero() );

	void setBorderColor( const ci::ColorA &color, State state = State::NORMAL );
	void setTextColor( const ci::ColorA &color, State state = State::NORMAL );

	void				setText( const std::string &text )				{ mInputString = text; }
	const std::string&	getText() const									{ return mInputString; }
	void				setPlaceholderText( const std::string &text );
	const std::string&	getPlaceholderText() const						{ return mPlaceholderString; }

	void		setInputMode( InputMode mode )		{ mInputMode = mode; }
	InputMode	getInputMode() const				{ return mInputMode; }
	void		setBorderMode( BorderMode mode )	{ mBorderMode = mode; }
	BorderMode	getBorderMode() const				{ return mBorderMode; }

	ci::signals::Signal<void ()>&	getSignalmSignalTextInputBegin()	{ return mSignalTextInputBegin; }
	ci::signals::Signal<void ()>&	getSignalTextInputCompleted()	{ return mSignalTextInputCompleted; }
	ci::signals::Signal<void ()>&	getSignalTextInputCanceled()	{ return mSignalTextInputCanceled; }

  private:
	void	draw( Renderer *ren )	override;

	bool	willBecomeFirstResponder() override;
	bool	willResignFirstResponder() override;

	bool	keyDown( ci::app::KeyEvent &event ) override;
	bool	checkCharIsValid( char c ) const;

	TextRef		mText;
	std::string mInputString, mInputStringBeforeInput;
	std::string mPlaceholderString;
	int			mCursorPos = -1; // position of next character input. -1 indicates it's never been set and will be at the end of the text once we're first responder

	InputMode	mInputMode = InputMode::TEXT;
	BorderMode  mBorderMode = BorderMode::LINE;
	ci::ColorA	mBorderColorNormal = ci::ColorA::gray( 1, 0.4f );
	ci::ColorA	mBorderColorSelected = ci::ColorA::gray( 1, 0.6f );
	ci::ColorA	mTextColorNormal = ci::ColorA::gray( 1, 0.6f );
	ci::ColorA	mTextColorSelected = ci::ColorA::white();

	ci::signals::Signal<void ()>	mSignalTextInputBegin, mSignalTextInputCompleted, mSignalTextInputCanceled;
};

class CI_UI_API SliderBase : public Control {
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
	void onDoubleTap();

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
	TapTracker	mTapTracker;
};

class CI_UI_API HSlider : public SliderBase {
  public:
	HSlider( const ci::Rectf &bounds = ci::Rectf::zero() ) : SliderBase( bounds )	{}

  protected:
	ci::Rectf	getValueRect( float sliderPos, float sliderRadius ) const	override;
	float		getValuePercentage( const ci::vec2 &pos )					override;
};

class CI_UI_API VSlider : public SliderBase {
  public:
	VSlider( const ci::Rectf &bounds = ci::Rectf::zero() ) : SliderBase( bounds )	{}

  protected:
	ci::Rectf	getValueRect( float sliderPos, float sliderRadius ) const	override;
	float		getValuePercentage( const ci::vec2 &pos )					override;
};

class CI_UI_API SelectorBase : public Control {
  public:
	SelectorBase( const ci::Rectf &bounds = ci::Rectf::zero() );

	const std::vector<std::string>&	getSegmentLabels() const			{ return mSegments; }
	std::vector<std::string>&		getSegmentLabels()					{ return mSegments; }

	void	setSegmentLabels( const std::vector<std::string> &segments )	{ mSegments = segments; }

	size_t				getSelectedIndex() const	{ return mSelectedIndex; }
	const std::string&	getSelectedLabel() const;

	const std::string&	getTitle() const						{ return mTitle; }
	void				setTitle( const std::string &title )	{ mTitle = title; }

	void				setSelectedColor( const ci::ColorA &color )		{ mSelectedColor = color; }
	const ci::ColorA&	getSelectedColor() const						{ return mSelectedColor; }
	void				setUnselectedColor( const ci::ColorA &color )	{ mUnselectedColor = color; }
	const ci::ColorA&	getUnselectedColor() const						{ return mUnselectedColor; }
	void				setTitleColor( const ci::ColorA &color )		{ mTitleColor = color; }
	const ci::ColorA&	getTitleColor() const							{ return mTitleColor; }

	//! Causes value changed signal to be fired if the selection changes.
	void select( size_t index );
	//! Causes value changed signal to be fired if the selection changes.
	void select( const std::string &label );

  protected:
	void draw( Renderer *ren )	override;

	bool touchesBegan( ci::app::TouchEvent &event )	override;
	bool touchesMoved( ci::app::TouchEvent &event )	override;
	bool touchesEnded( ci::app::TouchEvent &event )	override;

	void updateSelection( const ci::vec2 &pos );

	std::vector<std::string>	mSegments;
	size_t						mSelectedIndex = 0;
	ci::ColorA					mSelectedColor = ci::ColorA::gray( 1, 0.9f );
	ci::ColorA					mUnselectedColor = ci::ColorA::gray( 1, 0.5f );
	ci::ColorA					mTitleColor = ci::ColorA::gray( 0.75f, 0.5f );
	std::string					mTitle;
	TextRef						mTextLabel;
};

class CI_UI_API VSelector : public SelectorBase {
  public:
	VSelector( const ci::Rectf &bounds = ci::Rectf::zero() ) : SelectorBase( bounds )	{}

  protected:
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

	void				setTitle( const std::string &title );
	const std::string&	getTitle() const						{ return mTitle; }

	float getValue() const	{ return mValue; }

	void setValue( float value, bool emitChanged = true );

	void setBorderColor( const ci::ColorA &color )	{ mBorderColor = color; }
	void setTitleColor( const ci::ColorA &color )	{ mTitleColor = color; }

	void setSnapToIntEnabled( bool enable )	{ mSnapToInt = enable; }
	bool isSnapToIntEnabled() const			{ return mSnapToInt; }

	//! Specifies the number of digits that will be used when formatting the decimal portion of the value as a string.
	void setNumDigits( size_t numDigits );
	//! Returns the number of digits that will be used when formatting the decimal portion of the value as a string.
	size_t	getNumDigits() const	{ return mNumDigits; }

	void setNextResponder( const ViewRef &view ) override;

  protected:
	void layout() override;
	void draw( Renderer *ren )	override;

	bool touchesBegan( ci::app::TouchEvent &event )	override;
	bool touchesMoved( ci::app::TouchEvent &event )	override;
	bool touchesEnded( ci::app::TouchEvent &event )	override;

	std::string	getTitleLabel() const;

	void updateValue( const ci::vec2 &pos );

  private:
	void onDoubleTap();
	void onTextInputBegin();
	void onTextInputUpdated();
	void onTextInputCompleted();
	std::string	getValueAsString() const;
	void updateValueFromTextField();

	float	mValue = 0;
	float	mMin;
	float	mMax;
	float	mStep = 1;

	ci::vec2	mDragStartPos;
	float		mDragStartValue = 0;
	bool        mSnapToInt = false;
	float		mPadding = 6;
	ci::ColorA	mBorderColor = ci::ColorA::gray( 1, 0.4f );
	ci::ColorA	mTitleColor = ci::ColorA::gray( 1, 0.6f );
	std::string	mTitle;
	TextRef		mTextLabel;
	TapTracker	mTapTracker;

	size_t				mNumDigits;
	std::string			mFormatStr;
	ui::TextFieldRef	mTextField;
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
	int getSize() const	{ return mValue.length(); }

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

	void setNextResponder( const ViewRef &view ) override;

  protected:

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

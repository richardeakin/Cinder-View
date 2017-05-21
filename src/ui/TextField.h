/*
Copyright (c) 2017, Richard Eakin - All rights reserved.

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

typedef std::shared_ptr<class TextField>    TextFieldRef;

// TODO: use this to 
enum class TextMode {
    NUMERIC,
    ASCII
};

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

	ci::signals::Signal<void ()>	mSignalTextInputCompleted, mSignalTextInputCanceled;
};

} // namespace ui

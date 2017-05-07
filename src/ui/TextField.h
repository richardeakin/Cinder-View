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

	TextField( const ci::Rectf &bounds = ci::Rectf::zero() );

	void setBorderColor( const ci::ColorA &color, State state = State::NORMAL );
	void setTextColor( const ci::ColorA &color, State state = State::NORMAL );

	void	setText( const std::string &text )				{ mInputString = text; }
	void	setPlaceholderText( const std::string &text );

  private:

	void draw( Renderer *ren )	override;

	bool	willBecomeFirstResponder() override;
	bool	willResignFirstResponder() override;

	bool	keyDown( ci::app::KeyEvent &event ) override;


	TextRef		mText;
	std::string mInputString;
	std::string mPlaceholderString;

	ci::ColorA	mBorderColorNormal = ci::ColorA::gray( 1, 0.4f );
	ci::ColorA	mBorderColorSelected = ci::ColorA::gray( 1, 0.6f );
	ci::ColorA	mTextColorNormal = ci::ColorA::gray( 1, 0.6f );
	ci::ColorA	mTextColorSelected = ci::ColorA::white();
};

} // namespace ui

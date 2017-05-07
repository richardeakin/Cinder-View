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

#include "ui/TextField.h"
#include "cinder/Log.h"

using namespace ci;
using namespace std;

namespace ui {

TextField::TextField( const ci::Rectf &bounds )
	: Control( bounds )
{
	setAcceptsFirstResponder( true );

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
		setLabel( text );
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

	if( isFirstResponder() ) {
		// draw cursor position
		const float cursorThickness = 1;
		const float nextCharOffset = 6;
		vec2 cursorLoc = { 0, 0 };
		if( mCursorPos > 0 ) {
			// measure where the cursor should be drawn (where the next character will be inserted)
			string stringUntilCursor = mInputString.substr( 0, mCursorPos );
			cursorLoc = mText->measureString( stringUntilCursor );
		}

		cursorLoc.x += nextCharOffset;
		Rectf cursorRect = { cursorLoc.x - cursorThickness / 2, 0, cursorLoc.x + cursorThickness / 2, getHeight() };
		ren->setColor( mBorderColorSelected );
		ren->drawSolidRect( cursorRect );
	}

	// draw border
	{
		auto color = isFirstResponder() ? mBorderColorSelected : mBorderColorNormal;
		ren->setColor( color );
		ren->drawStrokedRect( getBoundsLocal(), 2 );
	}
}

bool TextField::willBecomeFirstResponder()
{
	CI_LOG_I( getName() );
	if( mCursorPos < 0 ) {
		// set cursor position to one character after the current input string
		mCursorPos = mInputString.size();
	}
	return true;
}

bool TextField::willResignFirstResponder()
{
	CI_LOG_I( getName() );
	return true;
}

bool TextField::keyDown( ci::app::KeyEvent &event )
{   
	//CI_LOG_I( "char: " << ( event.getChar() ? event.getChar() : 0 ) << ", char utf32: " << event.getCharUtf32() << ", code: " << event.getCode() 
	//	<< ", shift down: " << event.isShiftDown() << ", alt down: " << event.isAltDown() << ", ctrl down: " << event.isControlDown()
	//	<< ", meta down: " << event.isMetaDown() << ", accel down: " << event.isAccelDown() << ", native code: " << event.getNativeKeyCode() );

	if( event.getCode() == app::KeyEvent::KEY_BACKSPACE ) {
		//CI_LOG_I( "\t- delete, string length: " << mInputString.size() );

		if( mCursorPos > 0 && mCursorPos - 1 < mInputString.size() ) {
			mInputString.erase( mCursorPos - 1, 1 );
			mCursorPos -= 1;
		}

		CI_LOG_I( "(backspace) string size: " << mInputString.size() << ", cursor pos: " << mCursorPos );

		return true;
	}
	else if( event.getCode() == app::KeyEvent::KEY_RIGHT ) {
		// move cursor to the right, if possible
		if( mCursorPos < mInputString.size() ) {
			mCursorPos += 1;
		}

		CI_LOG_I( "(right) string size: " << mInputString.size() << ", cursor pos: " << mCursorPos );
	}
	else if( event.getCode() == app::KeyEvent::KEY_LEFT ) {
		// move cursor to the right, if possible
		if( mCursorPos > 0 ) {
			mCursorPos -= 1;
		}

		CI_LOG_I( "(left) string size: " << mInputString.size() << ", cursor pos: " << mCursorPos );
		return true;
	}
	else if( event.getChar() ) {
		auto c = event.getChar();
		// TODO: add numerical only mode
		//if( mMode == Format::NUMERICAL && ! isdigit( c ) )
		//    return;

		//CI_LOG_I( "\t- adding char: " << c );

		mInputString.insert( mCursorPos, 1, c );
		//mInputString.push_back( c );
		mCursorPos += 1;
		CI_LOG_I( "(enter char) string size: " << mInputString.size() << ", cursor pos: " << mCursorPos );

		return true;
	}

	return false;
}

} // namespace ui

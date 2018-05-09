/*
 Copyright (c) 2015, Richard Eakin - All rights reserved.

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
#include "ui/TextManager.h"

namespace ui {

typedef std::shared_ptr<class Label>		LabelRef;
typedef std::shared_ptr<class LabelGrid>	LabelGridRef;

//! Basic View to draw text. Non-interactive by default.
class CI_UI_API Label : public View {
public:
	Label( const ci::Rectf &bounds = ci::Rectf::zero() );

	void				setText( const std::string &text );
	const std::string&	getText() const						{ return mText; }

	void				setTextColor( const ci::ColorA &color )	{ mTextColor = color; }
	const ci::ColorA&	getTextColor() const					{ return mTextColor; }

	// TODO: need method for setting font by string
	// - will ask ui::TextManager if it is loaded or perhaps provide path to file?
	void                setFont( const std::string &key );

	void				setFont( float fontSize, FontFace fontFace );
	void				setFontSize( float fontSize )		{ setFont( fontSize, mFont->getFace() ); }
	void				setFontFace( FontFace face )		{ setFont( mFont->getSize(), face ); }

	void				setAlignment( TextAlignment alignment )	{ mAlignment = alignment; }

	void				setPadding( const ci::Rectf &padding );
	void				setShrinkToFitEnabled( bool enable = true );
	bool				isShrinkToFitEnabled() const	{ return mShrinkToFit; }

	void				setWrapEnabled( bool enable = true );
	bool				isWrapEnabled() const	{ return mWrapEnabled; }
	//void				setLineSpace( float lineSpace ) {}
	//! Updates rectangular region that text should be laid out in.
	void				setSize( const ci::vec2 &size ) override;
	//! note: not const because SdfText::getBounds() isn't const
	ci::vec2			getTextSize() const	{ return mTextSize; }

protected:
	void layout() override;
	void draw( Renderer *ren ) override;

private:
	ci::vec2	getBaseLine() const;
	void		adjustSizeIfNeeded();
	void		measureTextSize();

	TextRef			mFont; // TODO: should this be named mText, and below named mTextString? Or think of different name for ui::Text
	std::string		mText;
	ci::vec2		mTextSize; // TODO: move this to ui::Text
	ci::ColorA		mTextColor = ci::ColorA::black();
	ci::Rectf		mPadding = ci::Rectf( 4, 4, 4, 4 );

	TextAlignment	mAlignment = TextAlignment::LEFT;
	bool			mWrapEnabled = true;
	bool			mShrinkToFit = false;
};

//! Manages a grid of text entries, useful for building things like info panels. Non-interactive by default.
class CI_UI_API LabelGrid : public View {
public:
	LabelGrid( const ci::Rectf &bounds = ci::Rectf::zero() );

	void setCell( const ci::ivec2 &location, const std::string &text );
	void setCell( size_t xloc, size_t yloc, const std::string &text )	{ setCell( ci::ivec2( xloc, yloc ), text ); }

	void setRow( size_t yloc, const std::vector<std::string> &textColumns );

	void setCellHeight( float height )	            { mCellHeight = height; }
	//! Sets the default color for all cells
	void setTextColor( const ci::ColorA &color )	{ mTextColor = color; }
	//! Returns the number of rows currently set.
	int getNumRows() const;
	//! Removes all cells.
	void clearCells();
	
protected:
	void layout() override;
private:
	struct Cell {
		ci::ivec2	mLocation;
		LabelRef	mLabel;
	};

	LabelRef	makeOrFindCell( const ci::ivec2 &location );

	std::vector<Cell>	mCells;
	ci::ivec2			mGridExtents = { 0, 0 }; // TODO: use uvec2 once it is added to cinder/Vector.h
	float				mCellHeight = 20;
	ci::ColorA			mTextColor = ci::ColorA::black();
};

} // namespace ui

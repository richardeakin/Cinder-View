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


#include "ui/Label.h"

#include "cinder/Log.h"
#include "cinder/CinderAssert.h"

#include "cppformat/format.h"

using namespace std;
using namespace ci;

namespace ui {

// ----------------------------------------------------------------------------------------------------
// Label
// ----------------------------------------------------------------------------------------------------

Label::Label( const Rectf &bounds )
	: View( bounds )
{
	setFont( -1, FontFace::NORMAL );
	setInteractive( false );
	setBlendMode( BlendMode::PREMULT_ALPHA );
}

void Label::setFont( float fontSize, FontFace fontFace )
{
	mFont = TextManager::loadText( fontFace, fontSize );
}

void Label::setText( const std::string &text )
{
	if( mText == text )
		return;

	mText = text;
	mTextSize = mFont->measureString( text );
}

void Label::draw( Renderer *ren )
{
	if( mText.empty() )
		return;

	ren->setColor( mTextColor );
	mFont->drawString( mText, getBaseLine() );
}

vec2 Label::getBaseLine() const
{
	float x = 0;
	switch( mAlignment ) {
		case TextAlignment::LEFT:
			x = mPadding.x1;
		break;
		case TextAlignment::CENTER:
			x = ( getWidth() - mTextSize.x ) / 2;
		break;
		case TextAlignment::RIGHT:
			x = getWidth() - mTextSize.x - mPadding.x2;
		break;
		default: CI_ASSERT_NOT_REACHABLE();
	}

	vec2 result( x, getCenterLocal().y + mFont->getDescent() );
	return result;
}

// ----------------------------------------------------------------------------------------------------
// LabelGrid
// ----------------------------------------------------------------------------------------------------

LabelGrid::LabelGrid( const Rectf &bounds )
	: View( bounds )
{
	setInteractive( false );
}

void LabelGrid::setCell( const ci::ivec2 &location, const string &text )
{
	auto label = makeOrFindCell( location );
	label->setText( text );
}

void LabelGrid::setRow( size_t yloc, const std::vector<std::string> &textColumns )
{
	for( size_t i = 0; i < textColumns.size(); i++ )
		setCell( ivec2( i, yloc ), textColumns[i] );
}

LabelRef LabelGrid::makeOrFindCell( const ci::ivec2 &location )
{
	CI_ASSERT( location.x >= 0 && location.y >= 0 );

	for( const auto &cell : mCells ) {
		if( cell.mLocation == location )
			return cell.mLabel;
	}

	mCells.push_back( Cell() );

	Cell &cell= mCells.back();
	cell.mLocation = location;
	cell.mLabel = make_shared<Label>();
	cell.mLabel->setClipEnabled();

	// TODO: make settable per row / column / cell
	cell.mLabel->setTextColor( mTextColor );

	// TODO: make settable per row / column / cell
	if( location.x == 0 )
		cell.mLabel->setAlignment( TextAlignment::RIGHT );

	addSubview( cell.mLabel );

	if( mGridExtents.x < location.x )
		mGridExtents.x = location.x;
	if( mGridExtents.y < location.y )
		mGridExtents.y = location.y;

	return cell.mLabel;
}

int LabelGrid::getNumRows() const
{
	if( mCells.empty() )
		return 0;

	return mGridExtents.y + 1;
}

void LabelGrid::clearCells()
{
	mCells.clear();
	removeAllSubviews();
	mGridExtents = { 0, 0 };
}

void LabelGrid::layout()
{
	vec2 dims = mGridExtents + ivec2( 1 );
	vec2 cellSize = vec2( getWidth() / dims.x, mCellHeight );

//	CI_LOG_V( "view size: " << getSize() << ", cellSize: " << cellSize << ", dims: " << dims );

	for( const auto &cell : mCells ) {
		vec2 pos = cellSize * vec2( cell.mLocation );

		Rectf bounds = { pos, pos + cellSize };
		cell.mLabel->setBounds( bounds );

//		cout << "location: " << cell.mLocation << ", bounds: " << bounds << endl;
	}
}

} // namespace ui

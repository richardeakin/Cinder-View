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

#include "ui/Selector.h"

#include "cinder/Log.h"

using namespace ci;
using namespace std;

namespace ui {

SelectorBase::SelectorBase( const Rectf &bounds )
	: Control( bounds )
{
	mTextLabel = TextManager::loadText( FontFace::NORMAL );
}

void SelectorBase::draw()
{
	auto renderer = getRenderer();

	const float padding = 10;

	float sectionHeight = getHeight() / (float)mSegments.size();
	Rectf section( 0, 0, getWidth(), sectionHeight );

	renderer->setColor( mUnselectedColor );
	for( size_t i = 0; i < mSegments.size(); i++ ) {
		if( i != mSelectedIndex ) {
			renderer->drawStrokedRect( section );
			mTextLabel->drawString( mSegments[i], vec2( section.x1 + padding, section.getCenter().y + mTextLabel->getDescent() ) );
		}
		section += vec2( 0.0f, sectionHeight );
	}

	renderer->setColor( mSelectedColor );

	section.y1 = mSelectedIndex * sectionHeight;
	section.y2 = section.y1 + sectionHeight;
	renderer->drawStrokedRect( section );

	if( ! mSegments.empty() ) {
		mTextLabel->drawString( mSegments[mSelectedIndex], vec2( section.x1 + padding, section.getCenter().y + mTextLabel->getDescent() ) );
	}

	if( ! mTitle.empty() ) {
		renderer->setColor( mTitleColor );
		mTextLabel->drawString( mTitle, vec2( padding, - mTextLabel->getDescent() ) );
	}
}


bool SelectorBase::touchesBegan( const app::TouchEvent &event )
{
	setTouchCanceled( false );
	vec2 pos = toLocal( event.getTouches().front().getPos() );

	updateSelection( pos );
	return true;
}

bool SelectorBase::touchesMoved( const ci::app::TouchEvent &event )
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

bool SelectorBase::touchesEnded( const ci::app::TouchEvent &event )
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
	int offset = pos.y - (int)getPos().y;
	int sectionHeight = (int)getHeight() / mSegments.size();
	size_t selectedIndex = std::min<size_t>( offset / sectionHeight, mSegments.size() - 1 );

	if( mSelectedIndex != selectedIndex ) {
		mSelectedIndex = selectedIndex;
		mSignalValueChanged.emit();
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
		mSignalValueChanged.emit();
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

} // namespace ui

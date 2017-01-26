/*
 Copyright (c) 2016, The Cinder Project, All rights reserved.

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

#include "ui/Layout.h"
#include "ui/View.h"

#include <functional>
#include <numeric>

using namespace ci;
using namespace std;
using namespace ui;

namespace {

void updateAxisPos( const ui::ViewRef &view, float position, int axis )
{
	auto current = view->getPos();
	current[axis] = position;
	view->setPos( current );
}

} // anonymous namespace

ui::LinearLayout::LinearLayout( Orientation orientation, Mode mode, Alignment alignment )
	: mOrientation( orientation ), mMode( mode ), mAlignment( alignment )
{

}

void ui::LinearLayout::layout( View *view )
{
	int axis = (int)mOrientation;
	int axis2 = ( axis + 1 ) % 2;
	const auto subviews = view->getSubviews();

	vec2 containerSize = view->getSize();
	float containerSizeMinusMargins = containerSize[axis] - mMargin.getUpperLeft()[axis] - mMargin.getLowerRight()[axis];
	float paddingTotal = glm::max( 0.0f, float( subviews.size() ) - 1.0f ) * mPadding;
	float subviewsTotal = std::accumulate( subviews.begin(), subviews.end(), 0.0f, [&] ( float sum, const ui::ViewRef &view ) {
		return sum + view->getSize()[axis];
	} );

	// Update layout based on selected mode and primary axis.
	float offset = mMargin.getUpperLeft()[axis];
	for( auto &subview : subviews ) {
		if( mMode == Mode::DISTRIBUTE ) {
			offset += ( containerSizeMinusMargins - subviewsTotal ) / float( subviews.size() + 1 );
			updateAxisPos( subview, offset, axis );
			offset += subview->getSize()[axis];
		}
		else if( mMode == Mode::FILL ) {
			vec2 size = subview->getSize();
			size[axis] = ( containerSizeMinusMargins - paddingTotal ) / float( subviews.size() );
			subview->setSize( size );
			updateAxisPos( subview, offset, axis );
			offset += subview->getSize()[axis] + mPadding;
		}
		else {
			updateAxisPos( subview, offset, axis );
			offset += subview->getSize()[axis] + mPadding;
		}
	}

	// Update alignment of all subviews on secondary axis.
	offset = mMargin.getUpperLeft()[axis2];
	containerSizeMinusMargins = containerSize[axis2] - mMargin.getUpperLeft()[axis2] - mMargin.getLowerRight()[axis2];
	for( auto &subview : subviews ) {
		if( mAlignment == Alignment::MINIMUM ) {
			updateAxisPos( subview, offset, axis2 );
		}
		else if( mAlignment == Alignment::MIDDLE ) {
			float center = 0.5f * ( containerSize[axis2] - subview->getSize()[axis2] );
			updateAxisPos( subview, center, axis2 );
		}
		else if( mAlignment == Alignment::MAXIMUM ) {
			float pos = containerSize[axis2] - mMargin.getLowerRight()[axis2] - subview->getSize()[axis2];
			updateAxisPos( subview, pos, axis2 );
		}
		else if( mAlignment == Alignment::FILL ) {
			updateAxisPos( subview, offset, axis2 );

			auto currentSize = subview->getSize();
			currentSize[axis2] = containerSizeMinusMargins;
			subview->setSize( currentSize );
		}
	}
}

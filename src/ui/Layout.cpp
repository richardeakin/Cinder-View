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

ui::LinearLayout::LinearLayout( Orientation orientation, Mode mode )
	: mOrientation{ orientation }, mMode{ mode }
{
}

void ui::LinearLayout::layout( View * view )
{
	int axis = (int)mOrientation;
	const auto subviews = view->getSubviews();

	vec2 containerSize = view->getSize();
	float axisContainerSizeMinusMargins = containerSize[axis] - mMargin.getUpperLeft()[axis] - mMargin.getLowerRight()[axis];
	float axisPaddingTotal = glm::max( 0.0f, float(subviews.size()) - 1.0f ) * mPadding;
	float axisSubviewsTotal = std::accumulate( subviews.begin(), subviews.end(), 0.0f, [&] ( float sum, const ui::ViewRef& view ) {
		return sum + view->getSize()[axis];
	} );

	float i = 0.0f;
	vec2 offset = mMargin.getUpperLeft();
	for( auto &subview : subviews ) {
		if( mMode == Mode::Distribute ) {
			offset[axis] += ( axisContainerSizeMinusMargins - axisSubviewsTotal ) / float( subviews.size() + 1 );
			subview->setPos( offset );
			offset[axis] += subview->getSize()[axis];
		}
		else if( mMode == Mode::Fill ) {
			vec2 size = subview->getSize();
			size[axis] = ( axisContainerSizeMinusMargins - axisPaddingTotal ) / float( subviews.size() );
			subview->setBounds( Rectf( vec2(), size ) );
			subview->setPos( offset );
			offset[axis] += subview->getSize()[axis] + mPadding;
		}
		else {
			subview->setPos( offset );
			offset[axis] += subview->getSize()[axis] + mPadding;
		}
		i += 1.0f;
	}
}

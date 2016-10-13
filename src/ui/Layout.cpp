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

using namespace ci;
using namespace std;
using namespace ui;

BoxLayout::BoxLayout( Orientation orientation, Alignment alignment )
	: mOrientation{ orientation }, mAlignment{ alignment }
{
}

void BoxLayout::layout( View * view )
{
	// TODO: fix padding and margin behavior
	vec2 containerSize = view->getSize();
	int axis1 = (int)mOrientation;
	int axis2 = ((int)mOrientation + 1) % 2;

	vec2 offset = { mMargin.x1, mMargin.y1 };
	const auto subviews = view->getSubviews();
	for( auto &subview : subviews ) {
		if( mAlignment == Alignment::Minimum ) {
			subview->setPos( offset );
		}
		else if( mAlignment == Alignment::Middle ) {
			subview->setPos( 0.5f * ( containerSize - subview->getSize() ) );
		}
		else if( mAlignment == Alignment::Maximum ) {
			subview->setPos( containerSize - subview->getSize() - 2.0f * offset );
		}
		else if( mAlignment == Alignment::Fill ) {
			subview->setPos( offset );
			subview->setSize( containerSize - 2.0f * offset );
		}
	}
}

ui::LinearLayout::LinearLayout( Orientation orientation, Mode mode )
	: mOrientation{ orientation }, mMode{ mode }
{
}

void ui::LinearLayout::layout( View * view )
{
	vec2 containerSize = view->getSize();
	int axis1 = (int)mOrientation;
	int axis2 = ((int)mOrientation + 1) % 2;

	vec2 offset{ 0 };
	float i = 0.0f;
	const auto subviews = view->getSubviews();
	for( auto &subview : subviews ) {
		if( mMode == Mode::CenterDistribute ) {
			auto pos = subview->getPos();
			float center = ( i + 0.5f ) * ( containerSize[axis1] / float( subviews.size() ) );
			pos[axis1] = center - 0.5f * subview->getSize()[axis1];
			subview->setPos( pos );
		}
		else if( mMode == Mode::Fill ) {
			vec2 size = subview->getSize();
			size[axis1] = containerSize[axis1] / float( subviews.size() );
			subview->setBounds( Rectf( vec2(), size ) );
			subview->setPos( offset );
			offset[axis1] += subview->getSize()[axis1];
		}
		else {
			subview->setPos( offset );
			offset[axis1] += subview->getSize()[axis1];
		}
		i += 1.0f;
	}
}

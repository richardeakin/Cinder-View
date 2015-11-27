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

#include "ui/Renderer.h"

#include "cinder/gl/Context.h"
#include "cinder/gl/wrapper.h"
#include "cinder/gl/draw.h"
#include "cinder/Log.h"

using namespace ci;
using namespace std;

namespace ui {

Renderer::Renderer()
{
	mBlendModeStack.push_back( BlendMode::ALPHA );
}

void Renderer::setColor( const ColorA &color )
{
	gl::color( color );
}

void Renderer::pushColor()
{
	mColorStack.push_back( gl::context()->getCurrentColor() );
}

void Renderer::pushColor( const ci::ColorA &color )
{
	pushColor();
	setColor( color );
}

void Renderer::popColor()
{
	if( mColorStack.empty() ) {
		CI_LOG_E( "Color stack underflow" );
	}
	else {
		setColor( mColorStack.back() );
		mColorStack.pop_back();
	}
}

void Renderer::setBlendMode( BlendMode mode )
{
	auto ctx = gl::context();
	switch( mode ) {
		case BlendMode::ALPHA:
			gl::enableAlphaBlending();
		break;
		case BlendMode::PREMULT_ALPHA:
			gl::enableAlphaBlendingPremult();
		break;
		default:
			CI_ASSERT_NOT_REACHABLE();
	}
}

void Renderer::pushBlendMode( BlendMode mode )
{
	mBlendModeStack.push_back( mode );
	setBlendMode( mode );
}

void Renderer::popBlendMode()
{
	if( mBlendModeStack.empty() ) {
		CI_LOG_E( "BlendMode stack underflow" );
	}
	else {
		setBlendMode( mBlendModeStack.back() );
		mBlendModeStack.pop_back();
	}
}

void Renderer::drawSolidRect( const Rectf &rect )
{
	gl::drawSolidRect( rect );
}

void Renderer::drawStrokedRect( const Rectf &rect )
{
	gl::drawStrokedRect( rect );
}

void Renderer::drawStrokedRect( const Rectf &rect, float lineWidth )
{
	gl::drawStrokedRect( rect, lineWidth );
}

} // namespace ui

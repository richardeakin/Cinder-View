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

#include "ImageView.h"
#include "cinder/gl/Batch.h"

using namespace ci;
using namespace std;

namespace vu {

ImageView::ImageView( const ci::Rectf &bounds )
	: View( bounds )
{
	setInteractive( false );
}

void ImageView::setImage( const ImageRef &image )
{
	mImage = image;
}

void ImageView::setShader( const ci::gl::GlslProgRef &glsl )
{
	if( mBatch ) {
		mBatch->replaceGlslProg( glsl );
	}
	else {
		mBatch = gl::Batch::create( geom::Rect( Rectf( 0, 0, 1, 1 ) ),  glsl );
	}
}

ci::gl::GlslProgRef	ImageView::getShader() const
{
	if( mBatch ) {
		return mBatch->getGlslProg();
	}

	return nullptr;
}

void ImageView::draw( Renderer *ren )
{
	if( ! mImage )
		return;

	// TODO: this should be handled at the Renderer level
	if( isRenderTransparencyToFrameBufferEnabled() )
		ren->setColor( getColor() );
	else {
		ColorA color = getColor();
		color.a = getAlphaCombined();
		ren->setColor( color );
	}

	if( mBatch ) {
		ren->draw( mImage, getDestRectLocal(), mBatch );
	}
	else {
		ren->draw( mImage, getDestRectLocal() );
	}
}

Rectf ImageView::getDestRectLocal() const
{
	if( ! mImage ) {
		return Rectf::zero();
	}

	Rectf texBounds = mImage->getBounds();
	auto bounds = getBoundsLocal();

	switch( mScaleMode ) {
		case ImageScaleMode::FIT:
			return Rectf( texBounds ).getCenteredFit( bounds, true );
		case ImageScaleMode::FIT_HORIZONTAL: {
			Rectf result = texBounds;
			result.x2 = bounds.getWidth();
			result.y2 = texBounds.getHeight() * bounds.getWidth() / texBounds.getWidth();
			return result;
		}
		case ImageScaleMode::CENTER: {
			Rectf result( texBounds );
			result.offsetCenterTo( bounds.getCenter() );
			return result;
		}
		case ImageScaleMode::CENTER_HORIZONTAL: {
			Rectf result( texBounds );
			result += bounds.getUpperLeft() + vec2( ( bounds.getWidth() - texBounds.getWidth() ) / 2.0f, 0 );
			return result;
		}
		case ImageScaleMode::FILL:
			return bounds;
		case ImageScaleMode::NONE:
			return texBounds + bounds.getUpperLeft();
		default:
			CI_ASSERT_NOT_REACHABLE();
	}

	return Rectf::zero();
}

std::string toString( const ImageScaleMode &scaleMode )
{
	switch( scaleMode ) {
		case ImageScaleMode::FIT:				return "FIT";
		case ImageScaleMode::CENTER:			return "CENTER";
		case ImageScaleMode::CENTER_HORIZONTAL: return "CENTER_HORIZONTAL";
		case ImageScaleMode::FILL:				return "FILL";
		case ImageScaleMode::NONE:				return "NONE";
		default:								CI_ASSERT_NOT_REACHABLE();
	}

	return "(unhandled enum value)";
}

std::ostream& operator<<( std::ostream &os, const ImageScaleMode &rhs )
{
	os << toString( rhs );
	return os;
}

} // namespace vu

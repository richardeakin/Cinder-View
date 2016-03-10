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

#include "cinder/gl/Batch.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/wrapper.h"
#include "cinder/gl/draw.h"
#include "cinder/gl/scoped.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Log.h"

#define ENABLE_FRAMEBUFFER_CACHING 1

//#define LOG_FRAMEBUFFER( stream )	CI_LOG_I( stream )
#define LOG_FRAMEBUFFER( stream )	    ( (void)( 0 ) )

using namespace ci;
using namespace std;

namespace ui {

namespace {

const string FRAMEBUFFER_VERT = R"(
#version 400

uniform mat4 ciModelViewProjection;

uniform vec2 uPositionOffset, uPositionScale;
uniform vec2 uTexCoordOffset, uTexCoordScale;

in vec4 ciPosition;
in vec2 ciTexCoord0;
in vec4 ciColor;

out highp vec2 vTexCoord;
out lowp vec4 vColor;

void main()
{
//	gl_Position = ciModelViewProjection * ( vec4( uPositionOffset, 0, 0 ) + vec4( uPositionScale, 1, 1 ) * ciPosition );
//	vTexCoord = uTexCoordOffset + uTexCoordScale * ciTexCoord0;

	gl_Position = ciModelViewProjection * ciPosition;
	vTexCoord = ciTexCoord0;
	vColor = ciColor;
}
)";

const string FRAMEBUFFER_FRAG = R"(
#version 400

uniform sampler2D uTex0;

in vec2	vTexCoord;
in vec4 vColor;

out vec4 oFragColor;

void main()
{
	oFragColor = texture( uTex0, vTexCoord.st ) * vColor;
}
)";

} // anonymous namespace

// ----------------------------------------------------------------------------------------------------
// FrameBuffer
// ----------------------------------------------------------------------------------------------------

bool FrameBuffer::Format::operator==(const Format &other) const
{
	return mSize == other.mSize;
}

FrameBuffer::FrameBuffer( const Format &format )
{
	auto fboFormat = gl::Fbo::Format();
	fboFormat.colorTexture(
			gl::Texture2d::Format()
					.internalFormat( GL_RGBA )
					.minFilter( GL_LINEAR ).magFilter( GL_LINEAR )
	);

	mFbo = gl::Fbo::create( format.mSize.x, format.mSize.y, fboFormat );
	LOG_FRAMEBUFFER( "created FrameBuffer of size: " << format.mSize );
}

ivec2 FrameBuffer::getSize() const
{
	return mFbo->getSize();
}

bool FrameBuffer::isUsable() const
{
	return ! mDiscarded && ! mIsBound;
}

ImageSourceRef FrameBuffer::createImageSource() const
{
	return mFbo->getColorTexture()->createSource();
}

// ----------------------------------------------------------------------------------------------------
// Renderer
// ----------------------------------------------------------------------------------------------------

Renderer::Renderer()
{
	mBlendModeStack.push_back( BlendMode::ALPHA );
}

void Renderer::setColor( const ColorA &color )
{
	if( mBlendModeStack.back() == BlendMode::PREMULT_ALPHA ) {
		gl::color( color.r * color.a, color.g * color.a, color.b * color.a, color.a );
	}
	else {
		gl::color( color );
	}
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
	CI_ASSERT_MSG( ! mColorStack.empty(), "Color stack underflow" );

	setColor( mColorStack.back() );
	mColorStack.pop_back();
}

void Renderer::setBlendMode( BlendMode mode )
{
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
	mBlendModeStack.pop_back();
	CI_ASSERT_MSG( ! mBlendModeStack.empty(), "BlendMode stack underflow" );

	setBlendMode( mBlendModeStack.back() );
}

FrameBufferRef Renderer::getFrameBuffer( const ci::ivec2 &size )
{
	auto availableFrameBufferIt = mFrameBufferCache.end();
	for( auto frameBufferIt = mFrameBufferCache.begin(); frameBufferIt < mFrameBufferCache.end(); ++frameBufferIt ) {
		auto &frameBuffer = *frameBufferIt;
		if( frameBuffer->isBound() )
			continue;

		// Check for any unbound FrameBuffer large enough for the requested size
		if( frameBuffer->getSize().x >= size.x && frameBuffer->getSize().y >= size.y ) {
			return frameBuffer;
		}

		// store the largest unbound FrameBuffer, will replace this with one large enough if we don't find a suitable one in the cache
		if( availableFrameBufferIt == mFrameBufferCache.end() ) {
			availableFrameBufferIt = frameBufferIt;
			continue;
		}

		int currLargestArea = (*availableFrameBufferIt)->getSize().x * (*availableFrameBufferIt)->getSize().y;
		int area = frameBuffer->getSize().x * frameBuffer->getSize().y;
		if( area > currLargestArea ) {
			availableFrameBufferIt = frameBufferIt;
		}
	}

	// Make a new one.
	auto format = FrameBuffer::Format().size( size );
	auto result = make_shared<FrameBuffer>( format );

	// Replace the largest unbound FrameBuffer, or push another one if one wasn't available
	if( availableFrameBufferIt != mFrameBufferCache.end()  ) {
		auto old = *availableFrameBufferIt;
		LOG_FRAMEBUFFER( "discarding FrameBuffer of size: " << old->getSize() );
		old->mDiscarded = true;
		*availableFrameBufferIt = result;
	}
	else {
		mFrameBufferCache.push_back( result );
	}
	return result;
}

void Renderer::pushFrameBuffer( const FrameBufferRef &frameBuffer )
{
	frameBuffer->mIsBound = true;
	gl::context()->pushFramebuffer( frameBuffer->mFbo );
}

void Renderer::popFrameBuffer( const FrameBufferRef &frameBuffer )
{
	frameBuffer->mIsBound = false;
	gl::context()->popFramebuffer();
}

void Renderer::draw( const FrameBufferRef &frameBuffer, const Rectf &destRect )
{
	if( ! mGlslFrameBuffer ) {
		try {
			// TODO: add support for drawing partial textures
//			mGlslFrameBuffer = gl::GlslProg::create( FRAMEBUFFER_VERT, FRAMEBUFFER_FRAG );
			mGlslFrameBuffer = gl::getStockShader( gl::ShaderDef().texture().color() );
			CI_LOG_I( "loaded mGlslFrameBuffer" );
		}
		catch( Exception &exc ) {
			CI_LOG_EXCEPTION( "failed to load mGlslFrameBuffer", exc );
		}
	}

	gl::ScopedGlslProg glslScope( mGlslFrameBuffer );
	gl::ScopedTextureBind texScope( frameBuffer->mFbo->getColorTexture() );

	gl::drawSolidRect( destRect );
}

void Renderer::draw( const FrameBufferRef &frameBuffer, const ci::Area &sourceArea, const ci::Rectf &destRect )
{
	gl::draw( frameBuffer->mFbo->getColorTexture(), sourceArea, destRect );
}

void Renderer::draw( const ImageRef &image, const ci::Rectf &destRect, const ci::gl::GlslProgRef &glsl )
{
	// TODO: use Batch. if no glsl then use a default one, replacing on Batch as necessary
	if( glsl ) {
		gl::ScopedGlslProg glslScope( glsl );
		gl::ScopedTextureBind texScope( image->mTexture );

		gl::drawSolidRect( destRect );
	}
	else {
		gl::draw( image->mTexture, destRect );
	}
}

void Renderer::drawSolidRect( const Rectf &rect )
{
	if( ! mBatchSolidRect ) {
		mBatchSolidRect = gl::Batch::create( geom::Rect( Rectf( 0, 0, 1, 1 ) ), gl::getStockShader( gl::ShaderDef().color() ) );
	}

	gl::ScopedModelMatrix modelScope;
	gl::translate( rect.getUpperLeft() );
	gl::scale( rect.getSize() );
	mBatchSolidRect->draw();
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

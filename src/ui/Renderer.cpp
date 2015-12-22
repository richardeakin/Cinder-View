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

#include <cinder/gl/scoped.h>
#include "ui/Renderer.h"

#include "cinder/gl/Context.h"
#include "cinder/gl/wrapper.h"
#include "cinder/gl/draw.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Log.h"

#define ENABLE_FRAMEBUFFER_CACHING 1

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
//	oFragColor = texture( uTex0, vTexCoord.st ) * vColor;

	vec4 col = texture( uTex0, vTexCoord.st ) * vColor;
	col.rgb *= col.a;
	oFragColor = col;
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
	CI_LOG_V( "created gl::Fbo of size: " << format.mSize );
}

ci::ivec2  FrameBuffer::getSize() const
{
	return mFbo->getSize();
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
	auto format = FrameBuffer::Format().size( size );
#if ENABLE_FRAMEBUFFER_CACHING
	auto it = mFrameBufferCache.find( format );
	if( it != mFrameBufferCache.end() ) {
		return it->second;
	}
	else {
		auto result = make_shared<FrameBuffer>( format );
		mFrameBufferCache.insert( make_pair( format, result ) );
		return result;
	}
#else
	return make_shared<FrameBuffer>( format );
#endif
}

void Renderer::draw( const FrameBufferRef &frameBuffer, const Rectf &destRect )
{
	if( ! mGlslFrameBuffer ) {
		try {
			mGlslFrameBuffer = gl::GlslProg::create( FRAMEBUFFER_VERT, FRAMEBUFFER_FRAG );
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

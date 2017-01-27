/*
Copyright (c) 2016, Richard Eakin - All rights reserved.

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

#include "ui/Filter.h"
#include "cinder/CinderAssert.h"

#include "cinder/gl/scoped.h"
#include "cinder/gl/draw.h"
#include "cinder/gl/wrapper.h"
#include "cinder/gl/GlslProg.h"

using namespace ci;
using namespace std;

namespace {

const string PASSTHROUGH_VERT = R"(
#version 410

uniform mat4 ciModelViewProjection;

in vec4 ciPosition;
in vec2 ciTexCoord0;

out vec2 vTexCoord0;

void main()
{	
	vTexCoord0 = ciTexCoord0;
	gl_Position = ciModelViewProjection * ciPosition;
}
)";

const string BLUR_FRAG = R"(
#version 410

uniform sampler2D	uTex0;
uniform vec2		uSampleOffset;

in vec2 vTexCoord0;

out vec4 oFragColor;

void main()
{ 
	vec4 sum = vec4( 0.0 );	
	sum += texture( uTex0, vTexCoord0 + -10.0 * uSampleOffset ) * 0.009167927656011385;
	sum += texture( uTex0, vTexCoord0 +  -9.0 * uSampleOffset ) * 0.014053461291849008;
	sum += texture( uTex0, vTexCoord0 +  -8.0 * uSampleOffset ) * 0.020595286319257878;
	sum += texture( uTex0, vTexCoord0 +  -7.0 * uSampleOffset ) * 0.028855245532226279;
	sum += texture( uTex0, vTexCoord0 +  -6.0 * uSampleOffset ) * 0.038650411513543079;
	sum += texture( uTex0, vTexCoord0 +  -5.0 * uSampleOffset ) * 0.049494378859311142;
	sum += texture( uTex0, vTexCoord0 +  -4.0 * uSampleOffset ) * 0.060594058578763078;
	sum += texture( uTex0, vTexCoord0 +  -3.0 * uSampleOffset ) * 0.070921288047096992;
	sum += texture( uTex0, vTexCoord0 +  -2.0 * uSampleOffset ) * 0.079358891804948081;
	sum += texture( uTex0, vTexCoord0 +  -1.0 * uSampleOffset ) * 0.084895951965930902;
	sum += texture( uTex0, vTexCoord0 +   0.0 * uSampleOffset ) * 0.086826196862124602;
	sum += texture( uTex0, vTexCoord0 +   1.0 * uSampleOffset ) * 0.084895951965930902;
	sum += texture( uTex0, vTexCoord0 +   2.0 * uSampleOffset ) * 0.079358891804948081;
	sum += texture( uTex0, vTexCoord0 +   3.0 * uSampleOffset ) * 0.070921288047096992;
	sum += texture( uTex0, vTexCoord0 +   4.0 * uSampleOffset ) * 0.060594058578763078;
	sum += texture( uTex0, vTexCoord0 +   5.0 * uSampleOffset ) * 0.049494378859311142;
	sum += texture( uTex0, vTexCoord0 +   6.0 * uSampleOffset ) * 0.038650411513543079;
	sum += texture( uTex0, vTexCoord0 +   7.0 * uSampleOffset ) * 0.028855245532226279;
	sum += texture( uTex0, vTexCoord0 +   8.0 * uSampleOffset ) * 0.020595286319257878;
	sum += texture( uTex0, vTexCoord0 +   9.0 * uSampleOffset ) * 0.014053461291849008;
	sum += texture( uTex0, vTexCoord0 +  10.0 * uSampleOffset ) * 0.009167927656011385;

	oFragColor = sum;
}
)";

const string DROP_SHADOW_FRAG = R"(
#version 410

uniform sampler2D	uTex0;
uniform vec2		uSampleOffset;
uniform vec2		uShadowOffset;

in vec2 vTexCoord0;

out vec4 oFragColor;

void main()
{ 
	vec2 offset = uSampleOffset;
	vec2 dropShadowOffset = uShadowOffset;

	vec4 sum = vec4( 0.0 );	
	sum += texture( uTex0, vTexCoord0 + -10.0 * offset + dropShadowOffset ) * 0.009167927656011385;
	sum += texture( uTex0, vTexCoord0 +  -9.0 * offset + dropShadowOffset ) * 0.014053461291849008;
	sum += texture( uTex0, vTexCoord0 +  -8.0 * offset + dropShadowOffset ) * 0.020595286319257878;
	sum += texture( uTex0, vTexCoord0 +  -7.0 * offset + dropShadowOffset ) * 0.028855245532226279;
	sum += texture( uTex0, vTexCoord0 +  -6.0 * offset + dropShadowOffset ) * 0.038650411513543079;
	sum += texture( uTex0, vTexCoord0 +  -5.0 * offset + dropShadowOffset ) * 0.049494378859311142;
	sum += texture( uTex0, vTexCoord0 +  -4.0 * offset + dropShadowOffset ) * 0.060594058578763078;
	sum += texture( uTex0, vTexCoord0 +  -3.0 * offset + dropShadowOffset ) * 0.070921288047096992;
	sum += texture( uTex0, vTexCoord0 +  -2.0 * offset + dropShadowOffset ) * 0.079358891804948081;
	sum += texture( uTex0, vTexCoord0 +  -1.0 * offset + dropShadowOffset ) * 0.084895951965930902;
	sum += texture( uTex0, vTexCoord0 +   0.0 * offset + dropShadowOffset ) * 0.086826196862124602;
	sum += texture( uTex0, vTexCoord0 +   1.0 * offset + dropShadowOffset ) * 0.084895951965930902;
	sum += texture( uTex0, vTexCoord0 +   2.0 * offset + dropShadowOffset ) * 0.079358891804948081;
	sum += texture( uTex0, vTexCoord0 +   3.0 * offset + dropShadowOffset ) * 0.070921288047096992;
	sum += texture( uTex0, vTexCoord0 +   4.0 * offset + dropShadowOffset ) * 0.060594058578763078;
	sum += texture( uTex0, vTexCoord0 +   5.0 * offset + dropShadowOffset ) * 0.049494378859311142;
	sum += texture( uTex0, vTexCoord0 +   6.0 * offset + dropShadowOffset ) * 0.038650411513543079;
	sum += texture( uTex0, vTexCoord0 +   7.0 * offset + dropShadowOffset ) * 0.028855245532226279;
	sum += texture( uTex0, vTexCoord0 +   8.0 * offset + dropShadowOffset ) * 0.020595286319257878;
	sum += texture( uTex0, vTexCoord0 +   9.0 * offset + dropShadowOffset ) * 0.014053461291849008;
	sum += texture( uTex0, vTexCoord0 +  10.0 * offset + dropShadowOffset ) * 0.009167927656011385;

	// Convert to grayscale using NTSC conversion weights
    //float gray = dot( sum.rgb, vec3( 0.299, 0.587, 0.114 ) );

	float gray = 0; // black shadow
	//sum.a *= 1.1;

	oFragColor = vec4( gray, gray, gray, sum.a );	
}
)";

} // anonymous namespace

namespace ui {

// ----------------------------------------------------------------------------------------------------
// Filter::PassInfo
// ----------------------------------------------------------------------------------------------------

void Filter::PassInfo::setCount( size_t count )
{ 
	mCount = count;
	mSizes.resize( count );
}

void Filter::PassInfo::setSize( const ci::ivec2 &size, size_t passIndex )
{
	CI_ASSERT( passIndex <= mSizes.size() );

	mSizes[passIndex] = size;
}

// ----------------------------------------------------------------------------------------------------
// Filter::Pass
// ----------------------------------------------------------------------------------------------------

Filter::Pass::~Pass()
{
#if ! UI_FRAMEBUFFER_CACHING_ENABLED
	// temporary: marking FrameBuffer as unused once Pass is destroyed because it is the sole owner
	// TODO: remove this once caching is fixed
	if( mFrameBuffer )
		mFrameBuffer->setInUse( false );
#endif
}

// ----------------------------------------------------------------------------------------------------
// Filter
// ----------------------------------------------------------------------------------------------------

Filter::~Filter()
{
}

void Filter::configure( const ivec2 &size, PassInfo *info )
{
	info->mSizes.resize( 1, size );
}

ci::gl::TextureRef Filter::getRenderColorTexture() const
{
	return mRenderFrameBuffer->getColorTexture();
}

gl::TextureRef Filter::getPassColorTexture( size_t passIndex ) const
{
	if( passIndex >= mPasses.size() )
		return nullptr;

	return mPasses[passIndex].getColorTexture();
}

// ----------------------------------------------------------------------------------------------------
// FilterBlur
// ----------------------------------------------------------------------------------------------------

FilterBlur::FilterBlur()
{
}

void FilterBlur::configure( const ci::ivec2 &size, ui::Filter::PassInfo *info )
{
	// TODO: rethink how these should be specified
	info->setCount( 2 );
	info->setSize( size, 0 );
	info->setSize( size, 1 );
}

void FilterBlur::process( ui::Renderer *ren, const ui::Filter::Pass &pass )
{
	if( ! mGlsl )
		mGlsl = ci::gl::GlslProg::create( PASSTHROUGH_VERT, BLUR_FRAG );

	vec2 sampleOffset;
	gl::TextureRef tex;

	if( pass.getIndex() == 0 ) {
		sampleOffset.x = mBlurPixels.x / (float)pass.getWidth();
		tex = getRenderColorTexture();
	}
	else {
		sampleOffset.y = mBlurPixels.y / (float)pass.getHeight();
		tex = getPassColorTexture( 0 );
	}

	gl::ScopedGlslProg glslScope( mGlsl );
	mGlsl->uniform( "uSampleOffset", sampleOffset );

	gl::ScopedTextureBind texScope( tex );
	gl::clear( ColorA::zero() );

	vec2 ratio = vec2( pass.getSize() ) / vec2( tex->getSize() );
	vec2 lr = { ratio.x, 1 - ratio.y };
	gl::drawSolidRect( Rectf( vec2( 0 ), pass.getSize() ), vec2( 0, 1 ), lr );
}

// ----------------------------------------------------------------------------------------------------
// FilterDropShadow
// ----------------------------------------------------------------------------------------------------

FilterDropShadow::FilterDropShadow()
{
	mBlurPixels = vec2( 2.0f );
}

void FilterDropShadow::configure( const ci::ivec2 &size, ui::Filter::PassInfo *info )
{
	ivec2 framebufferSize( vec2( size ) / (float)mDownsampleFactor );
	info->setCount( 2 );
	info->setSize( framebufferSize, 0 );
	info->setSize( framebufferSize, 1 );
}

void FilterDropShadow::setDownsampleFactor( float factor )
{
	mDownsampleFactor = glm::max( 1.0f, factor );
	// TODO: need to mark the filter as needing to be re-configured
}

void FilterDropShadow::process( ui::Renderer *ren, const ui::Filter::Pass &pass )
{
	if( ! mGlsl )
		mGlsl = ci::gl::GlslProg::create( PASSTHROUGH_VERT, DROP_SHADOW_FRAG );

	vec2 sampleOffset;
	gl::TextureRef tex;

	if( pass.getIndex() == 0 ) {
		sampleOffset.x = mBlurPixels.x / (float)pass.getWidth();
		tex = getRenderColorTexture();
	}
	else {
		sampleOffset.y = mBlurPixels.y / (float)pass.getHeight();
		tex = getPassColorTexture( 0 );
	}

	vec2 shadowOffset = mShadowOffset / vec2( pass.getSize() );

	{
		gl::ScopedGlslProg glslScope( mGlsl );
		mGlsl->uniform( "uSampleOffset", sampleOffset );
		mGlsl->uniform( "uShadowOffset", shadowOffset );

		gl::ScopedTextureBind texScope( tex );
		gl::clear( ColorA::zero() );
		//gl::drawSolidRect( Rectf( vec2( 0 ), pass.getSize() ) );

		vec2 ratio = vec2( pass.getSize() ) / vec2( tex->getSize() );
		vec2 lr = { ratio.x, 1 - ratio.y };
		gl::drawSolidRect( Rectf( vec2( 0 ), pass.getSize() ), vec2( 0, 1 ), lr );
	}

	if( pass.getIndex() == 1 ) {
		// draw original image again on top
		auto sourceArea = Area( ivec2( 0 ), pass.getSize() );
		auto destRect = Rectf( vec2( 0 ), pass.getSize() );

		gl::draw( getRenderColorTexture(), sourceArea, destRect );
	}
}

} // namespace ui
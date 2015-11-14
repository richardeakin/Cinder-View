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

#include "view/Interface3d.h"

#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/app/App.h"

using namespace ci;
using namespace std;

namespace view {

const float Z_PLACEMENT = -2.0f;
const float RAY_PADDING = 0.27f;

CoordinateAxisView::CoordinateAxisView( const ci::Rectf &bounds )
	: View( bounds )
{
}

void CoordinateAxisView::layout()
{
	vec2 size = getSize();
	if( size.x < 1.0f || size.y < 1.0f )
		return;

	mCoordFrameCam.setPerspective( 40, size.x / size.y, 0.05f, 100.0f );
	mCoordFrameCam.lookAt( vec3( 0 ), vec3( 0, 0, Z_PLACEMENT ) );

	if( size.x > size.y ) {
		vec3 topPos = mCoordFrameCam.generateRay( 0, RAY_PADDING, mCoordFrameCam.getAspectRatio() ).calcPosition( Z_PLACEMENT );
		vec3 bottomPos = mCoordFrameCam.generateRay( 0, 1 - RAY_PADDING, mCoordFrameCam.getAspectRatio() ).calcPosition( Z_PLACEMENT );
		mArrowLength = topPos.y - bottomPos.y;
	}
	else {
		vec3 rightPos = mCoordFrameCam.generateRay( 1 - RAY_PADDING, 0, mCoordFrameCam.getAspectRatio() ).calcPosition( Z_PLACEMENT );
		vec3 leftPos = mCoordFrameCam.generateRay( RAY_PADDING, 0, mCoordFrameCam.getAspectRatio() ).calcPosition( Z_PLACEMENT );
		mArrowLength = leftPos.x - rightPos.x;
	}
}

void CoordinateAxisView::draw()
{
	Rectf worldBounds = getWorldBounds();
	ivec2 pos = worldBounds.getLowerLeft();
	float windowHeight = app::getWindowHeight();
	pos.y = app::getWindowHeight() - pos.y; // flip y relative to window's bottom left
	ivec2 size = getSize();
	if( size.x == 0 || size.y == 0 )
		return;

	gl::ScopedViewport scopedViewport( pos, size );

	gl::ScopedGlslProg glslScope( gl::getStockShader( gl::ShaderDef().lambert().color() ) );
	gl::ScopedDepth depthScope( true );
	gl::ScopedMatrices matricesScope;

	gl::setMatrices( mCoordFrameCam );
	gl::translate( vec3( 0, 0, Z_PLACEMENT ) );
	gl::multModelMatrix( mat4( inverse( mOrientation ) ) );

	float headRadius = mArrowLength * 0.114f;
	float headLength = mArrowLength * 0.13f;
	gl::drawCoordinateFrame( mArrowLength, headLength, headRadius );
}

} // namespace view

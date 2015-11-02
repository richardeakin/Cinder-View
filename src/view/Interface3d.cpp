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
#include "cinder/app/App.h"

using namespace ci;
using namespace std;

namespace view {

CoordinateAxisView::CoordinateAxisView( const ci::Rectf &bounds )
	: View( bounds )
{
}

void CoordinateAxisView::layout()
{
	ivec2 viewportSize = getSize();
	if( viewportSize.x == 0 || viewportSize.y == 0 )
		return;

	mCoordFrameCam.setPerspective( 45, viewportSize.x / viewportSize.y, 0.1f, 100.0f );
	mCoordFrameCam.lookAt( vec3( 0, 0, 2.75f ), vec3( 0 ) );
}

void CoordinateAxisView::draw()
{
	Rectf worldBounds = getWorldBounds();
	ivec2 pos = worldBounds.getLowerLeft();
	float windowHeight = app::getWindowHeight();
	pos.y = app::getWindowHeight() - pos.y; // flip y relative to window's bottom left
	ivec2 size = getSize();

	gl::ScopedViewport scopedViewport( pos, size );

	gl::ScopedGlslProg glslScope( gl::getStockShader( gl::ShaderDef().lambert().color() ) );
	gl::ScopedDepth depthScope( true );
	gl::ScopedMatrices matricesScope;

	gl::setMatrices( mCoordFrameCam );
	gl::translate( vec3( -1, 0, 0 ) );
	gl::multModelMatrix( mat4( inverse( mOrientation ) ) );

	gl::drawCoordinateFrame( 1, 0.25f, 0.09f );
}

} // namespace view

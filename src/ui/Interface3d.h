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

#pragma once

#include "ui/View.h"

#include "cinder/Camera.h"

namespace ui {

typedef std::shared_ptr<class CoordinateAxisView>		CoordinateAxisViewRef;

class Interface3dBaseView : public View {
  public:
	Interface3dBaseView( const ci::Rectf &bounds = ci::Rectf::zero() )
		: View( bounds )
	{}

  protected:
	void draw( Renderer *ren ) override;
	void layout() override;

	//! Subclasses implement this to draw their 3d content
	virtual void draw3d() = 0;

	ci::CameraPersp mCam;
	ci::vec3		mPlacement = ci::vec3( 0, 0, -2 );
};

//! Draws a 3d coordinate axis of a Camera's Orientation
class CI_UI_API CoordinateAxisView : public Interface3dBaseView {
  public:
	CoordinateAxisView( const ci::Rectf &bounds = ci::Rectf::zero() );

	void	setOrientation( const ci::quat &orientation )	{ mOrientation = orientation; }

protected:
	void layout()	override;
	void draw3d()	override;

private:
	ci::quat		mOrientation;
	float			mArrowLength = 0;
};

} // namespace ui

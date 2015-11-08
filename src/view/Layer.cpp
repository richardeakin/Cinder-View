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

#include "view/Layer.h"
#include "view/View.h"

#include "cinder/Log.h"

#include "cinder/gl/gl.h"

using namespace ci;
using namespace std;

namespace view {

FrameBuffer::FrameBuffer( const vec2 &size )
{
	auto format = gl::Fbo::Format();
	mFbo = gl::Fbo::create( (int)size.x, (int)size.y, format );
}

Layer::Layer( View *view )
	: mView( view )
{
}

Layer::~Layer()
{
}

float Layer::getAlpha() const
{
	return mView->getAlpha();
}

void Layer::update()
{
	if( getAlpha() < 0.9999f ) {
		// TODO: pull FrameBuffer from a cache
		if( ! mFrameBuffer ) {
			CI_LOG_I( "creating FrameBuffer for view '" << mView->getName() << "', size: " << mView->getSize() );
			mFrameBuffer = make_shared<FrameBuffer>( mView->getSize() );
		}
	}
	else if( mFrameBuffer ) {
		CI_LOG_I( "removing FrameBuffer for view '" << mView->getName() << "'" );
		mFrameBuffer.reset();
	}

}

void Layer::draw()
{
	if( mView->isHidden() )
		return;

	if( mFrameBuffer ) {
		gl::context()->pushFramebuffer( mFrameBuffer->mFbo );
		gl::setMatricesWindow( mView->getWidth(), mView->getHeight() );
		gl::clear();
	}

	mView->beginClip();

	gl::ScopedModelMatrix modelScope1;
	gl::translate( mView->getPos() );

	mView->drawImpl();

	for( auto &view : mView->getSubviews() )
		view->getLayer()->draw();

	mView->endClip();

	if( mFrameBuffer ) {
		gl::context()->popFramebuffer();
	}
}

} // namespace view

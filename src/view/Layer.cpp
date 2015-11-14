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

#define ENABLE_FRAMEBUFFER_CACHING 1

using namespace ci;
using namespace std;

namespace view {

FrameBuffer::FrameBuffer( const Format &format )
{
	auto fboFormat = gl::Fbo::Format();
	fboFormat.colorTexture(
		gl::Texture2d::Format()
 			.minFilter( GL_LINEAR ).magFilter( GL_LINEAR )
	);

	mFbo = gl::Fbo::create( format.mSize.x, format.mSize.y, fboFormat );
	CI_LOG_I( "created gl::Fbo of size: " << format.mSize );
}

bool FrameBuffer::Format::operator==(const Format &other) const
{
	return mSize == other.mSize;
}

// static
FrameBufferCache* FrameBufferCache::instance()
{
	static FrameBufferCache sInstance;
	return &sInstance;
}

// static
FrameBufferRef FrameBufferCache::getFrameBuffer( const ci::ivec2 &size )
{
	auto format = FrameBuffer::Format().size( size );
#if ENABLE_FRAMEBUFFER_CACHING
	auto it = instance()->mCache.find( format );
	if( it != instance()->mCache.end() ) {
		return it->second;
	}
	else {
		auto result = make_shared<FrameBuffer>( format );
		instance()->mCache.insert( make_pair( format, result ) );
		return result;
	}
#else
	return make_shared<FrameBuffer>( format );
#endif
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
	if( getAlpha() < 0.9999f && mRenderTransparencyToFrameBuffer ) {
		if( ! mFrameBuffer ) {
			CI_LOG_I( "aquiring FrameBuffer for view '" << mView->getName() << "', size: " << mView->getSize() );
			mFrameBuffer = FrameBufferCache::getFrameBuffer( ivec2( mView->getSize() ) );
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
		gl::pushViewport( mView->getSize() );
		gl::pushMatrices();
		gl::setMatricesWindow( mView->getSize() );

		gl::clear();
	}
	else {
		gl::pushModelMatrix();
		gl::translate( mView->getPos() );
	}

	mView->beginClip();

	mView->drawImpl();

	for( auto &view : mView->getSubviews() )
		view->getLayer()->draw();

	mView->endClip();

	if( mFrameBuffer ) {
		gl::context()->popFramebuffer();
		gl::popViewport();
		gl::popMatrices();

		gl::ScopedColor colorScope( ColorA::gray( getAlpha() ) );

		auto destRect = Rectf( 0, 0, mFrameBuffer->mFbo->getWidth(), mFrameBuffer->mFbo->getHeight() ) + mView->getPos();
		gl::draw( mFrameBuffer->mFbo->getColorTexture(), destRect );
	}
	else {
		gl::popModelMatrix();
	}
}

} // namespace view

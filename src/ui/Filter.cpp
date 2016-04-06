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

using namespace ci;

namespace ui {

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

Filter::Pass::~Pass()
{
	// temporary: marking FrameBuffer as unused once Pass is destroyed because it is the sole owner
	// TODO: remove this once caching is fixed
	if( mFrameBuffer )
		mFrameBuffer->mInUse = false;
}

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

} // namespace ui
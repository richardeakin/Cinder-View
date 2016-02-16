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

#pragma once

#include "ui/Renderer.h"

#include <memory>

namespace ui {

typedef std::shared_ptr<class Filter>   FilterRef;

//! Performs filter effects as post-process operation.
class Filter {
  public:

  protected:
	//! Passed during initialize() to configure render targets
	struct PassInfo {
	  public:
		void	setCount( size_t count );
		size_t	getCount() const			{ return mCount; }

		void		setSize( const ci::ivec2 &size, size_t passIndex = 0 );
		ci::ivec2	getSize( size_t passIndex = 0 ) const		{ return mSizes.at( passIndex ); }

	  private:
		size_t					mCount = 1;
		std::vector<ci::ivec2>	mSizes;

		friend class Filter;
	};

	//! Provided during process() that contains information about the current processing pass
	class Pass {
	  public:
		//! Returns the index of the current pass
		size_t	getIndex() const	{ return mIndex; }

		ci::ivec2			getSize() const				{ return mSize; }
		int					getWidth() const			{ return mSize.x; }
		int					getHeight() const			{ return mSize.y; }
		ci::gl::TextureRef	getColorTexture() const		{ return mFrameBuffer->getColorTexture(); }

	  private:
		void setIndex( size_t index )	{ mIndex = index; }

		size_t			mIndex = 0;
		ci::ivec2		mSize;
		FrameBufferRef	mFrameBuffer;

		friend class Layer;
	};

	//! Called when Passes need to be configured. Defaults to one pass with a render target the size of \a size.
	virtual void configure( const ci::ivec2 &size, PassInfo *info );
	//! Called when the Filter should perform processing. The requested FrameBuffer will already be bound.
	virtual void process( Renderer *ren, const Pass &pass ) = 0;


	ci::gl::TextureRef getRenderColorTexture() const;
	ci::gl::TextureRef getPassColorTexture( size_t passIndex ) const;

  private:
	std::vector<Pass>	mPasses;
	FrameBufferRef		mRenderFrameBuffer;

	friend class Layer;
};

} // namespace ui

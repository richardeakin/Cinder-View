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

#include "ui/Renderer.h"

#include "cinder/gl/Fbo.h"

#include <unordered_map>

namespace ui {

class View;

typedef std::shared_ptr<class FrameBuffer>	FrameBufferRef;
typedef std::shared_ptr<class Layer>		LayerRef;

class FrameBuffer {
  public:
	struct Format {
		Format& size( const ci::ivec2 &size )	{ mSize = size; return *this; }

		//! Overloaded to allow Format to be used as a key in std::unordered_map
		bool operator==(const Format &other) const;

		ci::ivec2 mSize;
	};

	FrameBuffer( const Format &format );

	cinder::gl::FboRef mFbo;
};

} // namespace ui

namespace std {

template <>
struct hash<ui::FrameBuffer::Format> {
	inline size_t operator()( const ui::FrameBuffer::Format &format ) const
	{
		return hash<int>()( format.mSize.x ) ^ hash<int>()( format.mSize.y );
	}
};

} // namespace std

namespace ui {

class FrameBufferCache {
  public:
	static FrameBufferCache* instance();

	static FrameBufferRef getFrameBuffer( const ci::ivec2 &size );

  private:
	std::unordered_map<FrameBuffer::Format, FrameBufferRef>	mCache;
};

class Layer {
  public:
	Layer( View *view );
	virtual ~Layer();

	float	getAlpha() const;

	void update();

	void draw();

	void setRenderTransparencyToFrameBufferEnabled( bool enable )	{ mRenderTransparencyToFrameBuffer = enable; }
	bool isRenderTransparencyToFrameBufferEnabled() const			{ return mRenderTransparencyToFrameBuffer; }

  private:
	View*			mView;
	RendererRef		mRenderer;
	FrameBufferRef	mFrameBuffer;
	bool			mRenderTransparencyToFrameBuffer = true;
};

} // namespace ui
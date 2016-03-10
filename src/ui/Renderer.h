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

#include "cinder/Cinder.h"
#include "cinder/Color.h"
#include "cinder/Rect.h"

#include "ui/Image.h"

namespace cinder {

typedef std::shared_ptr<class ImageSource>		ImageSourceRef;

namespace gl {

typedef std::shared_ptr<class Batch>		BatchRef;
typedef std::shared_ptr<class Fbo>          FboRef;
typedef std::shared_ptr<class GlslProg>     GlslProgRef;

} } // namespace cinder::gl

namespace ui {

typedef std::shared_ptr<class Renderer> RendererRef;
typedef std::shared_ptr<class FrameBuffer> FrameBufferRef;

enum class BlendMode {
	ALPHA,
	PREMULT_ALPHA
};

class FrameBuffer {
  public:
	struct Format {
		Format &size( const ci::ivec2 &size )
		{
			mSize = size;
			return *this;
		}

		//! Allow Format to be used as a key in std::unordered_map
		bool operator==( const Format &other ) const;

		ci::ivec2 mSize;
	};

	FrameBuffer( const Format &format );

	ci::ivec2   getSize() const;
	int         getWidth() const { return getSize().x; }
	int         getHeight() const { return getSize().y; }
	bool        isBound() const { return mIsBound; }
	bool		isUsable() const;

	ci::ImageSourceRef  createImageSource() const;

  private:

	cinder::gl::FboRef  mFbo;
	bool                mIsBound = false;
	bool				mDiscarded = false;

	friend class Renderer;
};

} // namespace ui

/*
namespace std {

template <>
struct hash<ui::FrameBuffer::Format> {
	inline size_t operator()( const ui::FrameBuffer::Format &format ) const
	{
		return hash<int>()( format.mSize.x ) ^ hash<int>()( format.mSize.y );
	}
};

} // namespace std

*/

namespace ui {

class Renderer {
  public:
	Renderer();

	//! Sets the current color used for rendering
	void setColor( const ci::ColorA &color );
	//! Stores the current color.
	void pushColor();
	//! Makes \a color the current color used when rendering, first storing the current color.
	void pushColor( const ci::ColorA &color );
	//! Restores the color to what was previously set before the last pushColor().
	void popColor();
	//!
	void setBlendMode( BlendMode mode );
	//!
	void pushBlendMode( BlendMode mode );
	//!
	void popBlendMode();
	//!
	FrameBufferRef getFrameBuffer( const ci::ivec2 &size );
	//!
	size_t getNumFrameBuffersCached() const     { return mFrameBufferCache.size(); }
	//!
	void pushFrameBuffer( const FrameBufferRef &frameBuffer );
	//!
	void popFrameBuffer( const FrameBufferRef &frameBuffer );
	//!
	void draw( const FrameBufferRef &frameBuffer, const ci::Rectf &destRect );
	//!
	void draw( const FrameBufferRef &frameBuffer, const ci::Area &sourceArea, const ci::Rectf &destRect );
	//!
	void draw( const ImageRef &image, const ci::Rectf &destRect, const ci::gl::GlslProgRef &glsl = nullptr );

	//! Draws a solid rectangle with dimensions \a rect.
	void drawSolidRect( const ci::Rectf &rect );
	//! Draws a stroked rectangle with dimensions \a rect.
	void drawStrokedRect( const ci::Rectf &rect );
	//! Draws a stroked rectangle centered around \a rect, with a line width of \a lineWidth
	void drawStrokedRect( const ci::Rectf &rect, float lineWidth );

  private:
	std::vector<ci::ColorA>		mColorStack;
	std::vector<BlendMode>		mBlendModeStack;

	std::vector<FrameBufferRef>	mFrameBufferCache;

	ci::gl::GlslProgRef         mGlslFrameBuffer;
	ci::gl::BatchRef			mBatchSolidRect;
};

} // namespace ui

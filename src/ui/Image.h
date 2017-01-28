/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "ui/Export.h"
#include "cinder/Cinder.h"
#include "cinder/ImageIo.h"

#include <memory>

namespace cinder { namespace gl {

//typedef std::shared_ptr<class Texture>  TextureRef; // TODO: fix this forward decl. in Texture.h
typedef std::shared_ptr<class Texture2d>		Texture2dRef;
typedef Texture2dRef							TextureRef;

} } // namespace cinder::gl

namespace ui {

typedef std::shared_ptr<class Image>	ImageRef;

class CI_UI_API Image {
  public:
	Image( const ci::ImageSourceRef &imageSource );
	//! \note this is public although in the long run, we will want a way to load textures without being tied to gl, so this will likely change.
	Image( const ci::gl::TextureRef &texture );

	const ci::ivec2&    getSize() const     { return mSize; }
	ci::Area            getBounds() const   { return ci::Area( 0, 0, mSize.x, mSize.y ); }

	const ci::gl::TextureRef&	getTexture() const	{ return mTexture; }

  private:
	ci::gl::TextureRef	mTexture;
	ci::ivec2           mSize;

	friend class Renderer;
};

} // namespace ui
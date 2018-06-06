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
#include "cinder/Vector.h"
#include "cinder/Rect.h"

#include <atomic>
#include <mutex>
#include <vector>

namespace cinder {

class Font;

namespace gl {
	typedef std::shared_ptr<class TextureFont>	TextureFontRef;
} // namespace cinder::gl

} // namespace cinder

namespace ui {

enum class FontFace {
	NORMAL,
	BOLD
};

enum class TextAlignment {
	LEFT,
	CENTER,
	RIGHT
};

enum class TextBaselineAdjust {
	NONE,
	CENTER
};

typedef std::shared_ptr<class Text>	TextRef;

class CI_UI_API Text {
public:

	float		getSize() const;
	FontFace	getFace() const;
	float		getAscent() const;
	float		getDescent() const;

	ci::vec2	measureString( const std::string &str ) const;
	ci::vec2	measureStringWrapped( const std::string &str, const ci::Rectf &fitRect ) const;
	void		drawString( const std::string &str, const ci::vec2 &baseline );
	void		drawStringWrapped( const std::string &str, const ci::Rectf &fitRect );

private:
	Text();
	Text( const ci::Font &font, FontFace face );

	ci::gl::TextureFontRef	mTextureFont;
	FontFace				mFace;
	std::atomic<bool>		mIsReady;

	friend class TextManager;
};

class CI_UI_API TextManager {
public:
	//! If size < 0, a default size will be picked (this is temporary until some sort of styling is introduced)
	static TextRef loadText( FontFace fontFace, float size = -1 );

private:
	TextManager()	{}
	
	TextManager( const TextManager& )				= delete;
	TextManager& operator=( const TextManager& )	= delete;

	static TextManager* instance();

	TextRef loadTextImpl( FontFace fontFace, float size );
	TextRef loadTextImplAsync( FontFace fontFace, float size );

	std::string	getFontName( FontFace face ) const;

	std::vector<TextRef>	mTextCache;
	std::mutex				mMutex;
};

} // namespace ui

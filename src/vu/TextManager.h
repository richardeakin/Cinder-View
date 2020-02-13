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

#include "vu/Export.h"
#include "cinder/Vector.h"
#include "cinder/Rect.h"
#include "cinder/Filesystem.h"

#include <atomic>
#include <mutex>
#include <vector>

namespace cinder {

class Font;

namespace gl {
	typedef std::shared_ptr<class TextureFont>	TextureFontRef;
} // namespace cinder::gl

} // namespace cinder

namespace vu {

enum class TextAlignment {
	LEFT,
	CENTER,
	RIGHT,
	JUSTIFIED
};

enum class TextBaselineAdjust {
	NONE,
	CENTER
};

typedef std::shared_ptr<class Text>	TextRef;

class CI_UI_API Text {
public:

	const ci::fs::path&	getFilePath() const		{ return mFilePath; }
	const std::string&	getSystemName() const	{ return mSystemName; }
	bool isFileFont() const						{ return ! mFilePath.empty(); }
	bool isSystemFont() const					{ return ! mSystemName.empty(); }

	float		getSize() const;
	float		getAscent() const;
	float		getDescent() const;

	ci::vec2	measureString( const std::string &str ) const;
	ci::vec2	measureStringWrapped( const std::string &str, const ci::Rectf &fitRect ) const;
	void		drawString( const std::string &str, const ci::vec2 &baseline );
	void		drawStringWrapped( const std::string &str, const ci::Rectf &fitRect );

private:
	Text();
	Text( const ci::Font &font, float fontSize );

	ci::gl::TextureFontRef	mTextureFont;
	std::string				mSystemName;
	ci::fs::path			mFilePath;
	float					mFontSize; //! note: this might be different to the ci::Font size, due to content scaling
	std::atomic<bool>		mIsReady;

	friend class TextManager;
};

class CI_UI_API TextManager {
public:
	static TextManager* instance();

	//! If size < 0, a default size will be picked (this is temporary until some sort of styling is introduced)
	static TextRef loadText( std::string systemName = "", float size = -1 );
	static TextRef loadTextFromFile( const ci::fs::path &filePath, float size = -1 );

	//! Note: call this before loading any text objects
	void setSupportedChars( const std::string &str )	{ mSupportedChars = str; }

	const std::string&	getSupportedChars() const		{ return mSupportedChars; }

private:
	TextManager();
	
	TextManager( const TextManager& )				= delete;
	TextManager& operator=( const TextManager& )	= delete;

	TextRef loadTextImpl( const std::string &systemName, float size );
	TextRef loadTextFromFileImpl( const ci::fs::path &filePath, float size );

	std::vector<TextRef>	mTextCache;
	std::string				mSupportedChars;
};

} // namespace vu

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

#include "ui/TextManager.h"

#include "cinder/Cinder.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/CinderAssert.h"
#include "cinder/Log.h"
#include "cinder/app/App.h"

using namespace ci;
using namespace std;

namespace ui {

// ----------------------------------------------------------------------------------------------------
// TextManager
// ----------------------------------------------------------------------------------------------------

namespace {

float getDefaultSize()
{
#if defined( CINDER_MSW )
	return 16;
#else
	return 14;
#endif
}

} // anonymous namespace

// static
TextManager* TextManager::instance()
{
	static TextManager sInstance;
	return &sInstance;
}

// static
TextRef TextManager::loadText( std::string systemName, float size )
{
	if( size < 0 ) {
		size = getDefaultSize();
	}

	if( systemName.empty() ) {
		systemName = "Arial";
	}

	return instance()->loadTextImpl( systemName, size );
}

// static
TextRef TextManager::loadTextFromFile( const fs::path &filePath, float size )
{
	if( size < 0 ) {
		size = getDefaultSize();
	}

	return instance()->loadTextFromFileImpl( filePath, size );
}

TextRef TextManager::loadTextImpl( const std::string &systemName, float size )
{
	for( const auto &text : mTextCache ) {
		if( text->mSystemName == systemName && text->getSize() == size )
			return text;
	}

	auto font = Font( systemName, size );
	TextRef result = TextRef( new Text( font ) );
	result->mSystemName = systemName;

	mTextCache.push_back( result );

	return result;
}

TextRef TextManager::loadTextFromFileImpl( const fs::path &filePath, float size )
{
	for( const auto &text : mTextCache ) {
		if( text->mFilePath == filePath && text->getSize() == size )
			return text;
	}

	// account for content scale when using GDI
	size /= app::getWindow()->getContentScale(); // TODO: how to use Graph's app::Window for content size?

	auto font = Font( loadFile( filePath ), size );
	TextRef result = TextRef( new Text( font ) );
	result->mFilePath = filePath;

	mTextCache.push_back( result );

	return result;
}

// ----------------------------------------------------------------------------------------------------
// Text
// ----------------------------------------------------------------------------------------------------

Text::Text()
	: mIsReady( false )
{
}

Text::Text( const ci::Font &font )
	: mIsReady( false )
{
	auto format = gl::TextureFont::Format().premultiply( true );
	mTextureFont = gl::TextureFont::create( font, format );
	mIsReady = true;
}

float Text::getSize() const
{
	if( ! mIsReady )
		return 0;

	return mTextureFont->getFont().getSize();
}

float Text::getAscent() const
{
	if( ! mIsReady )
		return 0;

	return mTextureFont->getAscent();
}

float Text::getDescent() const
{
	if( ! mIsReady )
		return 0;

	return mTextureFont->getFont().getDescent();
}

vec2 Text::measureString( const std::string &str ) const
{
	if( ! mIsReady )
		return vec2( 0 );

	return mTextureFont->measureString( str );
}

vec2 Text::measureStringWrapped( const std::string &str, const ci::Rectf &fitRect ) const
{
	if( ! mIsReady )
		return vec2( 0 );

	return mTextureFont->measureStringWrapped( str, fitRect );
}

void Text::drawString( const string &str, const vec2 &baseline )
{
	if( ! mIsReady )
		return;

	mTextureFont->drawString( str, baseline );
}

void Text::drawStringWrapped( const std::string &str, const ci::Rectf &fitRect )
{
	if( ! mIsReady )
		return;

	mTextureFont->drawStringWrapped( str, fitRect );
}

} // namespace ui

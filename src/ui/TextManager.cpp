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

// static
TextManager* TextManager::instance()
{
	static TextManager sInstance;
	return &sInstance;
}

// static
TextRef TextManager::loadText( FontFace fontFace, float size )
{
	if( size < 0 ) {
#if defined( CINDER_MSW )
		size = 16;
#else
		size = 14;
#endif
	}

	if( app::isMainThread() )
		return instance()->loadTextImpl( fontFace, size );
	else
		return instance()->loadTextImplAsync( fontFace, size );
}

TextRef TextManager::loadTextImpl( FontFace face, float size )
{
	for( const auto &text : mTextCache ) {
		if( text->getFace() == face && text->getSize() == size )
			return text;
	}

	auto font = Font( getFontName( face ), size );

	TextRef result( new Text( font, face ) );

	lock_guard<mutex> lock( mMutex );
	mTextCache.push_back( result );

	return result;
}

TextRef TextManager::loadTextImplAsync( FontFace face, float size )
{
	for( const auto &text : mTextCache ) {
		if( text->getFace() == face && text->getSize() == size )
			return text;
	}

	shared_ptr<Text> result( new Text );
	{
		lock_guard<mutex> lock( mMutex );
		mTextCache.push_back( result );
	}

	CI_ASSERT_MSG( false, "TODO (read comment)" );
	// - move this to Script load process if possible, or force onto main thread with App
//	Dispatch::onMain( [this, result, face, size] {
		auto font = Font( getFontName( face ), size );
		result->mTextureFont = gl::TextureFont::create( font );
		result->mIsReady = true;
//	} );

	return result;
}

std::string TextManager::getFontName( FontFace face ) const
{
	return "Arial";
}

// ----------------------------------------------------------------------------------------------------
// Text
// ----------------------------------------------------------------------------------------------------

Text::Text()
	: mIsReady( false )
{
}

Text::Text( const ci::Font &font, FontFace face )
	: mFace( face ), mIsReady( false )
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

FontFace Text::getFace() const
{
	return mFace;
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

void Text::drawString( const string &str, const vec2 &baseline )
{
	if( ! mIsReady )
		return;

	mTextureFont->drawString( str, baseline );
}

} // namespace ui

/*
 Copyright (c) 2017, The Cinder Project, All rights reserved.

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

#include "ui/GestureTracker.h"

using namespace ci;
using namespace std;

namespace ui {

// ----------------------------------------------------------------------------------------------------
// SwipeTracker
// ----------------------------------------------------------------------------------------------------

void SwipeTracker::clear()
{
	mStoredTouches.clear();
}

void SwipeTracker::storeTouchPos( const ci::vec2 &pos, double currentTime )
{
	if( mStoredTouches.size() >= mMaxStoredTouches )
		mStoredTouches.pop_front();

	StoredTouch touch;
	touch.position = pos;
	touch.eventSeconds = currentTime;
	mStoredTouches.push_back( touch );

	if( mStoredTouches.size() == 1 )
		mFirstTouch = mStoredTouches.front();
}


vec2 SwipeTracker::calcSwipeVelocity()
{
	if( mStoredTouches.size() < 2 )
		return vec2( 0 );

	vec2 touchVelocity = vec2( 0 );
	int samples = 0;
	auto lastIt = --mStoredTouches.end();
	for( auto it = mStoredTouches.begin(); it != lastIt; ++it ) {
		auto nextIt = it;
		++nextIt;
		double dt = nextIt->eventSeconds - it->eventSeconds;
		if( dt > 0.001 ) {
			touchVelocity += ( nextIt->position - it->position ) / float( dt );
			samples += 1;
		}
	}

	if( samples > 0 ) {
		touchVelocity /= float( samples );
	}

	return touchVelocity;
}

vec2 SwipeTracker::calcSwipeDistance()
{
	return getLastTouchPos() - getFirstTouchPos();
}

vec2 SwipeTracker::getLastTouchPos() const
{
	if( mStoredTouches.empty() )
		return vec2( 0 );

	return mStoredTouches.back().position;
}

double SwipeTracker::getLastTouchTime() const
{
	if( mStoredTouches.empty() )
		return -1;

	return mStoredTouches.back().eventSeconds;
}

} // namespace ui

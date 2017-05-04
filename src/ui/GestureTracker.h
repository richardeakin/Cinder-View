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

// Notes:
// First off, these 'trackers' are not yet in any unified form. For now I'm just adding what useful in moving forward,
// and planning to rewrite this part when there is more gesture functionality to work with. Current thoughts:
// - Makes sense to pass in a Responder to the tracker (which is itself a View but we don't need all the View stuff).
// - need a way to insert these into touches stream within the graph. UIKit does this by keeping a list of UIGesterRecognizers per view.
// - if we want to be able to add these to arbitrary views, they should be callback (signal) based.

#pragma once

#include "ui/Export.h"
#include "cinder/app/TouchEvent.h"

#include <list>
#include <map>

namespace ui {

//! Swipe gesture tracker. Currently only one finger taps. TODO: multi-finger swipe support.
class CI_UI_API SwipeTracker {
  public:

	void clear();
	void storeTouchPos( const ci::vec2 &pos, double currentTime );
	ci::vec2 calcSwipeVelocity();
	ci::vec2 calcSwipeDistance();

	ci::vec2 getFirstTouchPos() const	{ return mFirstTouch.position; }
	//! Returns the positions of the last recorded touch, or vec2( 0 ) if none have yet been recorded.
	ci::vec2 getLastTouchPos() const;
	//! Returns the time of the last recorded touch, or -1 if none have yet been recorded.
	double getLastTouchTime() const;
	//! Returns the number of stored touches.
	size_t getNumStoredTouches() const	{ return mStoredTouches.size(); }

  private:
	struct StoredTouch {
		ci::vec2	position;
		double		eventSeconds;
	};

	std::list<StoredTouch>	mStoredTouches;
	StoredTouch				mFirstTouch;
	size_t					mMaxStoredTouches = 10;
};

//! Tap gesture tracker. Supports multiple consecutive taps. Currently only one finger taps. TODO: multi-finger tap support.
class CI_UI_API TapTracker {
  public:
	void processTouchesBegan( ci::app::TouchEvent &event, double currentTime );
	void processTouchesEnded( ci::app::TouchEvent &event, double currentTime );

	//void setNumTapsRequired( int count )	{ mNumTapsRequired = count; }
	//int	getNumTapsRequired() const	{ return mNumTapsRequired; }

	int	getCurrentTapCount() const	{ return mCurrentTapCount; }

  private:
	struct StoredTouch {
		//ci::vec2	position;
		double		eventSeconds;
	};

	std::map<uint32_t, StoredTouch>	mStoredTouches;

	//int		mNumTapsRequired = 2;
	int		mCurrentTapCount = 0;
	bool	mTouchIsDown = false;
	double	mMaxDurationBetweenTaps = 0.2f;
	double	mMaxDurationConsideredTap = 0.1f;
	double	mTimeLastTap = -1;
};

} // namespace ui
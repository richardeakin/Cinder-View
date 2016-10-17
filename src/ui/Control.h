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

#include "ui/View.h"

namespace ui {

typedef std::shared_ptr<class Control>	ControlRef;

//! Base class for all Controls, which usually are meant to be interacted with by a user.
class Control : public View {
public:
	Control( const ci::Rectf &bounds = ci::Rectf::zero() )	: View( bounds ) {}

	void setTouchCanceled( bool cancel )	{ mTouchCanceled = cancel; }
	bool isTouchCanceled() const			{ return mTouchCanceled; }

	void setCancelPadding( const ci::Rectf &padding )   { mCancelPadding = padding; }
	const ci::Rectf& getCancelPadding() const           { return mCancelPadding; }

	//! Signal that is emitted whenever a Control's value changes
	ci::signals::Signal<void ()>&	getSignalValueChanged()	{ return mSignalValueChanged; }

protected:
	bool hitTestInsideCancelPadding( const ci::vec2 &localPos ) const;

private:
	ci::Rectf	mCancelPadding = ci::Rectf( 40, 40, 40, 40 );
	bool		mTouchCanceled = false;

	ci::signals::Signal<void ()>	mSignalValueChanged;
};

} // namespace ui

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

#include "ui/Control.h"
#include "ui/TextManager.h"

#include <vector>

namespace ui {

typedef std::shared_ptr<class VSelector>	VSelectorRef;

class SelectorBase : public Control {
public:
	SelectorBase( const ci::Rectf &bounds = ci::Rectf::zero() );

	const std::vector<std::string>&	getSegmentLabels() const			{ return mSegments; }
	std::vector<std::string>&		getSegmentLabels()					{ return mSegments; }

	void	setSegmentLabels( const std::vector<std::string> &segments )	{ mSegments = segments; }

	size_t				getSelectedIndex() const	{ return mSelectedIndex; }
	const std::string&	getSelectedLabel() const;

	const std::string&	getTitle() const						{ return mTitle; }
	void				setTitle( const std::string &title )	{ mTitle = title; }

	void				setSelectedColor( const ci::ColorA &color )		{ mSelectedColor = color; }
	const ci::ColorA&	getSelectedColor() const						{ return mSelectedColor; }
	void				setUnselectedColor( const ci::ColorA &color )	{ mUnselectedColor = color; }
	const ci::ColorA&	getUnselectedColor() const						{ return mUnselectedColor; }
	void				setTitleColor( const ci::ColorA &color )		{ mTitleColor = color; }
	const ci::ColorA&	getTitleColor() const							{ return mTitleColor; }

	//! Causes value changed signal to be fired if the selection changes.
	void select( size_t index );
	//! Causes value changed signal to be fired if the selection changes.
	void select( const std::string &label );

	ci::signals::Signal<void ()>&	getSignalValueChanged()	{ return mSignalValueChanged; }

protected:
	void draw( Renderer *ren )	override;

	bool touchesBegan( const ci::app::TouchEvent &event )	override;
	bool touchesMoved( const ci::app::TouchEvent &event )	override;
	bool touchesEnded( const ci::app::TouchEvent &event )	override;

	void updateSelection( const ci::vec2 &pos );

	std::vector<std::string>	mSegments;
	size_t						mSelectedIndex = 0;
	ci::ColorA					mSelectedColor = ci::ColorA::gray( 1, 0.9f );
	ci::ColorA					mUnselectedColor = ci::ColorA::gray( 1, 0.5f );
	ci::ColorA					mTitleColor = ci::ColorA::gray( 0.75f, 0.5f );
	std::string					mTitle;
	TextRef						mTextLabel;

	ci::signals::Signal<void ()>				mSignalValueChanged;
};

class VSelector : public SelectorBase {
public:
	VSelector( const ci::Rectf &bounds = ci::Rectf::zero() ) : SelectorBase( bounds )	{}

protected:
};

} // namespace ui

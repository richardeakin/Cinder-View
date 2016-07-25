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

#include "ui/View.h"
#include "ui/Button.h"
#include "ui/Label.h"
#include "ui/Selector.h"
#include "ui/Slider.h"
#include "ui/Graph.h"

#include "mason/Factory.h"

namespace ui {

typedef std::shared_ptr<class Suite>		SuiteRef;
typedef std::shared_ptr<class SuiteView>	SuiteViewRef;

class Suite;

//! View type that can be registered with and managed by Suite.
class SuiteView : public ui::View {
  public:
	Suite*  getSuite() const   { return mSuite; }

  private:
	Suite*  mSuite = nullptr;

	friend class Suite;
};

//! Class for managing a selection of Views with some basic Controls and automatic layout. Useful for test or sample sets.
class Suite {
  public:
	Suite();

	//! Registers a subclass of SuiteView with an associated \a key that will be displayed in a VSelector on screen.
	template<typename Y>
	void registerSuiteView( const std::string &key );

	//! Selects a registered SuiteView by index
	void                select( size_t index );
	//! Returns the index of the currently selected SuiteView
	size_t              getCurrentIndex() const	    { return mSelector->getSelectedIndex(); }
	//! Returns the key associated with the currently selected SuiteView

	const std::string&  getCurrentKey() const   { return mCurrentTestKey; }
	//! Returns the Control used for selecting a SuiteView
	ui::VSelectorRef	getSelector() const	        { return mSelector; }
	//! Returns a LabelGrid that SuiteViews can use to display information
	ui::LabelGridRef    getInfoLabel() const        { return mInfoLabel; }

	//! Reloads the current SuiteView
	void reload();

	//! Causes the Graph to be updated
	void update();
	//! Causes the Graph to be drawn
	void draw();
	//! Returns the Graph owned by this Suite
	ui::GraphRef	getGraph() const	{ return mGraph; }

	void setDrawUiEnabled( bool enable );
	bool isDrawUiEnabled() const	{ return mDrawUi; }

  private:
	void resize();
	void resizeInfoLabel();
	void selectTest( const std::string &key );
	void updateUI();

	ui::GraphRef		mGraph;
	SuiteViewRef		mCurrentSuiteView;
	ui::VSelectorRef	mSelector;
	ui::LabelGridRef    mInfoLabel;
	std::string			mCurrentTestKey;
	bool				mDrawUi = true;

	mason::Factory<SuiteView>	mFactory;
};

template<typename Y>
void Suite::registerSuiteView( const std::string &key )
{
	mFactory.registerBuilder<Y>( key );
	mSelector->getSegmentLabels().push_back( key );
}

} // namespace ui

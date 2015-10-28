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

#include "view/Suite.h"
#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/audio/Context.h"

using namespace std;
using namespace ci;

namespace view {

// ----------------------------------------------------------------------------------------------------
// MARK: - SuiteView
// ----------------------------------------------------------------------------------------------------

void SuiteView::connectKeyDown( const std::function<void( ci::app::KeyEvent& )> &callback)
{
	mKeyDownConn = app::getWindow()->getSignalKeyDown().connect( callback );
}

// ----------------------------------------------------------------------------------------------------
// MARK: - Suite
// ----------------------------------------------------------------------------------------------------

Suite::Suite()
{
	mRootView = make_shared<view::View>( app::getWindowBounds() );
	mRootView->setLabel( "Suite root" );
	mRootView->connectTouchEvents();

	mSelector = make_shared<view::VSelector>(); // bounds is set in resize()
	mSelector->getBackground()->setColor( ColorA::gray( 0, 0.3f ) );

	mSelector->getSignalValueChanged().connect( [this] {
		CI_LOG_V( "selector changed, index: " << mSelector->getSelectedIndex() << ", label: " << mSelector->getSelectedLabel() );
		selectTest( mSelector->getSelectedLabel() );
	} );

	mRootView->addSubview( mSelector );

	app::getWindow()->getSignalResize().connect( bind( &Suite::resize, this ) );
	app::getWindow()->getSignalKeyDown().connect( bind( &Suite::keyDown, this, placeholders::_1 ) );
}

void Suite::resize()
{
	mRootView->setSize( app::getWindowSize() );
	if( mCurrentSuiteView )
		mCurrentSuiteView->setSize( mRootView->getSize() ); // TODO: autoresize flags?

	const float padding = 6;
	const float width = 120; // TODO: calculate widest segment
	const float height = 22 * mSelector->getSegmentLabels().size();
	mSelector->setBounds( Rectf( (float)mRootView->getWidth() - width - padding, padding, (float)mRootView->getWidth() - padding, height + padding ) );
}

void Suite::selectTest( const string &key )
{
	CI_LOG_V( "selecting test: " << key );

	// first remove and destroy the current test
	if( mCurrentSuiteView ) {
		mCurrentSuiteView->removeFromParent();
		mCurrentSuiteView.reset();		
	}

	auto suiteView = mFactory.build( key );
	if( ! suiteView ) {
		CI_LOG_E( "no registered test for key: " << key );
		return;
	}

	suiteView->setBounds( Rectf( 0, 0, mRootView->getWidth(), mRootView->getHeight() ) );
	suiteView->setLabel( key );
	mRootView->insertSubview( suiteView, 0 );
	mCurrentSuiteView = suiteView;
	mCurrentTestKey = key;
}

void Suite::selectTest( size_t index )
{
	mSelector->select( index );
}

void Suite::reloadCurrentTest()
{
	selectTest( mSelector->getSelectedLabel() );
}

void Suite::keyDown( app::KeyEvent &event )
{
	if( event.getChar() == 'v' ) {
		mRootView->printHeirarchy( app::console() );
	}

}

void Suite::update()
{
	// select the first test if there has been one added to the selector and there is no current test view
	if( ! mCurrentSuiteView && ! mSelector->getSegmentLabels().empty() )
		selectTest( mSelector->getSelectedLabel() );

	mRootView->propagateUpdate();
}

void Suite::draw()
{
	mRootView->propagateDraw();
}

} // namespace view

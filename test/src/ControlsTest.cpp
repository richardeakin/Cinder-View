#include "ControlsTest.h"

#include "cinder/app/App.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"
#include "cinder/Log.h"

using namespace std;
using namespace ci;
using namespace mason;

ControlsTest::ControlsTest()
	: SuiteView()
{
	mButton = make_shared<view::Button>();
	mButton->setTitle( "Moe" );
	mButton->getSignalPressed().connect( [] { CI_LOG_V( "Bob pressed" ); } );
	mButton->getSignalReleased().connect( [] { CI_LOG_V( "Bob released" ); } );

	mToggle = make_shared<view::Button>();
	mToggle->setAsToggle();
	mToggle->setLabel( "toggle" );
	mToggle->setTitle( "Larry" );
	mToggle->setTitle( "Curly", view::Button::State::ENABLED );
	mToggle->getSignalPressed().connect( [] { CI_LOG_V( "toggle pressed" ); } );
	mToggle->getSignalReleased().connect( [] { CI_LOG_V( "toggle released" ); } );

	// temp - adding constrols to this test
	mHSlider = make_shared<view::HSlider>();
	mHSlider->getBackground()->setColor( ColorA( "green", 0.5f ) );
	mHSlider->getSignalValueChanged().connect( [this] {
		CI_LOG_V( "mHSlider value: " << mHSlider->getValue() );
	} );

	mVSlider = make_shared<view::VSlider>();
	mVSlider->getBackground()->setColor( ColorA( "green", 0.5f ) );
	mVSlider->getSignalValueChanged().connect( [this] {
		CI_LOG_V( "mVSlider value: " << mVSlider->getValue() );
	} );

	addSubviews( { mButton, mToggle, mHSlider, mVSlider } );

	connectKeyDown( signals::slot( this, &ControlsTest::keyEvent ) );
}

void ControlsTest::layout()
{
	const float padding = 40.0f;

	Rectf buttonBounds( padding, padding, padding + 80, padding + 30 );
	mButton->setBounds( buttonBounds );

	buttonBounds += vec2( 0, buttonBounds.getHeight() + 10 );
	mToggle->setBounds( buttonBounds );

	Rectf sliderHBounds = Rectf( padding, buttonBounds.y2 + padding, padding + 200, buttonBounds.y2 + padding + 30 );
	mHSlider->setBounds( sliderHBounds );

	Rectf sliderVBounds = Rectf( padding, sliderHBounds.y2 + 10, padding + 30, sliderHBounds.y2 + 210 );
	mVSlider->setBounds( sliderVBounds );
}

void ControlsTest::keyEvent( app::KeyEvent &event )
{
}

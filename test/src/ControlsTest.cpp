#include "ControlsTest.h"

#include "cinder/app/App.h"
//#include "cinder/Rand.h"
#include "cinder/Log.h"
#include "cinder/gl/draw.h"

using namespace std;
using namespace ci;
using namespace mason;

ControlsTest::ControlsTest()
	: SuiteView()
{
	mButton = make_shared<ui::Button>();
	mButton->setTitle( "color button" );
	mButton->getSignalPressed().connect( [] { CI_LOG_V( "Bob pressed" ); } );
	mButton->getSignalReleased().connect( [] { CI_LOG_V( "Bob released" ); } );

	mImageButton = make_shared<ui::Button>();
	mImageButton->setLabel( "image button" );
	try {
		auto imageNormal = make_shared<ui::Image>( loadImage( app::loadAsset( "images/button_normal.png" ) ) );
		auto imagePressed = make_shared<ui::Image>( loadImage( app::loadAsset( "images/button_pressed.png" ) ) );
		auto imageEnabled = make_shared<ui::Image>( loadImage( app::loadAsset( "images/button_enabled.png" ) ) );

		mImageButton->setImage( imageNormal, ui::Button::State::NORMAL );
		mImageButton->setImage( imagePressed, ui::Button::State::PRESSED );
		mImageButton->setImage( imageEnabled, ui::Button::State::ENABLED );

		mImageButton->setSize( imageNormal->getSize() );
		mImageButton->setAsToggle();
	}
	catch( exception &exc ) {
		CI_LOG_EXCEPTION( "failed to load images for button", exc );
	}

	mToggle = make_shared<ui::Button>();
	mToggle->setAsToggle();
	mToggle->setLabel( "toggle" );
	mToggle->setTitle( "Larry" );
	mToggle->setTitle( "Curly", ui::Button::State::ENABLED );
	mToggle->setColor( Color( 0.2f, 0.5f, 0.5f ), ui::Button::State::ENABLED );
	mToggle->getSignalPressed().connect( [] { CI_LOG_V( "toggle pressed" ); } );
	mToggle->getSignalReleased().connect( [] { CI_LOG_V( "toggle released" ); } );

	// temp - adding constrols to this test
	mHSlider = make_shared<ui::HSlider>();
	mHSlider->getBackground()->setColor( ColorA( "green", 0.5f ) );
	mHSlider->getSignalValueChanged().connect( [this] {
//		CI_LOG_V( "mHSlider value: " << mHSlider->getValue() );
	} );

	mVSlider = make_shared<ui::VSlider>();
	mVSlider->getBackground()->setColor( ColorA( "green", 0.5f ) );
	mVSlider->getSignalValueChanged().connect( [this] {
//		CI_LOG_V( "mVSlider value: " << mVSlider->getValue() );
	} );

	mTextField1 = make_shared<ui::TextField>();
	mTextField1->setPlaceholderText( "textfield 1" );
	mTextField1->setTextColor( Color( 0.5f, 1, 1 ), ui::TextField::State::SELECTED );

	mTextField2 = make_shared<ui::TextField>();
	mTextField2->setPlaceholderText( "textfield 2" );
	mTextField2->setInputMode( ui::TextField::InputMode::NUMERIC );
	mTextField2->setTextColor( Color( 0.5f, 1, 1 ), ui::TextField::State::SELECTED );

	mTextField1->setNextResponder( mTextField2 );
	mTextField2->setNextResponder( mTextField1 );

	auto nbox = make_shared<ui::NumberBox>( Rectf( 400, 360, 480, 400 ) );
	nbox->setTitle( "val" );
	//nbox->setBackgroundEnabled( false );

	auto nbox3 = make_shared<ui::NumberBox3>( Rectf( 400, 410, 580, 450 ) );
	ui::NumberBox3* nbox3Ptr = nbox3.get();
	//	nbox3->getSignalValueChanged().connect( [nbox3Ptr] { CI_LOG_I( "nbox3 value: " << nbox3Ptr->getValue(); ); } );

	addSubviews( { 
		mButton,
		mImageButton,
		mToggle,
		mHSlider,
		mVSlider,
		nbox,
		nbox3,
		mTextField1,
		mTextField2
	} );
}

void ControlsTest::layout()
{
	// TODO: move these all to a container view and use a Layout
	const float padding = 40.0f;

	Rectf buttonBounds( padding, padding, padding + 80, padding + 30 );
	mButton->setBounds( buttonBounds );

	buttonBounds += vec2( 0, buttonBounds.getHeight() + 10 );
	mToggle->setBounds( buttonBounds );

	mImageButton->setPos( vec2( mButton->getBounds().x2 + padding, mButton->getPosY() ) );

	Rectf sliderHBounds = Rectf( padding, buttonBounds.y2 + padding, padding + 200, buttonBounds.y2 + padding + 30 );
	mHSlider->setBounds( sliderHBounds );

	Rectf sliderVBounds = Rectf( padding, sliderHBounds.y2 + 10, padding + 30, sliderHBounds.y2 + 210 );
	mVSlider->setBounds( sliderVBounds );

    Rectf textFieldBounds = Rectf( padding, sliderVBounds.y2 + 20, 300, sliderVBounds.y2 + 60 );
    mTextField1->setBounds( textFieldBounds );

	textFieldBounds += vec2( 0, textFieldBounds.getHeight() + 20 );
	mTextField2->setBounds( textFieldBounds );
}

bool ControlsTest::keyDown( app::KeyEvent &event )
{
//	switch( event.getChar() ) {
//	}

	return false;
}

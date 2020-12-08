#include "ControlsTest.h"

#include "cinder/app/App.h"
#include "cinder/Log.h"

using namespace std;
using namespace ci;
using namespace mason;

ControlsTest::ControlsTest()
	: SuiteView()
{
	mButton = make_shared<vu::Button>();
	mButton->setTitle( "Button" );
	mButton->getSignalPressed().connect( [] { CI_LOG_I( "mButton pressed" ); } );
	mButton->getSignalReleased().connect( [] { CI_LOG_I( "mButton released" ); } );

	mToggle = make_shared<vu::Button>();
	mToggle->setAsToggle();
	mToggle->setLabel( "toggle" );
	mToggle->setTitle( "disabled" );
	mToggle->setTitle( "enabled", vu::Button::State::ENABLED );
	mToggle->setColor( Color( 0.2f, 0.5f, 0.5f ), vu::Button::State::ENABLED );
	mToggle->getTitleLabel()->setFontSize( 28 );
	mToggle->getSignalPressed().connect( [] { CI_LOG_V( "toggle pressed" ); } );
	mToggle->getSignalReleased().connect( [] { CI_LOG_V( "toggle released" ); } );

	mCheckBox = make_shared<vu::CheckBox>();
	mCheckBox->setTitle( "checkbox" );

	mImageButton = make_shared<vu::Button>();
	mImageButton->setLabel( "image button" );
	try {
		auto imageNormal = make_shared<vu::Image>( loadImage( app::loadAsset( "images/button_normal.png" ) ) );
		auto imagePressed = make_shared<vu::Image>( loadImage( app::loadAsset( "images/button_pressed.png" ) ) );
		auto imageEnabled = make_shared<vu::Image>( loadImage( app::loadAsset( "images/button_enabled.png" ) ) );

		mImageButton->setImage( imageNormal, vu::Button::State::NORMAL );
		mImageButton->setImage( imagePressed, vu::Button::State::PRESSED );
		mImageButton->setImage( imageEnabled, vu::Button::State::ENABLED );

		mImageButton->setSize( imageNormal->getSize() );
		mImageButton->setAsToggle();
	}
	catch( exception &exc ) {
		CI_LOG_EXCEPTION( "failed to load images for button", exc );
	}

	// temp - adding controls to this test
	mHSlider = make_shared<vu::HSlider>();
	mHSlider->getBackground()->setColor( ColorA( "green", 0.5f ) );
	mHSlider->getSignalValueChanged().connect( [this] {
//		CI_LOG_V( "mHSlider value: " << mHSlider->getValue() );
	} );

	mVSlider = make_shared<vu::VSlider>();
	mVSlider->getBackground()->setColor( ColorA( "green", 0.5f ) );
	mVSlider->getSignalValueChanged().connect( [this] {
//		CI_LOG_V( "mVSlider value: " << mVSlider->getValue() );
	} );

	auto selectedColor = Color( 0.5f, 1, 1 );

	mTextField1 = make_shared<vu::TextField>();
	mTextField1->setPlaceholderText( "textfield 1" );
	mTextField1->setTextColor( selectedColor, vu::TextField::State::SELECTED );
	mTextField1->setBorderColor( selectedColor, vu::TextField::State::SELECTED );

	mTextField2 = make_shared<vu::TextField>();
	mTextField2->setPlaceholderText( "textfield 2" );
	mTextField2->setTextColor( selectedColor, vu::TextField::State::SELECTED );
	mTextField2->setBorderColor( selectedColor, vu::TextField::State::SELECTED );

	mTextField3 = make_shared<vu::TextField>();
	mTextField3->setPlaceholderText( "textfield 3" );
	mTextField3->setInputMode( vu::TextField::InputMode::NUMERIC );
	mTextField3->setTextColor( selectedColor, vu::TextField::State::SELECTED );
	mTextField3->setBorderColor( selectedColor, vu::TextField::State::SELECTED );

	mTextField1->setNextResponder( mTextField2 );
	mTextField2->setNextResponder( mTextField3 );
	mTextField3->setNextResponder( mTextField1 );

	auto nboxA = make_shared<vu::NumberBox>( Rectf( 400, 360, 500, 400 ) );
	nboxA->setTitle( "valA" );
	//nbox->setBackgroundEnabled( false );

	auto nboxB = make_shared<vu::NumberBox>( Rectf( 510, 360, 610, 400 ) );
	nboxB->setTitle( "valB" );

	auto nbox3 = make_shared<vu::NumberBox3>( Rectf( 400, 410, 580, 450 ) );
	nbox3->setTitle( "nbox3" );
//	nbox3->getSignalValueChanged().connect( [&nbox3] { CI_LOG_I( "nbox3 value: " << nbox3->getValue(); ); } );

	nboxA->setNextResponder( nboxB );
	nboxB->setNextResponder( nbox3 );
	nbox3->setNextResponder( nboxA );

	addSubviews( { 
		mButton,
		mToggle,
		mCheckBox,
		mImageButton,
		mHSlider, mVSlider,
		nboxA, nboxB, nbox3,
		mTextField1, mTextField2, mTextField3
	} );
}

void ControlsTest::layout()
{
	// TODO: move these all to a container view and use a Layout
	const float padding = 40.0f;

	Rectf buttonBounds( padding, padding, padding + 90, padding + 30 );
	mButton->setBounds( buttonBounds );

	buttonBounds += vec2( 0, buttonBounds.getHeight() + 10 );
	mToggle->setBounds( buttonBounds );

	buttonBounds.moveULTo( vec2( buttonBounds.x2 + padding, padding ) );
	mCheckBox->setBounds( buttonBounds );

	mImageButton->setPos( vec2( mCheckBox->getBounds().x2 + padding, mCheckBox->getPosY() ) );

	Rectf sliderHBounds = Rectf( padding, mImageButton->getBounds().y2 + padding, padding + 200, mImageButton->getBounds().y2 + padding + 30 );
	mHSlider->setBounds( sliderHBounds );

	Rectf sliderVBounds = Rectf( padding, sliderHBounds.y2 + 10, padding + 30, sliderHBounds.y2 + 210 );
	mVSlider->setBounds( sliderVBounds );

    Rectf textFieldBounds = Rectf( padding, sliderVBounds.y2 + 10, 300, sliderVBounds.y2 + 60 );
    mTextField1->setBounds( textFieldBounds );

	textFieldBounds += vec2( 0, textFieldBounds.getHeight() + 10 );
	mTextField2->setBounds( textFieldBounds );

	textFieldBounds += vec2( 0, textFieldBounds.getHeight() + 10 );
	mTextField3->setBounds( textFieldBounds );
}

bool ControlsTest::keyDown( app::KeyEvent &event )
{
//	switch( event.getChar() ) {
//	}

	return false;
}

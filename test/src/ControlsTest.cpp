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

    mTextField = make_shared<ui::TextField>();

	addSubviews( { 
        mButton,
        mImageButton,
        mToggle,
        mHSlider,
        mVSlider,
        mTextField
    } );
}

void ControlsTest::layout()
{
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
    mTextField->setBounds( textFieldBounds );
}

bool ControlsTest::keyDown( app::KeyEvent &event )
{
//	switch( event.getChar() ) {
//	}

	return false;
}

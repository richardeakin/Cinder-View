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
	mButton->setTitle( "Moe" );
	mButton->getSignalPressed().connect( [] { CI_LOG_V( "Bob pressed" ); } );
	mButton->getSignalReleased().connect( [] { CI_LOG_V( "Bob released" ); } );

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

	mTouchOverlay = make_shared<TouchOverlayView>();

	addSubviews( { mButton, mToggle, mHSlider, mVSlider } );
	addSubview( mTouchOverlay ); // Add last so it is always on top

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
	switch( event.getChar() ) {
		case 't':
			mTouchOverlay->setHidden( ! mTouchOverlay->isHidden() );
		break;
	}
}


TouchOverlayView::TouchOverlayView()
{
	setFillParentEnabled();
	mTextureFont = gl::TextureFont::create( Font( "Arial", 12 ) );
}

void TouchOverlayView::draw( ui::Renderer *ren )
{
	const float circleRadius = 14;


	const auto &touches = app::App::get()->getActiveTouches();
	for( size_t i = 0; i < touches.size(); i++ ) {
		vec2 pos = touches[i].getPos();

		ren->setColor( Color( 0, 1, 1 ) );
		gl::drawStrokedCircle( pos, circleRadius, 2.0f );

		// TODO: should these be in the same order from event to event?
		// - drawing the index like below shows that the order of the touches is changing.
//		ren->setColor( Color::white() );
//		Rectf fitRect( pos.x - circleRadius, pos.y - circleRadius, pos.x + circleRadius, pos.y + circleRadius  );
//		mTextureFont->drawString( to_string( i ), vec2( pos.x - 3, pos.y + 4 ) );
	}
}

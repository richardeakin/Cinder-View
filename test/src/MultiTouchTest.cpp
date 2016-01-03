#include "MultiTouchTest.h"

#include "cinder/app/App.h"
//#include "cinder/Rand.h"
#include "cinder/Log.h"
#include "cinder/gl/draw.h"

using namespace std;
using namespace ci;
using namespace mason;

MultiTouchTest::MultiTouchTest()
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
	mVSlider2 = make_shared<ui::VSlider>();
	mVSlider2->getBackground()->setColor( ColorA( "green", 0.5f ) );
	mVSlider2->getSignalValueChanged().connect( [this] {
//		CI_LOG_V( "mHSlider value: " << mHSlider->getValue() );
	} );

	mVSlider1 = make_shared<ui::VSlider>();
	mVSlider1->getBackground()->setColor( ColorA( "green", 0.5f ) );
	mVSlider1->getSignalValueChanged().connect( [this] {
//		CI_LOG_V( "mVSlider value: " << mVSlider->getValue() );
	} );

	mTouchOverlay = make_shared<TouchOverlayView>();

	addSubviews( { mButton, mToggle, mVSlider1, mVSlider2 } );
	addSubview( mTouchOverlay ); // Add last so it is always on top

	connectKeyDown( signals::slot( this, &MultiTouchTest::keyEvent ) );
}

void MultiTouchTest::layout()
{
	const float padding = 40.0f;

	Rectf buttonBounds( padding, padding, padding + 120, padding + 60 );
	mButton->setBounds( buttonBounds );

	buttonBounds += vec2( 0, buttonBounds.getHeight() + 10 );
	mToggle->setBounds( buttonBounds );

	Rectf sliderBounds = Rectf( padding, buttonBounds.y2 + padding, padding + 80, buttonBounds.y2 + 300 + padding );
	mVSlider1->setBounds( sliderBounds );

	sliderBounds += vec2( sliderBounds.getWidth() + padding, 0 );
	mVSlider2->setBounds( sliderBounds );
}

void MultiTouchTest::keyEvent( app::KeyEvent &event )
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

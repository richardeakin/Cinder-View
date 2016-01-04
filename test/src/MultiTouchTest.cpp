#include "MultiTouchTest.h"

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

	mVSlider1 = make_shared<ui::VSlider>();
	mVSlider1->setLabel( "slider1" );
	mVSlider1->getBackground()->setColor( ColorA( "green", 0.5f ) );
	mVSlider1->getSignalValueChanged().connect( [this] {
//		CI_LOG_V( "mVSlider value: " << mVSlider->getValue() );
	} );

	// temp - adding constrols to this test
	mVSlider2 = make_shared<ui::VSlider>();
	mVSlider2->setLabel( "slider2" );
//	mVSlider2->setCancelPadding( Rectf( 40, 40, 200, 40 ) );
	mVSlider2->getBackground()->setColor( ColorA( "green", 0.5f ) );
	mVSlider2->getSignalValueChanged().connect( [this] {
//		CI_LOG_V( "mHSlider value: " << mHSlider->getValue() );
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

	Rectf sliderBounds = Rectf( padding, buttonBounds.y2 + padding, padding + 120, buttonBounds.y2 + 300 + padding );
	mVSlider1->setBounds( sliderBounds );

	CI_LOG_I( "slider1 bounds: " << sliderBounds );

	sliderBounds += vec2( sliderBounds.getWidth() + padding, 0 );
	mVSlider2->setBounds( sliderBounds );

	CI_LOG_I( "slider2 bounds: " << sliderBounds );
}

void MultiTouchTest::keyEvent( app::KeyEvent &event )
{
	switch( event.getChar() ) {
		case 't':
			mTouchOverlay->setHidden( ! mTouchOverlay->isHidden() );
			break;
	}
}

void MultiTouchTest::injectTouches()
{
	vec2 pos1 = mVSlider1->getCenter();
	vec2 pos2 = mVSlider2->getCenter();

	auto graph = getGraph();
	{
		vector<app::TouchEvent::Touch> touches = {
			app::TouchEvent::Touch( pos1, vec2( 0 ), 1, 0, nullptr ),
			app::TouchEvent::Touch( pos2, vec2( 0 ), 2, 0, nullptr )
		};
		app::TouchEvent touchEvent( graph->getWindow(), touches );
		graph->propagateTouchesBegan( touchEvent );
	}

	{
		pos1 += vec2( 0, 60 );
		pos2 += vec2( 0, 60 );
		vector<app::TouchEvent::Touch> touches = {
			app::TouchEvent::Touch( pos1, vec2( 0 ), 1, 0, nullptr ),
			app::TouchEvent::Touch( pos2, vec2( 0 ), 2, 0, nullptr )
		};
		app::TouchEvent touchEvent( graph->getWindow(), touches );
		graph->propagateTouchesMoved( touchEvent );
	}

	for( int i = 0; i < 10; i++ ) {
		pos1 += vec2( 0, 2 );
		pos2 += vec2( 0, 2 );
		vector<app::TouchEvent::Touch> touches = {
				app::TouchEvent::Touch( pos1, vec2( 0 ), 1, 0, nullptr ),
				app::TouchEvent::Touch( pos2, vec2( 0 ), 2, 0, nullptr )
		};
		app::TouchEvent touchEvent( graph->getWindow(), touches );
		graph->propagateTouchesMoved( touchEvent );
	}

}

void MultiTouchTest::update()
{
	// TODO: make this work from layout
	static bool sFirstTime = true;
	if( sFirstTime ) {
		sFirstTime = false;
		injectTouches();
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

	const auto &touches = getGraph()->getAllTouchesInWindow();
	for( const auto &touch : touches ) {
		vec2 pos = touch.getPos();

		ren->setColor( Color( 0, 1, 1 ) );
		gl::drawStrokedCircle( pos, circleRadius, 2.0f );

		ren->setColor( Color::white() );
		Rectf fitRect( pos.x - circleRadius, pos.y - circleRadius, pos.x + circleRadius, pos.y + circleRadius  );
		mTextureFont->drawString( to_string( touch.getId() ), vec2( pos.x - 3, pos.y + 4 ) );
	}
}

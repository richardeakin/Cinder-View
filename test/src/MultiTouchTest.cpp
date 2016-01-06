#include "MultiTouchTest.h"

#include "cinder/Log.h"
#include "cinder/gl/draw.h"
#include "cinder/Rand.h"

using namespace std;
using namespace ci;
using namespace mason;

class DraggableView : public ui::RectView {
  public:
	DraggableView()
			: RectView( Rectf::zero() )
	{
		setColor( mDefaultColor );
	}
  protected:
	bool touchesBegan( app::TouchEvent &event )	override
	{
		setColor( mBeganColor );

		auto &firstTouch = event.getTouches().front();
		mPosBegan = toLocal( firstTouch.getPos() );

		firstTouch.setHandled();
		return true;
	}

	bool touchesMoved( app::TouchEvent &event )		override
	{
		setColor( mDragColor );

		auto &firstTouch = event.getTouches().front();
		vec2 localPos = toLocal( firstTouch.getPos() );

		vec2 diff = localPos - mPosBegan;
		setPos( getPos() + diff );

		firstTouch.setHandled();
		return true;
	}

	bool touchesEnded( app::TouchEvent &event )		override
	{
		setColor( mDefaultColor );

		auto &firstTouch = event.getTouches().front();
		vec2 localPos = toLocal( firstTouch.getPos() );

		vec2 diff = localPos - mPosBegan;
		setPos( getPos() + diff );

		firstTouch.setHandled();
		return true;
	}

	Color mBeganColor = { 1, 0.7f, 0 };
	Color mDragColor = { 0.8f, 0.4f, 0 };
	Color mDefaultColor = { 0, 0.2f, 0.6f };

	vec2 mPosBegan;
};

MultiTouchTest::MultiTouchTest()
		: SuiteView()
{
	setupControls();
	setupDraggables();

	mDraggablesContainer->setHidden();

	mTouchOverlay = make_shared<TouchOverlayView>();

	addSubview( mTouchOverlay ); // Add last so it is always on top

	connectKeyDown( signals::slot( this, &MultiTouchTest::keyEvent ) );
}

void MultiTouchTest::setupControls()
{
	mControlsContainer = make_shared<ui::View>();
	mControlsContainer->setLabel( "controls container" );
	mControlsContainer->setFillParentEnabled();

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

	// temp - adding constrols to this test
	mVSlider2 = make_shared<ui::VSlider>();
	mVSlider2->setLabel( "slider2" );
//	mVSlider2->setCancelPadding( Rectf( 40, 40, 200, 40 ) );
	mVSlider2->getBackground()->setColor( ColorA( "green", 0.5f ) );

	mControlsContainer->addSubviews( { mButton, mToggle, mVSlider1, mVSlider2 } );
	addSubview( mControlsContainer );
}

void MultiTouchTest::setupDraggables()
{
	const size_t numDraggables = 50;

	mDraggablesContainer = make_shared<ui::View>();
	mDraggablesContainer->setLabel( "draggables container" );
	mDraggablesContainer->setFillParentEnabled();

	for( size_t i = 0; i < numDraggables; i++ ) {
		auto draggable = make_shared<DraggableView>();
		draggable->setSize( vec2( 60 ) );
		mDraggablesContainer->addSubview( draggable );
	}

	addSubview( mDraggablesContainer );
}

void MultiTouchTest::layoutControls()
{
	const float padding = 40.0f;

	Rectf buttonBounds( padding, padding, padding + 120, padding + 60 );
	mButton->setBounds( buttonBounds );

	buttonBounds += vec2( 0, buttonBounds.getHeight() + 10 );
	mToggle->setBounds( buttonBounds );

	Rectf sliderBounds = Rectf( padding, buttonBounds.y2 + padding, padding + 120, buttonBounds.y2 + 300 + padding );
	mVSlider1->setBounds( sliderBounds );

	sliderBounds += vec2( sliderBounds.getWidth() + padding, 0 );
	mVSlider2->setBounds( sliderBounds );
}

void MultiTouchTest::layoutDraggables()
{
	// TODO: mDraggablesContainer's setFillParentEnabled() hasn't been done yet, so can't rely on it's size.
	// - should it before resized before View::layout() is called? It is a subview.
//	auto containerSize = mDraggablesContainer->getSize();
	auto containerSize = getSize();
	for( auto &draggable : mDraggablesContainer->getSubviews() ) {
		draggable->setPos( vec2( randFloat( 0, containerSize.x ), randFloat( 0, containerSize.y ) ) );
	}
}

void MultiTouchTest::layout()
{
	layoutControls();
	layoutDraggables();

//	injectTouches();
}

void MultiTouchTest::keyEvent( app::KeyEvent &event )
{
	switch( event.getChar() ) {
		case 't':
			mTouchOverlay->setHidden( ! mTouchOverlay->isHidden() );
			break;
		case 'v':
			CI_LOG_I( "num views with touches: " << getGraph()->getViewsWithTouches().size() );
			break;
		case '1':
			mControlsContainer->setHidden( false );
			mDraggablesContainer->setHidden( true );
			break;
		case '2':
			mControlsContainer->setHidden( true );
			mDraggablesContainer->setHidden( false );
			break;
	}
}

void MultiTouchTest::injectTouches()
{
	vec2 pos1 = mVSlider1->getCenter();
	vec2 pos2 = mVSlider2->getCenter();

	vector<app::TouchEvent::Touch> touches = {
			app::TouchEvent::Touch( pos1, vec2( 0 ), 1, 0, nullptr ),
			app::TouchEvent::Touch( pos2, vec2( 0 ), 2, 0, nullptr )
	};

	auto graph = getGraph();
	{
		app::TouchEvent touchEvent( graph->getWindow(), touches );
		graph->propagateTouchesBegan( touchEvent );
	}

	{
		pos1 += vec2( 0, 60 );
		pos2 += vec2( 0, 60 );
		touches.at( 0 ).setPos( pos1 );
		touches.at( 1 ).setPos( pos2 );
		app::TouchEvent touchEvent( graph->getWindow(), touches );
		graph->propagateTouchesMoved( touchEvent );
	}

	{
		app::TouchEvent touchEvent( graph->getWindow(), touches );
		graph->propagateTouchesEnded( touchEvent );
	}
}

void MultiTouchTest::update()
{
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

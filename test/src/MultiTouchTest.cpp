#include "MultiTouchTest.h"

#include "cinder/Log.h"
#include "cinder/gl/draw.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"
#include "cinder/app/App.h"

using namespace std;
using namespace ci;
using namespace mason;

const float PADDING = 40.0f;

const size_t NUM_DRAGGABLES = 1000;
const size_t NUM_TOUCHES = 16;

//const size_t NUM_DRAGGABLES = 100;
//const size_t NUM_TOUCHES = 1;

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
		mPrevPos = firstTouch.getPos(); // Don't need local position as we'll just use the difference

		firstTouch.setHandled();
		return true;
	}

	bool touchesMoved( app::TouchEvent &event )		override
	{
		setColor( mDragColor );

		auto &firstTouch = event.getTouches().front();
		vec2 pos = firstTouch.getPos();

		vec2 diff = pos - mPrevPos;
		vec2 prevPosDebug = mPrevPos;
		mPrevPos = pos;
		setPos( getPos() + diff );

		firstTouch.setHandled();
		return true;
	}

	bool touchesEnded( app::TouchEvent &event )		override
	{
		setColor( mDefaultColor );

		auto &firstTouch = event.getTouches().front();
		vec2 pos = firstTouch.getPos();

		vec2 diff = pos - mPrevPos;
		setPos( getPos() + diff );

		firstTouch.setHandled();
		return true;
	}

	Color mBeganColor = { 1, 0.7f, 0 };
	Color mDragColor = { 0.8f, 0.4f, 0 };
	Color mDefaultColor = { 0, 0.2f, 0.6f };

	vec2 mPrevPos; // TODO: pass this through in test injection and pull out of Touch
};

MultiTouchTest::MultiTouchTest()
		: SuiteView()
{
	mTestTouches.resize( NUM_TOUCHES );

	setupControls();
	setupDraggables();

	mControlsContainer->setHidden();

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
	mDraggablesContainer = make_shared<ui::View>();
	mDraggablesContainer->setLabel( "draggables container" );
	mDraggablesContainer->setFillParentEnabled();

	for( size_t i = 0; i < NUM_DRAGGABLES; i++ ) {
		auto draggable = make_shared<DraggableView>();
		draggable->setLabel( "DraggableView-" + to_string( i ) );
		draggable->setSize( vec2( 60 ) );
		mDraggablesContainer->addSubview( draggable );
	}

	addSubview( mDraggablesContainer );
}

void MultiTouchTest::layoutControls()
{
	Rectf buttonBounds( PADDING, PADDING, PADDING + 120, PADDING + 60 );
	mButton->setBounds( buttonBounds );

	buttonBounds += vec2( 0, buttonBounds.getHeight() + 10 );
	mToggle->setBounds( buttonBounds );

	Rectf sliderBounds = Rectf( PADDING, buttonBounds.y2 + PADDING, PADDING + 120, buttonBounds.y2 + 300 + PADDING );
	mVSlider1->setBounds( sliderBounds );

	sliderBounds += vec2( sliderBounds.getWidth() + PADDING, 0 );
	mVSlider2->setBounds( sliderBounds );
}

void MultiTouchTest::layoutDraggables()
{
	// TODO: mDraggablesContainer's setFillParentEnabled() hasn't been done yet, so can't rely on it's size.
	// - should it before resized before View::layout() is called? It is a subview.
//	auto containerSize = mDraggablesContainer->getSize();
	auto containerSize = getSize();
	for( auto &draggable : mDraggablesContainer->getSubviews() ) {
		draggable->setPos( vec2( randFloat( PADDING, containerSize.x - PADDING ), randFloat( PADDING, containerSize.y - PADDING ) ) );
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
		case 'c':
			mEnableContinuousInjection = ! mEnableContinuousInjection;
			if( ! mEnableContinuousInjection )
				endCountinuousTouches();
			CI_LOG_I( "mEnableContinuousInjection: " << mEnableContinuousInjection );
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

void MultiTouchTest::injectContinuousTouches()
{
	const float maxDragDistance = 100;
	const vec2 dragDurationRange = { 0.5f, 2.0f };
	const vec2 containerSize = mDraggablesContainer->getSize();

	for( size_t i = 0; i < mTestTouches.size(); i++ ) {
		if( mTestTouches[i].mPhase == TestTouch::Phase::UNUSED ) {
			vec2 startPos( randFloat( PADDING, containerSize.x - PADDING ), randFloat( PADDING, containerSize.y - PADDING ) );
			vec2 endPos( randFloat( startPos.x - maxDragDistance, startPos.x + maxDragDistance ), randFloat( startPos.y - maxDragDistance, startPos.y + maxDragDistance ) );
			endPos = min( endPos, containerSize - vec2( PADDING ) );

			float duration = randFloat( dragDurationRange[0], dragDurationRange[1] );

			//CI_LOG_I( "cueing test touch " << i << ", startPos: " << startPos << ", endPos: " << endPos << ", duration: " << duration );

			mTestTouches[i].mPhase = TestTouch::Phase::BEGAN;

			mTestTouches[i].mPos = startPos;
			app::timeline().apply( &mTestTouches[i].mPos, endPos, duration )
					.easeFn( EaseInOutQuad() )
					.updateFn( [this, i] {
						mTestTouches[i].mPhase = TestTouch::Phase::MOVED;
					} )
					.finishFn( [this, i] {
						mTestTouches[i].mPhase = TestTouch::Phase::ENDED;
					} );
		}
	}

	// send off all touches at once
	auto graph = getGraph();
	vector<app::TouchEvent::Touch> touchesBegan;
	vector<app::TouchEvent::Touch> touchesMoved;
	vector<app::TouchEvent::Touch> touchesEnded;

	for( size_t i = 0; i < mTestTouches.size(); i++ ) {
		auto &t = mTestTouches[i];
		if( t.mPhase == TestTouch::Phase::BEGAN )
			touchesBegan.emplace_back( t.mPos, vec2( 0 ), i, 0, nullptr );
		else if( t.mPhase == TestTouch::Phase::MOVED )
			touchesMoved.emplace_back( t.mPos, vec2( 0 ), i, 0, nullptr );
		else if( t.mPhase == TestTouch::Phase::ENDED ) {
			touchesEnded.emplace_back( t.mPos, vec2( 0 ), i, 0, nullptr );
			t.mPhase = TestTouch::Phase::UNUSED;
		}
	}

	if( ! touchesBegan.empty() ) {
		app::TouchEvent touchEvent( graph->getWindow(), touchesBegan );
		graph->propagateTouchesBegan( touchEvent );
	}
	if( ! touchesMoved.empty() ) {
		app::TouchEvent touchEvent( graph->getWindow(), touchesMoved );
		graph->propagateTouchesMoved( touchEvent );
	}
	if( ! touchesEnded.empty() ) {
		app::TouchEvent touchEvent( graph->getWindow(), touchesEnded );
		graph->propagateTouchesEnded( touchEvent );
	}
}

void MultiTouchTest::endCountinuousTouches()
{
	vector<app::TouchEvent::Touch> touchesEnded;
	for( size_t i = 0; i < mTestTouches.size(); i++ ) {
		auto &t = mTestTouches[i];
		touchesEnded.emplace_back( t.mPos, vec2( 0 ), i, 0, nullptr );
	}

	auto graph = getGraph();
	app::TouchEvent touchEvent( graph->getWindow(), touchesEnded );
	graph->propagateTouchesEnded( touchEvent );
}

void MultiTouchTest::update()
{
	if( mEnableContinuousInjection && ! mDraggablesContainer->isHidden() ) {
		injectContinuousTouches();
	}
}

TouchOverlayView::TouchOverlayView()
{
	setFillParentEnabled();
	mTextureFont = gl::TextureFont::create( Font( "Arial", 12 ) );
	mBatchCircle = gl::Batch::create( geom::WireCircle().radius( 14 ).subdivisions( 40 ), gl::getStockShader( gl::ShaderDef().color() ) );
}

void TouchOverlayView::draw( ui::Renderer *ren )
{
	const float circleRadius = 14;

	const auto &touches = getGraph()->getAllTouchesInWindow();
	for( const auto &touch : touches ) {
		vec2 pos = touch.getPos();
		{
			ren->setColor( Color( 0, 1, 1 ) );
			gl::ScopedModelMatrix modelScope;
			gl::translate( pos );
			mBatchCircle->draw();
		}
		{
			ren->setColor( Color::white() );
			Rectf fitRect( pos.x - circleRadius, pos.y - circleRadius, pos.x + circleRadius, pos.y + circleRadius  );
			mTextureFont->drawString( to_string( touch.getId() ), vec2( pos.x - 3, pos.y + 4 ) );
		}
	}
}

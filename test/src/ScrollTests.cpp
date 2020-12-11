#include "ScrollTests.h"
#include "vu/ImageView.h"
#include "vu/Label.h"
#include "mason/Format.h"

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"
#include "cinder/Log.h"


using namespace std;
using namespace ci;

const float PADDING = 40.0f;
const ColorA SCROLL_BACKGROUND_COLOR = ColorA( 0.8f, 0.4f, 0, 1 );


class CustomScrollTestsView : public vu::RectView {
public:
	CustomScrollTestsView( const Rectf &bounds )
		: RectView( bounds )
	{
		setInteractive();
		setColor( Color( 0, 0.5f, 0 ) );
	}

protected:
	bool touchesBegan( app::TouchEvent &event )	override
	{
		mLocalTouchColor = Color( 0.7f, 0.0f, 0.0f );

		vec2 pos = event.getTouches().front().getPos();
		event.getTouches().front().setHandled();
		mLocalTouchPos = toLocal( pos );
		CI_LOG_I( getLabel() << " local pos: " << mLocalTouchPos << ", world pos: " << getWorldPos() << ", event pos: " << pos );

		return true;
	}

	bool touchesMoved( app::TouchEvent &event )		override
	{
		mLocalTouchColor = Color( 0.7f, 0.3f, 0.0f );

		vec2 pos = event.getTouches().front().getPos();
		mLocalTouchPos = toLocal( pos );
		CI_LOG_I( getLabel() << " local pos: " << mLocalTouchPos << ", world pos: " << getWorldPos() << ", event pos: " << pos );

		return true;
	}

	bool touchesEnded( app::TouchEvent &event )		override
	{
		mLocalTouchColor = Color::gray( 0.3f );

		vec2 pos = event.getTouches().front().getPos();
		mLocalTouchPos = toLocal( pos );
		CI_LOG_I( getLabel() << " local pos: " << mLocalTouchPos << ", world pos: " << getWorldPos() << ", event pos: " << pos );

		return true;
	}

	void draw( vu::Renderer *ren ) override
	{
		RectView::draw( ren );

		ren->setColor( mLocalTouchColor );
		gl::drawSolidCircle( mLocalTouchPos, 10.0f );
	}

	vec2 mLocalTouchPos;
	Color mLocalTouchColor = Color::gray( 0.3f );
};

ScrollTests::ScrollTests()
	: SuiteView()
{
	// Free scrolling ScrollView
	mScrollViewFree = make_shared<vu::ScrollView>();
//	mScrollView->setClipEnabled( false );
	mScrollViewFree->setLabel( "ScrollView (free)" );
	mScrollViewFree->getContentView()->getBackground()->setColor( SCROLL_BACKGROUND_COLOR );

	{
		auto scrollBorder = make_shared<vu::StrokedRectView>();
		scrollBorder->setFillParentEnabled();
		scrollBorder->setColor( ColorA( 0.9f, 0.5f, 0.0f, 0.7f ) );
		//mScrollViewFree->addSubview( scrollBorder );

		// add some content views:
		auto custom = make_shared<CustomScrollTestsView>( Rectf( 40, 30, 180, 130 ) );

		auto button = make_shared<vu::Button>( Rectf( 200, 30, 300, 70 ) );
		button->setTitle( "button" );
		button->setLabel( "Button ('ScrollView - free')" );
		button->setTitleColor( Color::white() );
		button->setColor( Color( 0.4f, 0.6f, 0.9f ), vu::Button::State::PRESSED );
		button->getSignalPressed().connect( [] { CI_LOG_I( "button pressed" ); } );
		button->getSignalReleased().connect( [] { CI_LOG_I( "button released" ); } );

		auto imageView = make_shared<vu::ImageView>( Rectf( 40, 150, 600, 600 ) );
		imageView->getBackground()->setColor( Color( 0, 0.2f, 0 ) );

		fs::path imageFilePath = app::getAssetPath( "images/monkey_hitchhike.jpg" );
		try {
			CI_LOG_I( "loading image view.." );
			auto image = make_shared<vu::Image>( loadImage( loadFile( imageFilePath ) ) );
			imageView->setImage( image );
			imageView->setSize( image->getSize() );
			CI_LOG_I( "complete" );
		}
		catch( std::exception &exc ) {
			CI_LOG_EXCEPTION( "failed to load image at path: " << imageFilePath, exc );
		}

		mScrollViewFree->addContentViews( { custom, imageView, button } );
	}

	// Horizontal paging ScrollView
	mHorizontalPager = make_shared<vu::PagingScrollView>();
	mHorizontalPager->setLabel( "PagingScrollView (horizontal)" );
	mHorizontalPager->setPageMargin( vec2( 4, 0 ) );
	mHorizontalPager->getBackground()->setColor( SCROLL_BACKGROUND_COLOR );
	const size_t numHorizontalPages = 4;
	for( size_t i = 0; i < numHorizontalPages; i++ ) {
		auto label = make_shared<vu::Label>( Rectf( 40, 100, 140, 130 ) );
		label->setFontSize( 36 );
		label->setText( "Page " + to_string( i ) );
		label->setTextColor( Color::white() );
		label->getBackground()->setColor( Color( CM_HSV, 0.5f + (float)i * 0.4f / (float)numHorizontalPages, 1.0f, 0.75f ) );
		mHorizontalPager->addContentView( label );
	}

	// Vertical paging ScrollView
	mVerticalPager = make_shared<vu::PagingScrollView>();
	mVerticalPager->setLabel( "PagingScrollView (vertical)" );
	mVerticalPager->setAxis( vu::PagingScrollView::Axis::VERTICAL );
	mVerticalPager->setPageMargin( vec2( 0, 4 ) );
	mVerticalPager->getBackground()->setColor( SCROLL_BACKGROUND_COLOR );
	const size_t numVerticalPages = 10;
	for( size_t i = 0; i < numVerticalPages; i++ ) {
		auto label = make_shared<vu::Label>( Rectf( 40, 100, 140, 130 ) );
		label->setFontSize( 36 );
		label->setText( "Page " + to_string( i ) );
		label->setTextColor( Color::white() );
		label->getBackground()->setColor( Color( CM_HSV, 1.0f - (float)i * 0.2f / (float)numHorizontalPages, 1.0f, 0.75f ) );
		mVerticalPager->addContentView( label );
	}

	mScrollViewNested = make_shared<vu::ScrollView>();
	mScrollViewNested->setLabel( "ScrollView (nested parent)" );
	mScrollViewNested->setHorizontalScrollingEnabled( false );
	mScrollViewNested->getBackground()->setColor( Color( 0, 0, 0 ) );
	mScrollViewNested->getContentView()->getBackground()->setColor( SCROLL_BACKGROUND_COLOR );
	mScrollViewNested->setClipEnabled( true );

	{
		auto scrollview = make_shared<vu::ScrollView>( Rectf( 40, 100, 1200, 130 ) );
		scrollview->setLabel( "ScrollView (nested child)" );
		scrollview->setVerticalScrollingEnabled( false );
		//scrollview->getContentView()->getBackground()->setColor( Color( 0, 1, 0 ) );
		scrollview->setClipEnabled( false ); // FIXME: enabling clip in nested scrollview breaks parent scrollview's clip (eg "blah blah blah label not clipping)

		auto button = make_shared<vu::Button>();
		button->setSize( vec2( 80, 40 ) );
		button->setTitle( "nested button" );
		button->setLabel( "nested Button" );
		button->setTitleColor( Color::white() );
		button->setColor( Color( 0.4f, 0.6f, 0.9f ), vu::Button::State::PRESSED );
		button->getSignalPressed().connect( [] { CI_LOG_I( "nested button pressed" ); } );
		button->getSignalReleased().connect( [] { CI_LOG_I( "nested button released" ); } );

		auto label = make_shared<vu::Label>();
		label->setPos( button->getBounds().getUpperRight() + vec2( 10, 3 ) );
		label->setSize( vec2( 240, 130 ) );
		label->setFontSize( 36 );
		label->setShrinkToFitEnabled();
		label->setText( "this is a somewhat lengthy label" );
		label->setTextColor( Color::white() );
		label->getBackground()->setColor( Color( 0, 0.3f, 1 ) );

		scrollview->addContentView( button );
		scrollview->addContentView( label );

		mScrollViewNested->addContentView( scrollview );
	}

	mScrollViewWithLayout = make_shared<vu::ScrollView>();
	mScrollViewWithLayout->setLabel( "ScrollView (with layout)" );
	mScrollViewWithLayout->setHorizontalScrollingEnabled( false );
	mScrollViewWithLayout->getContentView()->getBackground()->setColor( SCROLL_BACKGROUND_COLOR );
	mScrollViewWithLayout->getBackground()->setColor( Color( 0, 0, 0 ) );
	mScrollViewWithLayout->setDisableScrollingWhenContentFits();

	{
		auto layout = make_shared<vu::VerticalLayout>();
		layout->setPadding( 6 );
		layout->setAlignment( vu::Alignment::FILL );
		mScrollViewWithLayout->getContentView()->setLayout( layout );
	}

	for( size_t i = 0; i < 20; i++ ) {
		auto label = make_shared<vu::Label>( Rectf( 0, 0, 140, 30 ) );
		label->setFontSize( 24 );
		label->setText( "label " + to_string( i ) );
		label->setTextColor( Color::white() );
		label->getBackground()->setColor( Color( CM_HSV, 0.5f + (float)i * 0.025f, 1.0f, 0.75f ) );
		mScrollViewWithLayout->addContentView( label );
	}

	mInfoLabel = make_shared<vu::LabelGrid>();
	mInfoLabel->setTextColor( Color::white() );
	mInfoLabel->getBackground()->setColor( ColorA::gray( 0, 0.3f ) );

	addSubview( mScrollViewFree );
	addSubview( mHorizontalPager );
	addSubview( mVerticalPager );
	addSubview( mScrollViewNested );
	addSubview( mScrollViewWithLayout );
	addSubview( mInfoLabel );
}

void ScrollTests::layout()
{
	vec2 pos = vec2( PADDING );
	vec2 size = getSize() / vec2( 4, 3.5f );

	mScrollViewFree->setPos( pos );
	mScrollViewFree->setSize( size );

	pos.x += size.x + PADDING;
	mHorizontalPager->setPos( pos );
	mHorizontalPager->setSize( size );

	pos.x += size.x + PADDING;
	mVerticalPager->setPos( pos );
	mVerticalPager->setSize( size );

	pos.x = PADDING;
	pos.y += size.y + PADDING;
	mScrollViewNested->setPos( pos );
	mScrollViewNested->setSize( size );

	pos.x += size.x + PADDING;
	mScrollViewWithLayout->setPos( pos );
	mScrollViewWithLayout->setSize( size );
}

bool ScrollTests::keyDown( app::KeyEvent &event )
{
	if( event.isControlDown() )
		return false;

	const bool animate = true;
	const float animateOffsetDist = 200;

	bool handled = true;
	switch( event.getCode() ) {
		case app::KeyEvent::KEY_c: {
			mScrollViewFree->setClipEnabled( ! mScrollViewFree->isClipEnabled() );
			break;
		}
		case app::KeyEvent::KEY_h: {
			mScrollViewFree->setHorizontalScrollingEnabled( ! mScrollViewFree->isHorizontalScrollingEnabled() );
			break;
		}
		case app::KeyEvent::KEY_v: {
			mScrollViewFree->setVerticalScrollingEnabled( ! mScrollViewFree->isVerticalScrollingEnabled() );
			break;
		}
		case app::KeyEvent::KEY_DOWN: {
			if( event.isShiftDown() ) {
				mVerticalPager->nextPage( animate );
			}
			else {
				mScrollViewFree->setContentOffset( mScrollViewFree->getContentOffset() + vec2( 0, animateOffsetDist ), animate );
			}
			break;
		}
		case app::KeyEvent::KEY_UP: {
			if( event.isShiftDown() ) {
				mVerticalPager->previousPage( animate );
			}
			else {
				mScrollViewFree->setContentOffset( mScrollViewFree->getContentOffset() - vec2( 0, animateOffsetDist ), animate );
			}
			break;
		}
		case app::KeyEvent::KEY_RIGHT: {
			if( event.isShiftDown() ) {
				mHorizontalPager->nextPage( animate );
			}
			else {
				mScrollViewFree->setContentOffset( mScrollViewFree->getContentOffset() + vec2( animateOffsetDist, 0 ), animate );
			}
			break;
		}
		case app::KeyEvent::KEY_LEFT: {
			if( event.isShiftDown() ) {
				mHorizontalPager->previousPage( animate );
			}
			else {
				mScrollViewFree->setContentOffset( mScrollViewFree->getContentOffset() - vec2( animateOffsetDist, 0 ), animate );
			}
			break;
		}
		default:
			handled = false;
	}

	return handled;
}

void ScrollTests::update()
{
	size_t row = 0;
	mInfoLabel->setRow( row++, { "is dragging:",  to_string( mScrollViewFree->isDragging() ) } );
	mInfoLabel->setRow( row++, { "is decelerating:",  to_string( mScrollViewFree->isDecelerating() ) } );
	mInfoLabel->setRow( row++, { "swipe velocity:",  fmt::format( "{:.2f}", mScrollViewFree->getSwipeVelocity() ) } );
	mInfoLabel->setRow( row++, { "scroll velocity:",  fmt::format( "{:.2f}", mScrollViewFree->getScrollVelocity() ) } );
	mInfoLabel->setRow( row++, { "content offset:",  fmt::format( "{:.2f}", mScrollViewFree->getContentOffset() ) } );
	mInfoLabel->setRow( row++, { "target offset:",  fmt::format( "{:.2f}", mScrollViewFree->getTargetOffset() ) } );

	//  resize info label
	{
		const float padding = 6;
		const vec2 infoRowSize = vec2( 350, 20 );

		const int numRows = mInfoLabel->getNumRows();
		vec2 windowSize = vec2( app::getWindow()->getSize() );
		vec2 labelSize = { infoRowSize.x, infoRowSize.y * numRows };
		mInfoLabel->setBounds( { padding, windowSize.y - labelSize.y - padding, labelSize.x + padding, windowSize.y - padding } ); // anchor bottom left
	}
}

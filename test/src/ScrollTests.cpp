#include "ScrollTests.h"
#include "ui/ImageView.h"
#include "ui/Label.h"

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"
#include "cinder/Log.h"

using namespace std;
using namespace ci;
using namespace mason;

const float PADDING = 40.0f;

class CustomView : public ui::RectView {
public:
	CustomView( const Rectf &bounds )
		: RectView( bounds )
	{
		setColor( Color( 0, 0.3f, 0 ) );
	}

protected:
	bool touchesBegan( app::TouchEvent &event )	override
	{
		mLocalTouchColor = Color( 0.7f, 0.0f, 0.0f );

		vec2 pos = event.getTouches().front().getPos();
		event.getTouches().front().setHandled();
		mLocalTouchPos = toLocal( pos );
//		CI_LOG_V( getLabel() << " local pos: " << mLocalTouchPos << ", world pos: " << getWorldPos() << ", event pos: " << pos );

		return true;
	}

	bool touchesMoved( app::TouchEvent &event )		override
	{
		mLocalTouchColor = Color( 0.7f, 0.3f, 0.0f );

		vec2 pos = event.getTouches().front().getPos();
		mLocalTouchPos = toLocal( pos );
//		CI_LOG_V( getLabel() << " local pos: " << mLocalTouchPos << ", world pos: " << getWorldPos() << ", event pos: " << pos );

		return true;
	}

	bool touchesEnded( app::TouchEvent &event )		override
	{
		mLocalTouchColor = Color::gray( 0.3f );

		vec2 pos = event.getTouches().front().getPos();
		mLocalTouchPos = toLocal( pos );
//		CI_LOG_V( getLabel() << " local pos: " << mLocalTouchPos << ", world pos: " << getWorldPos() << ", event pos: " << pos );

		return true;
	}

	void draw( ui::Renderer *ren ) override
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
	mScrollViewFree = make_shared<ui::ScrollView>();
//	mScrollView->setClipEnabled( false );
	mScrollViewFree->setLabel( "ScrollView (free)" );
	mScrollViewFree->getContentView()->getBackground()->setColor( Color( 0.15f, 0, 0 ) );

	auto scrollBorder = make_shared<ui::StrokedRectView>();
	scrollBorder->setFillParentEnabled();
	scrollBorder->setColor( ColorA( 0.9f, 0.5f, 0.0f, 0.7f ) );
	mScrollViewFree->addSubview( scrollBorder );

	// add some content views:
	auto custom = make_shared<CustomView>( Rectf( 40, 30, 180, 130 ) );

	auto button = make_shared<ui::Button>( Rectf( 200, 30, 300, 70 ) );
	button->setTitle( "tap me" );
	button->setTitleColor( Color( 0, 0.2f, 0.8f ) );
	button->setColor( Color( 0.6f, 0.6f, 0.8f ), ui::Button::State::PRESSED );
	button->getSignalPressed().connect( [] { CI_LOG_V( "button pressed" ); } );
	button->getSignalReleased().connect( [] { CI_LOG_V( "button released" ); } );

	auto imageView = make_shared<ui::ImageView>( Rectf( 40, 150, 600, 600 ) );
	imageView->getBackground()->setColor( Color( 0, 0.2f, 0 ) );

	fs::path imageFilePath = app::getAssetPath( "images/monkey_hitchhike.jpg" );
	try {
		CI_LOG_I( "loading image view.." );
		auto image = make_shared<ui::Image>( loadImage( loadFile( imageFilePath ) ) );
		imageView->setImage( image );
		imageView->setSize( image->getSize() );
		CI_LOG_I( "complete" );
	}
	catch( std::exception &exc ) {
		CI_LOG_EXCEPTION( "failed to load image at path: " << imageFilePath, exc );
	}

	mScrollViewFree->addContentViews( { custom, imageView, button } );

	// Horizontal paging ScrollView
	mHorizontalPager = make_shared<ui::PagingScrollView>();
	mHorizontalPager->setLabel( "PagingScrollView (horizontal)" );
	mHorizontalPager->setPageMargin( vec2( 4, 0 ) );
	mHorizontalPager->getBackground()->setColor( Color( 0.8f, 0.4f, 0 ) );
	const size_t numHorizontalPages = 4;
	for( size_t i = 0; i < numHorizontalPages; i++ ) {
		auto label = make_shared<ui::Label>( Rectf( 40, 100, 140, 130 ) );
		label->setFontSize( 36 );
		label->setText( "Page " + to_string( i ) );
		label->setTextColor( Color::white() );
		label->getBackground()->setColor( Color( CM_HSV, 0.5f + (float)i * 0.4f / (float)numHorizontalPages, 1.0f, 0.75f ) );
		mHorizontalPager->addContentView( label );
	}

	// Vertical paging ScrollView
	mVerticalPager = make_shared<ui::PagingScrollView>();
	mVerticalPager->setLabel( "PagingScrollView (vertical)" );
	mVerticalPager->setAxis( ui::PagingScrollView::Axis::VERTICAL );
	mVerticalPager->setPageMargin( vec2( 0, 4 ) );
	mVerticalPager->getBackground()->setColor( Color( 0.8f, 0.4f, 0 ) );
	const size_t numVerticalPages = 10;
	for( size_t i = 0; i < numVerticalPages; i++ ) {
		auto label = make_shared<ui::Label>( Rectf( 40, 100, 140, 130 ) );
		label->setFontSize( 36 );
		label->setText( "Page " + to_string( i ) );
		label->setTextColor( Color::white() );
		label->getBackground()->setColor( Color( CM_HSV, 1.0f - (float)i * 0.2f / (float)numHorizontalPages, 1.0f, 0.75f ) );
		mVerticalPager->addContentView( label );
	}

	mScrollViewNested = make_shared<ui::ScrollView>();
	mScrollViewNested->setLabel( "ScrollView (nested)" );
	mScrollViewNested->setHorizontalScrollingEnabled( false );
	mScrollViewNested->getContentView()->getBackground()->setColor( Color( 0.15f, 0, 0 ) );

	mScrollViewNested->getBackground()->setColor( Color( 0, 0, 0 ) );

	{
		auto scrollview = make_shared<ui::ScrollView>( Rectf( 40, 100, 3000, 130 ) );
		scrollview->setLabel( "ScrollView (nested child)" );
		scrollview->setVerticalScrollingEnabled( false );

		auto label = make_shared<ui::Label>( Rectf( 0, 0, 240, 130 ) );
		label->setFontSize( 36 );
		label->setShrinkToFitEnabled();
		label->setText( "blah blah blah blah blah" );
		label->setTextColor( Color::white() );
		label->getBackground()->setColor( Color( 0, 0, 1 ) );
		scrollview->addContentView( label );

		mScrollViewNested->addContentView( scrollview );
	}

	mScrollViewWithLayout = make_shared<ui::ScrollView>();
	mScrollViewWithLayout->setLabel( "ScrollView (with layout)" );
	mScrollViewWithLayout->setHorizontalScrollingEnabled( false );
	mScrollViewWithLayout->getContentView()->getBackground()->setColor( Color( 0.15f, 0, 0 ) );
	mScrollViewWithLayout->getBackground()->setColor( Color( 0, 0, 0 ) );

	{
		auto layout = make_shared<ui::VerticalLayout>();
		layout->setPadding( 6 );
		layout->setAlignment( ui::Alignment::FILL );
		mScrollViewWithLayout->getContentView()->setLayout( layout );
	}

	for( size_t i = 0; i < 10; i++ ) {
		auto label = make_shared<ui::Label>( Rectf( 0, 0, 140, 30 ) );
		label->setFontSize( 24 );
		label->setText( "label " + to_string( i ) );
		label->setTextColor( Color::white() );
		label->getBackground()->setColor( Color( CM_HSV, 0.5f + (float)i * 0.4f / (float)10, 1.0f, 0.75f ) );
		mScrollViewWithLayout->addContentView( label );
	}

	addSubview( mScrollViewFree );
	addSubview( mHorizontalPager );
	addSubview( mVerticalPager );
	addSubview( mScrollViewNested );
	addSubview( mScrollViewWithLayout );
}

void ScrollTests::layout()
{
	vec2 pos = vec2( PADDING );
	vec2 size = getSize() / 4.0f;

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
		default:
			handled = false;
	}

	return handled;
}

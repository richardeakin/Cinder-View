#include "ScrollViewTest.h"
#include "view/ImageView.h"
#include "view/Button.h"

#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"
#include "cinder/Log.h"

using namespace std;
using namespace ci;
using namespace mason;

const float PADDING = 40.0f;

class CustomView : public view::RectView {
public:
	CustomView( const Rectf &bounds )
		: RectView( bounds )
	{
		setColor( Color( 0, 0.3f, 0 ) );
	}

protected:
	bool touchesBegan( const app::TouchEvent &event )	override
	{
		mLocalTouchColor = Color( 0.7f, 0.0f, 0.0f );

		vec2 pos = event.getTouches().front().getPos();
		mLocalTouchPos = toLocal( pos );
//		CI_LOG_V( getLabel() << " local pos: " << mLocalTouchPos << ", world pos: " << getWorldPos() << ", event pos: " << pos );

		return true;
	}

	bool touchesMoved( const app::TouchEvent &event )		override
	{
		mLocalTouchColor = Color( 0.7f, 0.3f, 0.0f );

		vec2 pos = event.getTouches().front().getPos();
		mLocalTouchPos = toLocal( pos );
//		CI_LOG_V( getLabel() << " local pos: " << mLocalTouchPos << ", world pos: " << getWorldPos() << ", event pos: " << pos );

		return true;
	}

	bool touchesEnded( const app::TouchEvent &event )		override
	{
		mLocalTouchColor = Color::gray( 0.3f );

		vec2 pos = event.getTouches().front().getPos();
		mLocalTouchPos = toLocal( pos );
//		CI_LOG_V( getLabel() << " local pos: " << mLocalTouchPos << ", world pos: " << getWorldPos() << ", event pos: " << pos );

		return true;
	}

	void draw() override
	{
		RectView::draw();

		gl::ScopedColor colorScope( mLocalTouchColor );
		gl::drawSolidCircle( mLocalTouchPos, 10.0f );
	}

	vec2 mLocalTouchPos;
	Color mLocalTouchColor = Color::gray( 0.3f );
};

ScrollViewTest::ScrollViewTest()
	: SuiteView()
{
	// Free scrolling ScrollView
	mScrollView = make_shared<view::ScrollView>();
//	mScrollView->setClipEnabled( false );
	mScrollView->getContentView()->getBackground()->setColor( Color( 0.15f, 0, 0 ) );

	auto scrollBorder = make_shared<view::StrokedRectView>();
	scrollBorder->setFillParentEnabled();
	scrollBorder->setColor( ColorA( 0.9f, 0.5f, 0.0f, 0.7f ) );
	mScrollView->addSubview( scrollBorder );

	// add some content views:
	auto custom = make_shared<CustomView>( Rectf( 40, 60, 180, 160 ) );

	auto button = make_shared<view::Button>( Rectf( 200, 60, 300, 100 ) );
	button->setTitle( "tap me" );
	button->getSignalPressed().connect( [] { CI_LOG_V( "button pressed" ); } );
	button->getSignalReleased().connect( [] { CI_LOG_V( "button released" ); } );

	auto imageView = make_shared<view::ImageView>( Rectf( 40, 200, 600, 600 ) );
	imageView->getBackground()->setColor( Color( 0, 0.2f, 0 ) );

	fs::path imageFilePath = app::getAssetPath( "images/monkey_hitchhike.jpg" );
	try {
		CI_LOG_I( "loading image view.." );
		auto image = loadImage( loadFile( imageFilePath ) );
		auto tex = gl::Texture::create( image );
		imageView->setTexture( tex );
		imageView->setSize( tex->getSize() );
		CI_LOG_I( "complete" );
	}
	catch( std::exception &exc ) {
		CI_LOG_EXCEPTION( "failed to load image at path: " << imageFilePath, exc );
	}

	mScrollView->addContentViews( { custom, imageView, button } );

	// Horizontal paging ScrollView
	mHorizontalPager = make_shared<view::PagingScrollView>();
	mHorizontalPager->setPageMargin( vec2( 4, 0 ) );
	mHorizontalPager->getBackground()->setColor( Color( 0.8f, 0.4f, 0 ) );
	const size_t numHorizontalPages = 4;
	for( size_t i = 0; i < numHorizontalPages; i++ ) {
		auto label = make_shared<view::Label>( Rectf( 40, 100, 140, 130 ) );
		label->setFontSize( 36 );
		label->setText( "Page " + to_string( i ) );
		label->setTextColor( Color::white() );
		label->getBackground()->setColor( Color( 0.2f, (float)i / (float)numHorizontalPages, 0 ) );
		mHorizontalPager->addContentView( label );
	}

	// Vertical paging ScrollView
	mVerticalPager = make_shared<view::PagingScrollView>();
	mVerticalPager->setAxis( view::PagingScrollView::Axis::VERTICAL );
	mVerticalPager->setPageMargin( vec2( 0, 4 ) );
	mVerticalPager->getBackground()->setColor( Color( 0.8f, 0.4f, 0 ) );
	const size_t numVerticalPages = 10;
	for( size_t i = 0; i < numVerticalPages; i++ ) {
		auto label = make_shared<view::Label>( Rectf( 40, 100, 140, 130 ) );
		label->setFontSize( 36 );
		label->setText( "Page " + to_string( i ) );
		label->setTextColor( Color::white() );
		label->getBackground()->setColor( Color( 0.2f, (float)i / (float)numVerticalPages, 0 ) );
		mVerticalPager->addContentView( label );
	}

	addSubviews( { mScrollView, mHorizontalPager, mVerticalPager } );

	connectKeyDown( signals::slot( this, &ScrollViewTest::keyEvent ) );
}

void ScrollViewTest::layout()
{
	auto rect = Rectf( PADDING, PADDING, getWidth() / 2.0f - PADDING, getHeight() - PADDING );
	mScrollView->setBounds( rect );

	auto halfRect = Rectf( getWidth() / 2.0f + PADDING, PADDING, getWidth() - PADDING, getHeight() / 2.0f - PADDING );
	mHorizontalPager->setBounds( halfRect );

	halfRect += vec2( 0, PADDING + halfRect.getHeight() );
	mVerticalPager->setBounds( halfRect );
}


void ScrollViewTest::keyEvent( app::KeyEvent &event )
{
	switch( event.getCode() ) {
		case app::KeyEvent::KEY_p: {
			// TODO: make sure ScrollView's content views get printed too
			printHeirarchy( app::console() );
			break;
		}
		case app::KeyEvent::KEY_c: {
			mScrollView->setClipEnabled( ! mScrollView->isClipEnabled() );
		}
	}


}

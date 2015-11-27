#include "LayerTest.h"

#include "cinder/app/App.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"
#include "cinder/Log.h"

using namespace std;
using namespace ci;
using namespace mason;

const float PADDING = 50;

LayerTest::LayerTest()
	: SuiteView()
{
	mContainerView = make_shared<View>();
	mContainerView->getBackground()->setColor( Color::gray( 0.75f ) );
	mContainerView->setLabel( "container" );

	Rectf childBounds = Rectf( 30, 30, 300, 100 );

//	mBorderView = make_shared<ui::StrokedRectView>();
//	mBorderView->setColor( ColorA( "red", 1 ) );
//	mContainerView->addSubview( mBorderView );

	const vec2 labelSize = { 200, 200 };

	mLabel1 = make_shared<ui::Label>();
	mLabel1->setText( "1" );
	mLabel1->setSize( labelSize );
	mLabel1->setFontSize( 56 );
	mLabel1->setAlignment( ui::TextAlignment::CENTER );
	mLabel1->setTextColor( Color::white() );
	mLabel1->getBackground()->setColor( Color( 0.8f, 0, 0 ) );

	mLabel2 = make_shared<ui::Label>();
	mLabel2->setText( "2" );
	mLabel2->setSize( labelSize );
	mLabel2->setFontSize( 56 );
	mLabel2->setAlignment( ui::TextAlignment::CENTER );
	mLabel2->setTextColor( Color::white() );
	mLabel2->getBackground()->setColor( Color( 0, 0.8f, 0 ) );

	mLabel3 = make_shared<ui::Label>();
	mLabel3->setText( "3" );
	mLabel3->setSize( labelSize );
	mLabel3->setFontSize( 56 );
	mLabel3->setAlignment( ui::TextAlignment::CENTER );
	mLabel3->setTextColor( Color::white() );
	mLabel3->getBackground()->setColor( Color( 0, 0, 0.8f ) );

	mContainerView->addSubviews( { mLabel1, mLabel2, mLabel3 } );
	addSubview( mContainerView );

	connectKeyDown( signals::slot( this, &LayerTest::keyEvent ) );
}

void LayerTest::layout()
{
	mContainerView->setBounds( Rectf( PADDING, PADDING, getWidth() - PADDING, getHeight() - PADDING ) );

	mLabel1->setPos( { 200, 100 } );
	mLabel2->setPos( { 450, 100 } );
	mLabel3->setPos( { 325, 200 } );
}

void LayerTest::keyEvent( app::KeyEvent &event )
{
	switch( event.getCode() ) {
		case app::KeyEvent::KEY_SPACE: {
			app::timeline().apply( mContainerView->animPos(), vec2( randFloat( -PADDING, PADDING ), randFloat( -PADDING, PADDING ) ), 1.0f, EaseOutExpo() );
			break;
		}
		case app::KeyEvent::KEY_a: {
			float nextAlpha = mContainerView->getAlpha() > 0.01f ? 0.0f : 1.0f;
			app::timeline().apply( mContainerView->animAlpha(), nextAlpha, 2.0f, EaseInOutExpo() );
			break;
		}
		case app::KeyEvent::KEY_t: {
			mContainerView->getLayer()->setRenderTransparencyToFrameBufferEnabled( ! mContainerView->getLayer()->isRenderTransparencyToFrameBufferEnabled() );
			CI_LOG_I( "container view renders transparency to framebuffer: " << boolalpha << mContainerView->getLayer()->isRenderTransparencyToFrameBufferEnabled() << dec );
			break;
		}
	}
}

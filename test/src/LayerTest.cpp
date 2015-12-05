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

	mLabelA = make_shared<ui::Label>();
	mLabelA->setText( "A" );
	mLabelA->setLabel( "Label A" );
	mLabelA->setSize( labelSize );
	mLabelA->setFontSize( 56 );
	mLabelA->setAlignment( ui::TextAlignment::CENTER );
	mLabelA->setTextColor( Color::white() );
	mLabelA->getBackground()->setColor( Color( 0.8f, 0, 0 ) );

	mLabelB = make_shared<ui::Label>();
	mLabelB->setText( "B" );
	mLabelB->setLabel( "Label B" );
	mLabelB->setSize( labelSize );
	mLabelB->setFontSize( 56 );
	mLabelB->setAlignment( ui::TextAlignment::CENTER );
	mLabelB->setTextColor( Color::white() );
	mLabelB->getBackground()->setColor( Color( 0, 0.8f, 0 ) );

	mLabelC = make_shared<ui::Label>();
	mLabelC->setText( "C" );
	mLabelC->setLabel( "Label C" );
	mLabelC->setSize( labelSize );
	mLabelC->setFontSize( 56 );
	mLabelC->setAlignment( ui::TextAlignment::CENTER );
	mLabelC->setTextColor( Color::white() );
	mLabelC->getBackground()->setColor( Color( 0, 0, 0.8f ) );
	mLabelC->setAlpha( 0.5f );

	mLabelD = make_shared<ui::Label>();
	mLabelD->setText( "D" );
	mLabelD->setLabel( "Label D" );
	mLabelD->setSize( vec2( 60, 60 ) );
	mLabelD->setFontSize( 24 );
	mLabelD->setAlignment( ui::TextAlignment::CENTER );
	mLabelD->setTextColor( Color::black() );
	mLabelD->getBackground()->setColor( Color( 0.8f, 0.8f, 0 ) );

	mLabelC->addSubview( mLabelD );
	mContainerView->addSubviews( { mLabelA, mLabelB, mLabelC } );
	addSubview( mContainerView );

	connectKeyDown( signals::slot( this, &LayerTest::keyEvent ) );
}

void LayerTest::layout()
{
	mContainerView->setBounds( Rectf( PADDING, PADDING, getWidth() - PADDING, getHeight() - PADDING ) );

	mLabelA->setPos( { 200, 100 } );
	mLabelB->setPos( { 450, 100 } );
	mLabelC->setPos( { 325, 200 } );

	mLabelD->setPos( { 12, 70 } );
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
			mContainerView->setRenderTransparencyToFrameBufferEnabled( ! mContainerView->isRenderTransparencyToFrameBufferEnabled() );
			CI_LOG_I( "container view renders transparency to framebuffer: " << boolalpha << mContainerView->isRenderTransparencyToFrameBufferEnabled() << dec );
			break;
		}
	}
}

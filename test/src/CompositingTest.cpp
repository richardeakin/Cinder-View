#include "CompositingTest.h"

#include "ui/Slider.h"

#include "cinder/app/App.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"
#include "cinder/Log.h"

using namespace std;
using namespace ci;
using namespace mason;

const float PADDING = 50;

CompositingTest::CompositingTest()
	: SuiteView()
{
	auto container = make_shared<ui::StrokedRectView>();
	container->setColor( Color::white() );
	container->setLineWidth( 6 );
	container->setLabel( "container" );
	mContainerView = container;

	const vec2 labelSize = { 200, 200 };
	const float fontSizeBig = 64;

	mLabelA = make_shared<ui::Label>();
	mLabelA->setText( "A" );
	mLabelA->setLabel( "Label A" );
	mLabelA->setSize( labelSize );
	mLabelA->setFontSize( fontSizeBig );
	mLabelA->setAlignment( ui::TextAlignment::CENTER );
	mLabelA->setTextColor( Color::white() );
	mLabelA->getBackground()->setColor( Color( 0, 0, 0.8f ) );

	mLabelB = make_shared<ui::Label>();
	mLabelB->setText( "B" );
	mLabelB->setLabel( "Label B" );
	mLabelB->setSize( labelSize );
	mLabelB->setFontSize( fontSizeBig );
	mLabelB->setAlignment( ui::TextAlignment::CENTER );
	mLabelB->setTextColor( Color::white() );
	mLabelB->getBackground()->setColor( Color( 0, 0.8f, 0 ) );

	mLabelC = make_shared<ui::Label>();
	mLabelC->setText( "C" );
	mLabelC->setLabel( "Label C" );
	mLabelC->setSize( labelSize );
	mLabelC->setFontSize( fontSizeBig );
	mLabelC->setAlignment( ui::TextAlignment::CENTER );
	mLabelC->setTextColor( Color::white() );
	mLabelC->getBackground()->setColor( Color( 0.8f, 0, 0 ) );
	mLabelC->setAlpha( 0.5f );

	mLabelD = make_shared<ui::Label>();
	mLabelD->setText( "D" );
	mLabelD->setLabel( "Label D" );
	mLabelD->setSize( vec2( 60, 60 ) );
	mLabelD->setFontSize( 30 );
	mLabelD->setAlignment( ui::TextAlignment::CENTER );
	mLabelD->setTextColor( Color::white() );
	mLabelD->getBackground()->setColor( Color( 0.8f, 0.8f, 0 ) );

	{
		auto alphaSlider = make_shared<ui::HSlider>( Rectf( 10, 10, 150, 40 ) );
		alphaSlider->setTitle( "container alpha" );
		alphaSlider->setValue( mContainerView->getAlpha() );
		alphaSlider->getBackground()->setColor( Color::gray( 0.15f ) );
		auto alphaSliderPtr = alphaSlider.get(); // avoiding cyclical strong reference, slider is also owned by parent view
		alphaSlider->getSignalValueChanged().connect( [this, alphaSliderPtr] {
			mContainerView->setAlpha( alphaSliderPtr->getValue() );
		} );

		mContainerView->addSubview( alphaSlider );
	}
	{
		auto alphaSlider = make_shared<ui::HSlider>( Rectf( 10, 50, 150, 80 ) );
		alphaSlider->setTitle( "C alpha" );
		alphaSlider->setValue( mLabelC->getAlpha() );
		alphaSlider->getBackground()->setColor( Color::gray( 0.15f ) );
		auto alphaSliderPtr = alphaSlider.get(); // avoiding cyclical strong reference, slider is also owned by parent view
		alphaSlider->getSignalValueChanged().connect( [this, alphaSliderPtr] {
			mLabelC->setAlpha( alphaSliderPtr->getValue() );
		} );

		mContainerView->addSubview( alphaSlider );
	}

	mLabelC->addSubview( mLabelD );
	mContainerView->addSubviews( { mLabelA, mLabelB, mLabelC } );
	addSubview( mContainerView );

	connectKeyDown( signals::slot( this, &CompositingTest::keyEvent ) );
}

void CompositingTest::layout()
{
	mContainerView->setBounds( Rectf( PADDING, PADDING, getWidth() - PADDING, getHeight() - PADDING ) );
	CI_LOG_I( "mContainerView bounds: " << mContainerView->getBounds() );

	mLabelA->setPos( { 200, 100 } );
	mLabelB->setPos( { 450, 100 } );
	mLabelC->setPos( { 325, 200 } );

	mLabelD->setPos( { 12, 70 } );
}

void CompositingTest::keyEvent( app::KeyEvent &event )
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

#include "CompositingTest.h"

#include "cinder/app/App.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"
#include "cinder/Log.h"

using namespace std;
using namespace ci;
using namespace mason;

const float PADDING = 50;
const vec2 LABEL_SIZE = { 200, 200 };

CompositingTest::CompositingTest()
	: SuiteView()
{
	mContainerView = make_shared<ui::StrokedRectView>();
	mContainerView->setLabel( "container" );
	mContainerView->setColor( Color::white() );
	mContainerView->setLineWidth( 6 ); // test that FrameBuffer will account for line width drawing outside of View bounds by half line width
	mContainerView->getBackground()->setColor( Color::black() );

	const float fontSizeBig = 64;

	mLabelA = make_shared<ui::Label>();
	mLabelA->setText( "A" );
	mLabelA->setLabel( "Label A" );
	mLabelA->setFontSize( fontSizeBig );
	mLabelA->setAlignment( ui::TextAlignment::CENTER );
	mLabelA->setTextColor( Color::white() );
	mLabelA->getBackground()->setColor( Color( 0, 0, 1 ) );

	mLabelB = make_shared<ui::Label>();
	mLabelB->setText( "B" );
	mLabelB->setLabel( "Label B" );
	mLabelB->setFontSize( fontSizeBig );
	mLabelB->setAlignment( ui::TextAlignment::CENTER );
	mLabelB->setTextColor( Color::white() );
	mLabelB->getBackground()->setColor( Color( 0, 1, 0 ) );

	mLabelC = make_shared<ui::Label>();
	mLabelC->setText( "C" );
	mLabelC->setLabel( "Label C" );
	mLabelC->setFontSize( fontSizeBig );
	mLabelC->setAlignment( ui::TextAlignment::CENTER );
	mLabelC->setTextColor( Color::white() );
	mLabelC->getBackground()->setColor( Color( 1, 0, 0 ) );
	mLabelC->setAlpha( 0.5f );

	mLabelD = make_shared<ui::Label>();
	mLabelD->setText( "D" );
	mLabelD->setLabel( "Label D" );
	mLabelD->setFontSize( 30 );
	mLabelD->setAlignment( ui::TextAlignment::CENTER );
	mLabelD->setTextColor( Color::white() );
	mLabelD->getBackground()->setColor( Color( 1, 1, 0 ) );
	mLabelD->setAlpha( 0.75f );

	mLabelC->addSubview( mLabelD );
	mContainerView->addSubviews( { mLabelA, mLabelB, mLabelC } );
	addSubview( mContainerView );

	// add controls to suite view
	Rectf sliderRect =  Rectf( 10, 10, 150, 40 );
	{
		auto slider = make_shared<ui::HSlider>( sliderRect );
		slider->setTitle( "container alpha" );
		slider->setValue( mContainerView->getAlpha() );
		slider->getBackground()->setColor( Color::gray( 0.15f ) );
		auto alphaSliderPtr = slider.get(); // avoiding cyclical strong reference, slider is also owned by parent view
		slider->getSignalValueChanged().connect( [this, alphaSliderPtr] {
			mContainerView->setAlpha( alphaSliderPtr->getValue() );
		} );

		addSubview( slider );
	}
	sliderRect += vec2( 0, 40 );
	{
		auto slider = make_shared<ui::HSlider>( sliderRect );
		slider->setTitle( "C alpha" );
		slider->setValue( mLabelC->getAlpha() );
		slider->getBackground()->setColor( Color::gray( 0.15f ) );
		auto alphaSliderPtr = slider.get(); // avoiding cyclical strong reference, slider is also owned by parent view
		slider->getSignalValueChanged().connect( [this, alphaSliderPtr] {
			mLabelC->setAlpha( alphaSliderPtr->getValue() );
		} );

		addSubview( slider );
	}
	sliderRect += vec2( 0, 40 );
	{
		auto slider = make_shared<ui::HSlider>( sliderRect );
		slider->setTitle( "D alpha" );
		slider->setValue( mLabelD->getAlpha() );
		slider->getBackground()->setColor( Color::gray( 0.15f ) );
		auto alphaSliderPtr = slider.get(); // avoiding cyclical strong reference, slider is also owned by parent view
		slider->getSignalValueChanged().connect( [this, alphaSliderPtr] {
			mLabelD->setAlpha( alphaSliderPtr->getValue() );
		} );

		addSubview( slider );
	}
	sliderRect += vec2( 0, 40 );
	{
		auto slider = make_shared<ui::HSlider>( sliderRect );
		slider->setTitle( "C scale" );
		slider->setValue( 1 );
		slider->setMin( 0.1f );
		slider->setMax( 3 );
		slider->getBackground()->setColor( Color::gray( 0.15f ) );
		auto alphaSliderPtr = slider.get(); // avoiding cyclical strong reference, slider is also owned by parent view
		slider->getSignalValueChanged().connect( [this, alphaSliderPtr] {
			vec2 c = mLabelC->getCenter();
			vec2 h = ( LABEL_SIZE / 2.0f ) * alphaSliderPtr->getValue();
			Rectf bounds = Rectf( c.x - h.x, c.y - h.y, c.x + h.x, c.y + h.y );
			mLabelC->setBounds( bounds );
			//CI_LOG_I( "set bounds to: " << bounds );
		} );

		addSubview( slider );
	}

}

void CompositingTest::layout()
{
	mContainerView->setBounds( Rectf( PADDING, PADDING, getWidth() - PADDING, getHeight() - PADDING ) );
	CI_LOG_I( "mContainerView bounds: " << mContainerView->getBounds() );

	mLabelA->setPos( { 200, 100 } );
	mLabelB->setPos( { 450, 100 } );
	mLabelC->setPos( { 325, 200 } );

	mLabelA->setSize( LABEL_SIZE );
	mLabelB->setSize( LABEL_SIZE );
	mLabelC->setSize( LABEL_SIZE );

	mLabelD->setPos( { 12, 70 } );
	mLabelD->setSize( vec2( 60, 60 ) );
}

void CompositingTest::update()
{
}

bool CompositingTest::keyDown( app::KeyEvent &event )
{
	bool handled = true;

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
		case app::KeyEvent::KEY_o: {
			float nextBorderWidth = randFloat( 1, 30 );
			app::timeline().apply( mContainerView->getLineWidthAnim(), nextBorderWidth, 0.6f, EaseOutExpo() );
			break;
		}
		case app::KeyEvent::KEY_s: {
			vec2 nextLabelSize = LABEL_SIZE * vec2( randFloat( 0.5f, 2.0f ), randFloat( 0.5f, 2.0f ) );
			app::timeline().apply( mLabelC->animSize(), nextLabelSize, 0.6f, EaseInOutExpo() );
			break;
		}
		default:
			handled = false;
	}

	return handled;
}

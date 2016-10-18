#include "LayoutTests.h"
#include "ui/Label.h"
#include "ui/Layout.h"

#include "cinder/Log.h"

using namespace std;
using namespace ci;

LayoutTests::LayoutTests()
{
	const float padding = 6;
	const Rectf margin = Rectf( 6, 6, 6, 6 );
	const Color backgroundColor = Color( 0.85f, 0.5f, 0 );

	mVerticalLayout = make_shared<ui::VerticalLayout>();
	mVerticalLayout->setPadding( padding );
	mVerticalLayout->setMargin( margin );

	mVerticalGroupView = make_shared<ui::View>();
	mVerticalGroupView->setLayout( mVerticalLayout );
	mVerticalGroupView->setLabel( "vertical group" );
	mVerticalGroupView->getBackground()->setColor( backgroundColor );
	addSubview( mVerticalGroupView );
	addLabels( mVerticalGroupView, 6 );

	mHorizontalLayout = make_shared<ui::HorizontalLayout>();
	mHorizontalLayout->setPadding( padding );
	mHorizontalLayout->setMargin( margin );

	mHorizontalGroupView = make_shared<ui::View>();
	mHorizontalGroupView->setLayout( mHorizontalLayout );
	mHorizontalGroupView->setLabel( "horizontal group" );
	mHorizontalGroupView->getBackground()->setColor( backgroundColor );
	addSubview( mHorizontalGroupView );
	addLabels( mHorizontalGroupView, 6 );
}

void LayoutTests::addLabels( const ui::ViewRef &view, size_t count )
{
	for( size_t i = 0; i < count; i++ ) {
		auto label = make_shared<ui::Label>();
		label->setFontSize( 24 );
		label->setText( "Label " + to_string( i ) );
		label->setAlignment( ui::TextAlignment::CENTER );
		label->getBackground()->setColor( Color( CM_HSV, 1.0f - (float)i * 0.2f / (float)count, 1.0f, 0.75f ) );
		label->setSize( vec2( 120, 40 ) );
		view->addSubview( label );

		mInitialSizes[label] = label->getSize();
	}
}

void LayoutTests::layout()
{
	mHorizontalGroupView->setPos( vec2( 40, 40 ) );
	mHorizontalGroupView->setSize( vec2( 800, 80 ) );

	mVerticalGroupView->setPos( vec2( 40, 140 ) );
	mVerticalGroupView->setSize( vec2( 300, 300 ) );
}

bool LayoutTests::keyDown( ci::app::KeyEvent &event )
{
	// Reset sizes to initial setting.
	for( auto& vp : mInitialSizes ) {
		vp.first->setSize( vp.second );
	}

	bool handled = true;
	if( event.getCode() == ci::app::KeyEvent::KEY_m ) {
		auto nextMode = ui::LinearLayout::Mode( ( (int)mVerticalLayout->getMode() + 1 ) % (int)ui::LinearLayout::Mode::NUM_MODES );
		CI_LOG_I( "next mode (vertical): " << (int)nextMode );
		mVerticalLayout->setMode( nextMode );
		mVerticalGroupView->setNeedsLayout(); // TODO: this should happen automatically when updating the mode

		mHorizontalLayout->setMode( nextMode );
		mHorizontalGroupView->setNeedsLayout();
	}
	if( event.getCode() == ci::app::KeyEvent::KEY_a ) {
		auto nextAlignment = ui::Alignment( ((int)mVerticalLayout->getAlignment() + 1) % (int)ui::Alignment::NUM_ALIGNMENTS );
		CI_LOG_I( "next mode (vertical): " << (int)nextAlignment );
		mVerticalLayout->setAlignment( nextAlignment );
		mVerticalGroupView->setNeedsLayout(); // TODO: this should happen automatically when updating alignment

		mHorizontalLayout->setAlignment( nextAlignment );
		mHorizontalGroupView->setNeedsLayout();
	}
	else
		handled = false;

	return handled;
}

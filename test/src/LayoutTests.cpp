#include "LayoutTests.h"
#include "ui/Label.h"
#include "ui/Layout.h"

#include "cinder/Log.h"

using namespace std;
using namespace ci;

LayoutTests::LayoutTests()
{
	mVerticalLayout = make_shared<ui::VerticalLayout>();
	mVerticalLayout->setPadding( 6 );
	mVerticalLayout->setMargin( Rectf( 10, 10, 10, 10 ) );

	mVerticalGroupView = make_shared<ui::View>();
	mVerticalGroupView->setLayout( mVerticalLayout );
	mVerticalGroupView->setLabel( "vertical group" );
	mVerticalGroupView->getBackground()->setColor( Color( 0.85f, 0.5f, 0 ) );

	addSubview( mVerticalGroupView );

	const size_t numVerticalPages = 6;
	for( size_t i = 0; i < numVerticalPages; i++ ) {
		auto label = make_shared<ui::Label>();
		label->setFontSize( 24 );
		label->setText( "Label " + to_string( i ) );
		label->getBackground()->setColor( Color( CM_HSV, 1.0f - (float)i * 0.2f / (float)numVerticalPages, 1.0f, 0.75f ) );
		label->setSize( vec2( 180, 40 ) );
		mVerticalGroupView->addSubview( label );

		mInitialSizes[static_pointer_cast<ui::View>( label )] = label->getSize();
	}
}

void LayoutTests::layout()
{
	mVerticalGroupView->setPos( vec2( 40, 40 ) );
	mVerticalGroupView->setSize( vec2( 300, 500 ) );
}

bool LayoutTests::keyDown( ci::app::KeyEvent &event )
{
	// Reset sizes to initial setting
	for( auto& view : mVerticalGroupView->getSubviews() ) {
		view->setSize( mInitialSizes[view] );
	}
	bool handled = true;
	if( event.getCode() == ci::app::KeyEvent::KEY_v ) {
		auto nextMode = ui::LinearLayout::Mode( ( (int)mVerticalLayout->getMode() + 1 ) % (int)ui::LinearLayout::Mode::NumModes );
		CI_LOG_I( "next mode (vertical): " << (int)nextMode );
		mVerticalLayout->setMode( nextMode );
		mVerticalGroupView->setNeedsLayout(); // TODO: this should happen automatically when updating the mode
	}
	if( event.getCode() == ci::app::KeyEvent::KEY_a ) {
		auto nextAlignment = ui::Alignment( ((int)mVerticalLayout->getAlignment() + 1) % (int)ui::Alignment::NumAlignments );
		CI_LOG_I( "next mode (vertical): " << (int)nextAlignment );
		mVerticalLayout->setAlignment( nextAlignment );
		mVerticalGroupView->setNeedsLayout(); // TODO: this should happen automatically when updating the mode
	}
	else
		handled = false;

	return handled;
}

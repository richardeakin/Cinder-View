#include "LayoutTests.h"
#include "ui/Label.h"
#include "ui/Layout.h"

using namespace std;
using namespace ci;

LayoutTests::LayoutTests()
{
	mVerticalGroup = make_shared<ui::View>();
	mVerticalGroup->setLayout( make_shared<ui::VerticalLayout>() );
	mVerticalGroup->setLabel( "vertical group" );
	mVerticalGroup->getBackground()->setColor( Color( 0.85f, 0.5f, 0 ) );

	addSubview( mVerticalGroup );

	const size_t numVerticalPages = 6;
	for( size_t i = 0; i < numVerticalPages; i++ ) {
		auto label = make_shared<ui::Label>();
		label->setFontSize( 24 );
		label->setText( "Label " + to_string( i ) );
		label->getBackground()->setColor( Color( CM_HSV, 1.0f - (float)i * 0.2f / (float)numVerticalPages, 1.0f, 0.75f ) );

		mVerticalGroup->addSubview( label );
	}
}

void LayoutTests::layout()
{
	mVerticalGroup->setPos( vec2( 40, 40 ) );
	mVerticalGroup->setSize( vec2( 200, 300 ) );
}

bool LayoutTests::keyDown( ci::app::KeyEvent &event )
{
	return false;
}

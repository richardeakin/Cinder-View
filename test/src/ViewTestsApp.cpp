#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

#include "ui/Suite.h"

#include "BasicViewTests.h"
#include "ControlsTest.h"
#include "CompositingTest.h"
#include "FilterTest.h"
#include "MultiTouchTest.h"
#include "ScrollTests.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ViewTestsApp : public App {
  public:
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;

	void drawLayerBorders();

	ui::SuiteRef	mTestSuite;

	bool    mDrawLayerBorders = false;
};

void ViewTestsApp::setup()
{
	mTestSuite = make_shared<ui::Suite>();

	mTestSuite->registerSuiteView<BasicViewTests>( "basic" );
	mTestSuite->registerSuiteView<CompositingTest>( "compositing" );
	mTestSuite->registerSuiteView<ControlsTest>( "controls" );
	mTestSuite->registerSuiteView<MultiTouchTest>( "multitouch" );
	mTestSuite->registerSuiteView<ScrollTests>( "scroll" );
	mTestSuite->registerSuiteView<FilterTest>( "filters" );

	// TODO: this doesn't cover the case of calling Suite::select() directly - should probably add new signal that ties to both Selector and that
	mTestSuite->getSelector()->getSignalValueChanged().connect( [this] {
		CI_LOG_I( "selected test index: " << mTestSuite->getCurrentIndex() << ", key: " << mTestSuite->getCurrentKey() );
	} );

	mTestSuite->select( 5 );
}

void ViewTestsApp::keyDown( app::KeyEvent event )
{
	switch( event.getCode() ) {
		case app::KeyEvent::KEY_p:
			mTestSuite->getGraph()->printHeirarchy( app::console() );
		break;
		case app::KeyEvent::KEY_b:
			mDrawLayerBorders = ! mDrawLayerBorders;
		break;
	}
}

void ViewTestsApp::update()
{
	mTestSuite->update();
}

void ViewTestsApp::draw()
{
	gl::clear( Color( 0, 0.1f, 0.15f ) );

	mTestSuite->draw();

	if( mDrawLayerBorders )
		drawLayerBorders();

	CI_CHECK_GL();
}

void ViewTestsApp::drawLayerBorders()
{
	auto graph = mTestSuite->getGraph();
	auto ren = graph->getRenderer();

	ren->pushColor( Color( 0.75f, 0.5f, 0 ) );
	for( auto &layer : graph->getLayers() ) {
		Rectf layerBorder = layer->getBoundsWorld();
		ren->drawStrokedRect( layerBorder, 2);
	}
	ren->popColor();
}

CINDER_APP( ViewTestsApp, RendererGl( RendererGl::Options().msaa( 8 ) ), []( App::Settings *settings ) {
//	settings->setWindowPos( 0, 0 );
	settings->setWindowSize( 1000, 650 );

//	settings->setMultiTouchEnabled();

	// move app to macbook display
	for( const auto &display : Display::getDisplays() ) {
		CI_LOG_I( "display name: " << display->getName() );
		if( display->getName() == "Color LCD" ) {
			settings->setDisplay( display );
		}
		else if( display->getName() == "Generic PnP Monitor" ) {
			// gechic 1303i 13"touch display
			settings->setDisplay( display );
			//settings->setFullScreen( true );
		}
	}
} )

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

#include "ui/Suite.h"

#include "BasicViewTests.h"
#include "ControlsTest.h"
#include "CompositingTest.h"
#include "FilterTest.h"
#include "LayoutTests.h"
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
	mTestSuite->registerSuiteView<LayoutTests>( "layout" );
	mTestSuite->registerSuiteView<MultiTouchTest>( "multitouch" );
	mTestSuite->registerSuiteView<ScrollTests>( "scroll" );
	mTestSuite->registerSuiteView<FilterTest>( "filters" );

	// TODO: this doesn't cover the case of calling Suite::select() directly - should probably add new signal that ties to both Selector and that
	mTestSuite->getSelector()->getSignalValueChanged().connect( [this] {
		CI_LOG_I( "selected test index: " << mTestSuite->getCurrentIndex() << ", key: " << mTestSuite->getCurrentKey() );
	} );

	mTestSuite->select( 3 );
}

void ViewTestsApp::keyDown( app::KeyEvent event )
{
	if( event.isControlDown() ) {
		if( event.getChar() == 'r' ) {
			CI_LOG_I( "reloading.." );
			mTestSuite->reload();
		}
	}

	switch( event.getCode() ) {
		case app::KeyEvent::KEY_p:
			mTestSuite->getGraph()->printHierarchy( app::console() );
		break;
		case app::KeyEvent::KEY_b:
			mDrawLayerBorders = ! mDrawLayerBorders;
		break;
	}
}

void ViewTestsApp::update()
{
	size_t numFrameBuffers = mTestSuite->getGraph()->getRenderer()->getNumFrameBuffersCached();
	mTestSuite->getInfoLabel()->setRow( 1, { "num FrameBuffers: ", to_string( numFrameBuffers ) } );

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

void prepareSettings( app::App::Settings *settings )
{
	//settings->setWindowPos( 0, 0 );
	settings->setWindowSize( 1000, 650 );

	//settings->setMultiTouchEnabled();

	// move app to secondary display
	if( Display::getDisplays().size() > 1 ) {
		settings->setDisplay( Display::getDisplays()[1] );
	}
}

CINDER_APP( ViewTestsApp, RendererGl( RendererGl::Options().msaa( 8 ) ), prepareSettings )

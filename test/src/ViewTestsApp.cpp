#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

#include "ui/Suite.h"

#include "BasicViewTests.h"
#include "ControlsTest.h"
#include "CompositingTest.h"
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

	ui::SuiteRef	mTestSuite;
};

void ViewTestsApp::setup()
{
	mTestSuite = make_shared<ui::Suite>();

	mTestSuite->registerSuiteView<BasicViewTests>( "basic" );
	mTestSuite->registerSuiteView<ControlsTest>( "controls" );
	mTestSuite->registerSuiteView<CompositingTest>( "compositing" );
	mTestSuite->registerSuiteView<ScrollTests>( "scroll" );

	// TODO: this doesn't cover the case of calling selectTest() directly - should probably add new signal that ties to both Selector and that
	mTestSuite->getSelector()->getSignalValueChanged().connect( [this] {
		CI_LOG_I( "selected test index: " << mTestSuite->getCurrentTestIndex() << ", key: " << mTestSuite->getCurrentTestKey() );
	} );

	mTestSuite->selectTest( 0 );
}

void ViewTestsApp::keyDown( app::KeyEvent event )
{
	if( event.getChar() == 'p' ) {
		mTestSuite->getGraph()->printHeirarchy( app::console() );
	}
}

void ViewTestsApp::update()
{
	mTestSuite->update();
}

void ViewTestsApp::draw()
{
	gl::clear();

	mTestSuite->draw();

	CI_CHECK_GL();
}

CINDER_APP( ViewTestsApp, RendererGl( RendererGl::Options().msaa( 8 ) ), []( App::Settings *settings ) {
	settings->setWindowPos( 0, 0 );
	settings->setWindowSize( 960, 565 );
} )

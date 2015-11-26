#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

#include "ui/Suite.h"

#include "BasicViewTests.h"
#include "ControlsTest.h"
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
	mTestSuite->registerSuiteView<ScrollTests>( "scroll" );

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

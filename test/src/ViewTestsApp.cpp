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

#include "glm/gtc/epsilon.hpp"
#include "fmt/format.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const vec2 INFO_ROW_SIZE    = vec2( 250, 20 );
const float PADDING         = 6;

class ViewTestsApp : public App {
  public:
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;

	void updateUI();
	void resizeInfoLabel();
	void drawViewBorders();
	void drawLayerBorders();

	ui::SuiteRef		mTestSuite;
	ui::LabelGridRef    mInfoLabel;

	bool	mDrawViewBorders = false;
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

	mTestSuite->select( 1 );

	mInfoLabel = make_shared<ui::LabelGrid>();
	mInfoLabel->setTextColor( Color::white() );
	mInfoLabel->getBackground()->setColor( ColorA::gray( 0, 0.3f ) );

	mTestSuite->getGraph()->addSubview( mInfoLabel );
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
			CI_LOG_I( "TestSuite View hierarchy\n: " << ui::printHierarchyToString( mTestSuite->getGraph() ) );
		break;
		case app::KeyEvent::KEY_l:
			mDrawLayerBorders = ! mDrawLayerBorders;
		break;
		case app::KeyEvent::KEY_v:
			mDrawViewBorders = ! mDrawViewBorders;
		break;
	}
}

void ViewTestsApp::update()
{
	mTestSuite->update();
	updateUI();
}

void ViewTestsApp::updateUI()
{
	mInfoLabel->setRow( 0, { "fps:",  fmt::format( "{}", getAverageFps() ) } );

	size_t numFrameBuffers = mTestSuite->getGraph()->getRenderer()->getNumFrameBuffersCached();
	mInfoLabel->setRow( 1, { "FrameBuffers: ", to_string( numFrameBuffers ) } );

	if( ! glm::epsilonEqual( INFO_ROW_SIZE.y * mInfoLabel->getNumRows(), mInfoLabel->getHeight(), 0.01f ) )
		resizeInfoLabel();
}

void ViewTestsApp::resizeInfoLabel()
{
	const int numRows = mInfoLabel->getNumRows();
	vec2 windowSize = vec2( app::getWindow()->getSize() );
	vec2 labelSize = { INFO_ROW_SIZE.x, INFO_ROW_SIZE.y * numRows };
	mInfoLabel->setBounds( { windowSize - labelSize - PADDING, windowSize - PADDING } ); // anchor bottom right
}

void ViewTestsApp::draw()
{
	gl::clear( Color( 0, 0.1f, 0.15f ) );

	mTestSuite->draw();

	if( mDrawViewBorders )
		drawViewBorders();
	if( mDrawLayerBorders )
		drawLayerBorders();

	CI_CHECK_GL();
}

void ViewTestsApp::drawViewBorders()
{
	gl::ScopedColor colorScope( 0, 1, 1 );
	ui::traverse( mTestSuite->getGraph(), []( const ui::ViewRef &view ) {
		gl::drawStrokedRect( view->getWorldBounds(), 2 );
	} );
}

void ViewTestsApp::drawLayerBorders()
{
	auto graph = mTestSuite->getGraph();
	auto ren = graph->getRenderer();

	ren->pushColor( Color( 0.75f, 0, 0.75f ) );
	for( auto &layer : graph->getLayers() ) {
		Rectf layerBorder = layer->getBoundsWorld();
		ren->drawStrokedRect( layerBorder, 3 );
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

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
#include "glm/gtx/string_cast.hpp"
#include "fmt/format.h"

using namespace ci;
using namespace ci::app; 
using namespace std;

const bool MULTITOUCH_ENABLED	= 0;
const bool USE_SECONDARY_SCREEN = 1;
const int DEFAULT_TEST			= 1;
const vec2 WINDOW_SIZE			= vec2( 1220, 720 );
const vec2 INFO_ROW_SIZE		= vec2( 250, 20 );


#define LIVEPP_ENABLED 1

#if LIVEPP_ENABLED

#include <windows.h>
#include "../../../../../LivePP/API/LPP_ForceLinkStaticRuntime.h"
#include "../../../../../LivePP/API/LPP_API.h"

#endif

class ViewTestsApp : public App {
  public:
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void resize() override;
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
	bool	mDrawDebugNames = false;
};

void ViewTestsApp::setup()
{
	ui::Graph::EventOptions eventOptions;
#if MULTITOUCH_ENABLED
	eventOptions.mouse( false );
#endif
	mTestSuite = make_shared<ui::Suite>( eventOptions );

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

	mTestSuite->select( DEFAULT_TEST );

	mInfoLabel = make_shared<ui::LabelGrid>();
	mInfoLabel->setTextColor( Color::white() );
	mInfoLabel->getBackground()->setColor( ColorA::gray( 0, 0.3f ) );

	mTestSuite->getGraph()->addSubview( mInfoLabel );
}

void ViewTestsApp::keyDown( app::KeyEvent event )
{
	if( event.isControlDown() ) {
		switch( event.getCode() ) {
			case app::KeyEvent::KEY_r:
				CI_LOG_I( "reloading.." );
				mTestSuite->reload();
			break;
			case app::KeyEvent::KEY_f:
				setFullScreen( ! isFullScreen() );
			break;
			case app::KeyEvent::KEY_v:
				CI_LOG_I( "TestSuite View hierarchy\n: " << ui::printHierarchyToString( mTestSuite->getGraph() ) );
			break;
			case app::KeyEvent::KEY_l:
				mDrawLayerBorders = ! mDrawLayerBorders;
			break;
			case app::KeyEvent::KEY_k:
				mDrawViewBorders = ! mDrawViewBorders;
			break;
			case app::KeyEvent::KEY_n:
				mDrawDebugNames = ! mDrawDebugNames;
			break;
		}
	}

}

void ViewTestsApp::resize()
{
	resizeInfoLabel();
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
	const float padding = 6;

	const int numRows = mInfoLabel->getNumRows();
	vec2 windowSize = vec2( app::getWindow()->getSize() );
	vec2 labelSize = { INFO_ROW_SIZE.x, INFO_ROW_SIZE.y * numRows };
	mInfoLabel->setBounds( { windowSize - labelSize - padding, windowSize - padding } ); // anchor bottom right
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
	static gl::TextureFontRef sTextureFont;
	if( mDrawDebugNames && ! sTextureFont ) {
		sTextureFont = gl::TextureFont::create( Font( "Arial", 18 ) );
	}


	gl::ScopedColor colorScope( 0, 1, 1 );
	ui::traverse( mTestSuite->getGraph(), [this]( const ui::ViewRef &view ) {
		//if( view->isHidden() )
		//	return false;

		gl::drawStrokedRect( view->getWorldBounds(), 2 );

		if( mDrawDebugNames ) {
			auto str = view->getName();
			sTextureFont->drawString(str, view->getWorldPos() + vec2( 2, 2 + sTextureFont->getFont().getAscent() ) );
		}

		return true;
	} );
}

void ViewTestsApp::drawLayerBorders()
{
	static gl::TextureFontRef sTextureFont;
	if( mDrawDebugNames && ! sTextureFont ) {
		sTextureFont = gl::TextureFont::create( Font( "Arial", 18 ) );
	}

	auto graph = mTestSuite->getGraph();
	auto ren = graph->getRenderer();

	ren->pushColor( Color( 0.75f, 0, 0.75f ) );
	for( auto &layer : graph->getLayers() ) {
		Rectf layerBorder = layer->getBoundsWorld();
		ren->drawStrokedRect( layerBorder, 3 );

		if( mDrawDebugNames ) {
			auto view = layer->getRootView();
			auto str = view->getName();
			if( layer->getFrameBuffer() ) {
				str += ", FrameBuffer size: " + glm::to_string( layer->getFrameBuffer()->getSize() );
			}
			sTextureFont->drawString(str, view->getWorldPos() + vec2( 2, 2 + sTextureFont->getFont().getAscent() ) );
		}
	}
	ren->popColor();
}

void prepareSettings( app::App::Settings *settings )
{
#if LIVEPP_ENABLED
	HMODULE livePP = lpp::lppLoadAndRegister( L"../../../../../LivePP", "ViewTests" );
	lpp::lppEnableAllCallingModulesSync( livePP );
#endif

	//settings->setWindowPos( 0, 0 );
	settings->setWindowSize( WINDOW_SIZE.x, WINDOW_SIZE.y );

	// move app to secondary display
	if( USE_SECONDARY_SCREEN && Display::getDisplays().size() > 1 ) {
		settings->setDisplay( Display::getDisplays()[1] );
		settings->setFullScreen( true );
	}

	if( MULTITOUCH_ENABLED ) {
		settings->setMultiTouchEnabled();
	}
}

CINDER_APP( ViewTestsApp, RendererGl( RendererGl::Options().msaa( 8 ) ), prepareSettings )

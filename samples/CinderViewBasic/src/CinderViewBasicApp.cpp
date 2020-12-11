#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

#include "vu/vu.h"

using namespace ci;
using namespace std;

class CinderViewBasicApp : public app::App {
  public:
	void setup() override;
	void resize() override;
	void update() override;
	void draw() override;

  private:
	vu::GraphRef mGraph;
};

void CinderViewBasicApp::setup()
{
	mGraph = make_shared<vu::Graph>();
	mGraph->setFillParentEnabled();
	mGraph->connectEvents();

	// add a Button
	auto button = make_shared<vu::Button>( Rectf( 10, 10, 60, 40 ) );
	button->setTitle( "Button" );
	button->getSignalPressed().connect( [] { CI_LOG_I( "button pressed" ); } );
	button->getSignalReleased().connect( [] { CI_LOG_I( "button released" ); } );

	// add an ImageView (by default, it is laid out with ImageScaleMode::FIT)
	auto imageView = make_shared<vu::ImageView>( Rectf( 10, 10, 400, 300 ) + vec2( button->getPosX() + button->getWidth(), 0 ) );
	imageView->getBackground()->setColor( Color( 0, 0.5f, 0.75f ) );
	try {
		imageView->setImage( make_shared<vu::Image>( loadImage( loadFile( app::getAssetPath( "images/monkey_hitchhike.jpg" ) ) ) ) );
	}
	catch( std::exception &exc ) {
		CI_LOG_EXCEPTION( "failed to load image for ImageView", exc );
	}

	mGraph->addSubview( button );
	mGraph->addSubview( imageView );
}

void CinderViewBasicApp::resize()
{
	mGraph->setNeedsLayout();
}

void CinderViewBasicApp::update()
{
	mGraph->propagateUpdate();
}

void CinderViewBasicApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

	mGraph->propagateDraw();
}

CINDER_APP( CinderViewBasicApp, app::RendererGl )

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"

#include "vu/vu.h"

using namespace ci;
using namespace std;

class _TBOX_PREFIX_App : public app::App {
  public:
	void setup() override;
	void resize() override;
	void update() override;
	void draw() override;

  private:
	vu::GraphRef mGraph;
};

void _TBOX_PREFIX_App::setup()
{
	mGraph = make_shared<vu::Graph>();
	mGraph->setFillParentEnabled();
	mGraph->connectEvents();

	// add a Button
	auto button = make_shared<vu::Button>( Rectf( 10, 10, 60, 40 ) );
	button->setTitle( "Button" );
	button->getSignalPressed().connect( [] { CI_LOG_I( "button pressed" ); } );
	button->getSignalReleased().connect( [] { CI_LOG_I( "button released" ); } );

	mGraph->addSubview( button );
}

void _TBOX_PREFIX_App::resize()
{
	mGraph->setNeedsLayout();
}

void _TBOX_PREFIX_App::update()
{
	mGraph->propagateUpdate();
}

void _TBOX_PREFIX_App::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

	mGraph->propagateDraw();
}

CINDER_APP( _TBOX_PREFIX_App, app::RendererGl )

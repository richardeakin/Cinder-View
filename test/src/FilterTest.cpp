#include "FilterTest.h"

#include "cinder/app/App.h"
#include "cinder/Log.h"

using namespace std;
using namespace ci;

class FilterBlur : public ui::Filter {
  public:
	void process( const ui::FrameBufferRef &frameBuffer ) override
	{
		// TODO NEXT: blur this guy
	}
};

FilterTest::FilterTest()
{
	mContainerView = make_shared<View>();
	mContainerView->getBackground()->setColor( Color::white() );
	mContainerView->setLabel( "container" );

	mImageView = make_shared<ui::ImageView>();

	fs::path imageFilePath = app::getAssetPath( "images/monkey_hitchhike.jpg" );
	try {
		CI_LOG_I( "loading image view.." );
		auto image = make_shared<ui::Image>( loadImage( loadFile( imageFilePath ) ) );
		mImageView->setImage( image );
		//mImageView->setScaleMode( ui::ImageScaleMode::FILL );
		CI_LOG_I( "complete" );
	}
	catch( std::exception &exc ) {
		CI_LOG_EXCEPTION( "failed to load image at path: " << imageFilePath, exc );
	}

	mContainerView->addSubview( mImageView );
	addSubview( mContainerView );
}

void FilterTest::layout()
{
	const float padding = 50.0f;
	const vec2 imageViewSize = { 550, 350 };

	//mContainerView->setBounds( Rectf( padding, padding, getWidth() - padding, getHeight() - padding ) );
	//vec2 center = mContainerView->getCenter();

	Rectf containerBounds = Rectf( padding, padding, getWidth() - padding, getHeight() - padding );
	mContainerView->setBounds( containerBounds );

	vec2 center = containerBounds.getCenter();

	Rectf imageRect = { center.x - imageViewSize.x / 2, center.y - imageViewSize.y / 2, center.x + imageViewSize.x / 2, center.y + imageViewSize.y / 2 };
	imageRect -= vec2( padding ); // TODO: why is this needed? shouldn't matter as mImageView lives inside mContainerView
	mImageView->setBounds( imageRect );
}

bool FilterTest::keyDown( ci::app::KeyEvent &event )
{
	return false;
}


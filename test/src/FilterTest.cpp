#include "FilterTest.h"

#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/gl/gl.h"

#include "../../blocks/Cinder-FileWatcher/src/mason/FileWatcher.cpp" // TEMP

using namespace std;
using namespace ci;

FilterBlur::FilterBlur()
{
	vector<fs::path> glslPaths = {
		"glsl/blur.vert",
		"glsl/blur.frag"
	};

	try {
		mWatchGlsl = mason::FileWatcher::load( glslPaths, [this]( const std::vector<fs::path> &fullPaths ) {
			auto format = gl::GlslProg::Format()
				.vertex( loadFile( fullPaths.at( 0 ) ) )
				.fragment( loadFile( fullPaths.at( 1 ) ) )
			;

			try {
				mGlslBlur = gl::GlslProg::create( format );

				CI_LOG_I( "blur glsl loaded." );
			}
			catch( std::exception &exc ) {
				CI_LOG_EXCEPTION( "failed to compile GlslProg at: " << format.getVertexPath(), exc );
			}
		} );
	}
	catch( std::exception &exc ) {
		CI_LOG_EXCEPTION( "failed to load blur glsl", exc );
	}
}

ui::FrameBufferRef FilterBlur::process( ui::Renderer *ren, const ui::FrameBufferRef &inputFrameBuffer )
{
	gl::ScopedGlslProg glslScope( mGlslBlur );

	//float attenuation = 1.0f;
	//mGlslBlur->uniform( "uAttenuation", attenuation );

	// blur horizontally and the size of 1 pixel
	mGlslBlur->uniform( "uSampleOffset", vec2( mBlurPixels.x / inputFrameBuffer->getWidth(), 0.0f ) );

	inputFrameBuffer->mIsBound = true; // HACK: frameBuffer has just been unbound in Layer::draw(), so ren->getFrameBuffer() will otherwise return the same one to us

	// copy a horizontally blurred version of our scene into the first blur Fbo
	auto frameBufferBlur1 = ren->getFrameBuffer( inputFrameBuffer->getSize() );
	{

		ren->pushFrameBuffer( frameBufferBlur1 );

		// TODO: is this needed? framebuffer is same size
		gl::ScopedViewport viewport( 0, 0, inputFrameBuffer->getWidth(), inputFrameBuffer->getHeight() );

		gl::ScopedTextureBind tex0( inputFrameBuffer->getColorTexture(), 0 );

		//gl::ScopedMatrices matScope;
		//gl::setMatricesWindow( mFboBlur1->getWidth(), mFboBlur1->getHeight() );
		//gl::clear();

		gl::drawSolidRect( Rectf( vec2( 0 ), frameBufferBlur1->getSize() ) );

		ren->popFrameBuffer( frameBufferBlur1 );
	}

	//frameBufferBlur1->mFbo->blitTo( inputFrameBuffer->mFbo, frameBufferBlur1->mFbo->getBounds(), inputFrameBuffer->mFbo->getBounds() );
	CI_CHECK_GL();

	inputFrameBuffer->mIsBound = false;
	return frameBufferBlur1;

	// blur vertically and the size of 1 pixel
	//mGlslBlur->uniform( "uSampleOffset", vec2( 0.0f, 1.0f / mFboBlur2->getHeight() ) );

	//// copy a vertically blurred version of our blurred scene into the second blur Fbo
	//{
	//	gl::ScopedFramebuffer fboScope( mFboBlur2 );
	//	gl::ScopedViewport viewportScope( 0, 0, mFboBlur2->getWidth(), mFboBlur2->getHeight() );

	//	gl::ScopedTextureBind tex0( mFboBlur1->getColorTexture(), 0 );

	//	gl::ScopedMatrices matScope;
	//	gl::setMatricesWindow( mFboBlur2->getWidth(), mFboBlur2->getHeight() );
	//	gl::clear();

	//	gl::drawSolidRect( mFboBlur2->getBounds() );
	//}
}

FilterTest::FilterTest()
{
	mContainerView = make_shared<View>();
	mContainerView->getBackground()->setColor( Color::white() );
	mContainerView->setLabel( "container" );

	mImageView = make_shared<ui::ImageView>();
	mImageView->setLabel( "ImageView with filter" );

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

	mFilterBlur = make_shared<FilterBlur>();
	mImageView->addFilter( mFilterBlur );

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


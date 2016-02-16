#include "FilterTest.h"

#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/gl/gl.h"

#include "../../lib/Cinder-FileWatcher/src/mason/FileWatcher.cpp" // TEMP

using namespace std;
using namespace ci;

// ----------------------------------------------------------------------------------------------------
// FilterSinglePass
// ----------------------------------------------------------------------------------------------------

FilterSinglePass::FilterSinglePass()
{
	vector<fs::path> glslPaths = {
		"glsl/filterTest.vert",
		"glsl/filterTest.frag"
	};

	try {
		mWatchGlsl = mason::FileWatcher::load( glslPaths, [this]( const std::vector<fs::path> &fullPaths ) {
			auto format = gl::GlslProg::Format()
				.vertex( loadFile( fullPaths.at( 0 ) ) )
				.fragment( loadFile( fullPaths.at( 1 ) ) )
			;

			try {
				mGlsl = gl::GlslProg::create( format );

				CI_LOG_I( "fliter test glsl loaded." );
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

void FilterSinglePass::process( ui::Renderer *ren, const ui::Filter::Pass &pass  )
{
	gl::ScopedGlslProg		glslScope( mGlsl );
	gl::ScopedTextureBind	texScope( getRenderColorTexture() );

	gl::clear( ColorA::zero() );
	gl::drawSolidRect( Rectf( vec2( 0 ), pass.getSize() ) );
}

// ----------------------------------------------------------------------------------------------------
// FilterBlur
// ----------------------------------------------------------------------------------------------------

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
				mGlsl = gl::GlslProg::create( format );

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

void FilterBlur::process( ui::Renderer *ren, const ui::Filter::Pass &pass )
{
	// TODO: rework with pass stuff
#if 0
	gl::ScopedGlslProg glslScope( mGlsl );

	//float attenuation = 1.0f;
	//mGlslBlur->uniform( "uAttenuation", attenuation );

	// blur horizontally and the size of 1 pixel
	mGlsl->uniform( "uSampleOffset", vec2( mBlurPixels.x / inputFrameBuffer->getWidth(), 0.0f ) );

	// TODO NEXT: figure out how to ask for unique FrameBuffers. I need another for vertical blur too
	// HACK: frameBuffer has just been unbound in Layer::draw(), so ren->getFrameBuffer() will otherwise return the same one to us
	inputFrameBuffer->mIsBound = true;

	// copy a horizontally blurred version of our scene into the first blur Fbo
	auto frameBufferBlur1 = ren->getFrameBuffer( inputFrameBuffer->getSize() );
	{
		ren->pushFrameBuffer( frameBufferBlur1 );

		// TODO: is this needed? framebuffer is same size
		gl::ScopedViewport viewport( 0, 0, inputFrameBuffer->getWidth(), inputFrameBuffer->getHeight() );
		gl::ScopedMatrices matScope;
		gl::setMatricesWindow( inputFrameBuffer->getWidth(), inputFrameBuffer->getHeight() );

		gl::ScopedTextureBind tex0( inputFrameBuffer->getColorTexture(), 0 );

		gl::clear( ColorA::zero() );
		gl::drawSolidRect( Rectf( vec2( 0 ), frameBufferBlur1->getSize() ) );

		ren->popFrameBuffer( frameBufferBlur1 );
	}

	// blur vertically and the size of 1 pixel
	mGlsl->uniform( "uSampleOffset", vec2( 0.0f, mBlurPixels.y / inputFrameBuffer->getWidth() ) );

	frameBufferBlur1->mIsBound = true;
	auto frameBufferBlur2 = ren->getFrameBuffer( inputFrameBuffer->getSize() );

	// copy a vertically blurred version of our blurred scene into the second blur Fbo
	{
		ren->pushFrameBuffer( frameBufferBlur2 );

		gl::ScopedViewport viewport( 0, 0, frameBufferBlur1->getWidth(), frameBufferBlur1->getHeight() );
		gl::ScopedMatrices matScope;
		gl::setMatricesWindow( frameBufferBlur1->getWidth(), frameBufferBlur1->getHeight() );

		gl::ScopedTextureBind tex0( frameBufferBlur1->getColorTexture(), 0 );

		gl::clear( ColorA::zero() );
		gl::drawSolidRect( Rectf( vec2( 0 ), frameBufferBlur1->getSize() ) );

		ren->popFrameBuffer( frameBufferBlur2 );
	}

	inputFrameBuffer->mIsBound = false;
	frameBufferBlur1->mIsBound = false;

	return frameBufferBlur2;
#endif
}

// ----------------------------------------------------------------------------------------------------
// FilterTest
// ----------------------------------------------------------------------------------------------------

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

	mFilterSinglePass = make_shared<FilterSinglePass>();
	mFilterBlur = make_shared<FilterBlur>();

	mImageView->addFilter( mFilterSinglePass );
	//mImageView->addFilter( mFilterBlur );

	auto imageBorder = make_shared<ui::StrokedRectView>();
	imageBorder->setFillParentEnabled();
	imageBorder->setColor( Color( 0.9f, 0.5f, 0.0f ) );
	imageBorder->setLineWidth( 2 );
	mImageView->addSubview( imageBorder );

	const Color toggleEnabledColor = { 0, 0.2f, 0.6f };

	mToggleSinglePass = make_shared<ui::Button>();
	mToggleSinglePass->setTitle( "enable single pass" );
	mToggleSinglePass->setAsToggle();
	mToggleSinglePass->setColor( toggleEnabledColor, ui::Button::State::ENABLED );
	mToggleSinglePass->setTitleColor( Color::white() );
	mToggleSinglePass->setEnabled( true );
	mToggleSinglePass->getSignalReleased().connect( [this] {
		if( mToggleSinglePass->isEnabled() ) {
			mImageView->removeAllFilters();
			mImageView->addFilter( mFilterSinglePass );
			mToggleBlur->setEnabled( false );
		}
		else
			mImageView->removeFilter( mFilterSinglePass );
	} );


	mToggleBlur = make_shared<ui::Button>();
	mToggleBlur->setTitle( "enable blur" );
	mToggleBlur->setAsToggle();
	mToggleBlur->setColor( toggleEnabledColor, ui::Button::State::ENABLED );
	mToggleBlur->setTitleColor( Color::white() );
	mToggleBlur->setEnabled( false );
	mToggleBlur->getSignalReleased().connect( [this] {
		if( mToggleBlur->isEnabled() ) {
			mImageView->removeAllFilters();
			mImageView->addFilter( mFilterBlur );
			mToggleSinglePass->setEnabled( false );
		}
		else
			mImageView->removeFilter( mFilterBlur );
	} );

	mToggleDropShadow = make_shared<ui::Button>();
	mToggleDropShadow->setTitle( "enable drop shadow" );
	mToggleDropShadow->setAsToggle();
	mToggleDropShadow->setColor( toggleEnabledColor, ui::Button::State::ENABLED );
	mToggleDropShadow->setTitleColor( Color::white() );

	mSliderBlur = make_shared<ui::HSlider>();
	mSliderBlur->setTitle( "blur pixels" );
	mSliderBlur->setMax( 10 );
	mSliderBlur->setValue( mFilterBlur->getBlurPixels().x );
	mSliderBlur->getSignalValueChanged().connect( [this] {
		mFilterBlur->setBlurPixels( vec2( mSliderBlur->getValue() ) );
	} );

	mSliderDropShadow = make_shared<ui::HSlider>();
	mSliderDropShadow->setTitle( "drop shadow pixels" );
	mSliderDropShadow->setMax( 10 );

	mContainerView->addSubviews( { 
		mImageView,
		mToggleSinglePass, mToggleBlur, mToggleDropShadow,
		mSliderBlur, mSliderDropShadow
	} );

	addSubview( mContainerView );
}

void FilterTest::layout()
{
	const float containerPadding = 50;
	const float controlPadding = 10;

	Rectf containerBounds = Rectf( containerPadding, containerPadding, getWidth() - containerPadding, getHeight() - containerPadding );
	mContainerView->setBounds( containerBounds );

	vec2 togglePos = { controlPadding, controlPadding };
	vec2 toggleSize = { 130, 40 };

	mToggleSinglePass->setPos( togglePos );
	mToggleSinglePass->setSize( toggleSize );

	togglePos.x += toggleSize.x + controlPadding;
	mToggleBlur->setPos( togglePos );
	mToggleBlur->setSize( toggleSize );

	vec2 sliderPos = { controlPadding, togglePos.y + toggleSize.y + controlPadding }; 
	vec2 sliderSize = { 210, 40 };
	mSliderBlur->setPos( sliderPos );
	mSliderBlur->setSize( sliderSize );

	togglePos.x = getCenter().x;
	mToggleDropShadow->setPos( togglePos );
	mToggleDropShadow->setSize( toggleSize );

	sliderPos.x = togglePos.x;
	mSliderDropShadow->setPos( sliderPos );
	mSliderDropShadow->setSize( sliderSize );

	mImageView->setPos( { controlPadding, mSliderBlur->getBounds().y2 + controlPadding } );
	mImageView->setSize( { ( containerBounds.getWidth() - controlPadding ) / 2.0f, containerBounds.getHeight() - mImageView->getPosY() - controlPadding } );
}

bool FilterTest::keyDown( ci::app::KeyEvent &event )
{
	return false;
}


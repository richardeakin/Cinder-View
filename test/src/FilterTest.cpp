#include "FilterTest.h"

#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/gl/gl.h"

#include "../../blocks/Cinder-FileWatcher/src/mason/FileWatcher.cpp" // TEMP

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
	if( ! mGlsl )
		return;

	gl::ScopedGlslProg		glslScope( mGlsl );
	gl::ScopedTextureBind	texScope( getRenderColorTexture() );

	gl::clear( ColorA::zero() );
	gl::drawSolidRect( Rectf( vec2( 0 ), pass.getSize() ) );
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
		auto image = make_shared<ui::Image>( loadImage( loadFile( imageFilePath ) ) );
		mImageView->setImage( image );
		//mImageView->setScaleMode( ui::ImageScaleMode::FILL );
	}
	catch( std::exception &exc ) {
		CI_LOG_EXCEPTION( "failed to load image at path: " << imageFilePath, exc );
	}

	mLabel = make_shared<ui::Label>();
	mLabel->setText( "T" );
	mLabel->setLabel( "Label with FilterDropShadow" );
	mLabel->setFontFace( ui::FontFace::BOLD );
	mLabel->setFontSize( 280 );
	mLabel->setAlignment( ui::TextAlignment::CENTER );
	mLabel->setTextColor( Color( 0, 0, 0.75f ) );

	mFilterSinglePass = make_shared<FilterSinglePass>();
	mFilterBlur = make_shared<ui::FilterBlur>();
	mFilterDropShadow = make_shared<ui::FilterDropShadow>();
	mFilterDropShadow->setDownsampleFactor( 1 );
	mFilterDropShadow->setShadowOffset( vec2( -10, 8 ) );

	//loadGlsl(); // uncomment to load live shaders in assets folder

	mImageView->addFilter( mFilterSinglePass );
	//mImageView->addFilter( mFilterBlur );
	mLabel->addFilter( mFilterDropShadow );

	{
		auto imageBorder = make_shared<ui::StrokedRectView>();
		imageBorder->setFillParentEnabled();
		imageBorder->setColor( Color( 0.9f, 0.5f, 0.0f ) );
		imageBorder->setLineWidth( 2 );
		mImageView->addSubview( imageBorder );
	}
	//{
	//	auto labelBorder = make_shared<ui::StrokedRectView>();
	//	labelBorder->setFillParentEnabled();
	//	labelBorder->setColor( Color( 0.9f, 0.5f, 0.0f ) );
	//	labelBorder->setLineWidth( 2 );
	//	mLabel->addSubview( labelBorder );
	//}

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
			CI_LOG_I( "-------- adding mFilterSinglePass --------" );
			mImageView->addFilter( mFilterSinglePass );
			mToggleBlur->setEnabled( false );
		}
		else {
			CI_LOG_I( "-------- removing mFilterSinglePass --------" );
			mImageView->removeFilter( mFilterSinglePass );
		}
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
			CI_LOG_I( "-------- adding mFilterBlur --------" );
			mImageView->addFilter( mFilterBlur );
			mToggleSinglePass->setEnabled( false );
		}
		else {
			CI_LOG_I( "-------- removing mFilterBlur --------" );
			mImageView->removeFilter( mFilterBlur );
		}
	} );

	mToggleDropShadow = make_shared<ui::Button>();
	mToggleDropShadow->setTitle( "enable drop shadow" );
	mToggleDropShadow->setAsToggle();
	mToggleDropShadow->setColor( toggleEnabledColor, ui::Button::State::ENABLED );
	mToggleDropShadow->setTitleColor( Color::white() );
	mToggleDropShadow->setEnabled( true ); // filter already added
	mToggleDropShadow->getSignalReleased().connect( [this] {
		if( mToggleDropShadow->isEnabled() ) {
			CI_LOG_I( "-------- adding mFilterDropShadow --------" );
			mLabel->addFilter( mFilterDropShadow );
		}
		else {
			CI_LOG_I( "-------- removing mFilterDropShadow --------" );
			mLabel->removeFilter( mFilterDropShadow );
		}
	} );

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
	mSliderDropShadow->getSignalValueChanged().connect( [this] {
		// TODO: this should be shadow offset
		mFilterDropShadow->setBlurPixels( vec2( mSliderDropShadow->getValue() ) );
	} );

	mContainerView->addSubviews( { 
		mImageView,
		mLabel,
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

	mLabel->setPos( { containerBounds.getCenter().x, mSliderDropShadow->getBounds().y2 + controlPadding } );
	mLabel->setSize( vec2( 384, mImageView->getHeight() ) ); // TODO: fix this
}

bool FilterTest::keyDown( ci::app::KeyEvent &event )
{
	return false;
}

void FilterTest::loadGlsl()
{
	// load blur shader
	{
		vector<fs::path> glslPaths = {
			"glsl/blur.vert",
			"glsl/blur.frag"
		};

		try {
			mWatchGlslBlur = mason::FileWatcher::load( glslPaths, [this]( const std::vector<fs::path> &fullPaths ) {
				auto format = gl::GlslProg::Format()
					.vertex( loadFile( fullPaths.at( 0 ) ) )
					.fragment( loadFile( fullPaths.at( 1 ) ) )
				;

				try {
					mGlslBlur = gl::GlslProg::create( format );
					if( mFilterBlur )
						mFilterBlur->setGlslProg( mGlslBlur );

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

	// load dropshadow shader
	{
		vector<fs::path> glslPaths = {
			"glsl/blur.vert",
			"glsl/dropshadow.frag"
		};

		try {
			mWatchGlslDropshadow = mason::FileWatcher::load( glslPaths, [this]( const std::vector<fs::path> &fullPaths ) {
				auto format = gl::GlslProg::Format()
					.vertex( loadFile( fullPaths.at( 0 ) ) )
					.fragment( loadFile( fullPaths.at( 1 ) ) )
				;

				try {
					mGlslDropshadow = gl::GlslProg::create( format );
					if( mFilterDropShadow )
						mFilterDropShadow->setGlslProg( mGlslDropshadow );

					CI_LOG_I( "dropshadow glsl loaded." );
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
}
#include "FilterTest.h"

#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/gl/gl.h"

using namespace std;
using namespace ci;

// ----------------------------------------------------------------------------------------------------
// FilterSinglePass
// ----------------------------------------------------------------------------------------------------

FilterSinglePass::FilterSinglePass()
{
	try {
		mGlsl = gl::GlslProg::create( app::loadAsset( "glsl/filterTest.vert" ), app::loadAsset( "glsl/filterTest.frag" ) );

		CI_LOG_I( "fliter test glsl loaded." );
	}
	catch( std::exception &exc ) {
		CI_LOG_EXCEPTION( "failed to compile GlslProg for FilterSinglePass.", exc );
	}
}

void FilterSinglePass::process( ui::Renderer *ren, const ui::Filter::Pass &pass )
{
	if( ! mGlsl )
		return;

	gl::ScopedGlslProg		glslScope( mGlsl );
	gl::ScopedTextureBind	texScope( getRenderColorTexture() );

	gl::clear( ColorA::zero() );

	vec2 ratio = vec2( pass.getSize() ) / vec2( getRenderColorTexture()->getSize() );
	vec2 lr = { ratio.x, 1 - ratio.y };
	gl::drawSolidRect( Rectf( vec2( 0 ), pass.getSize() ), vec2( 0, 1 ), lr );
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
	mImageView->setLabel( "ImageView" );

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
	mLabel->setLabel( "Label T" );
	mLabel->setFontSize( 280 );
	mLabel->setAlignment( ui::TextAlignment::CENTER );
	mLabel->setTextColor( Color( 0, 0, 0.75f ) );

	mLabelNested = make_shared<ui::Label>();
	mLabelNested->setText( "PLFF!" );
	mLabelNested->setLabel( "Nested Label" );
	mLabelNested->setFontName( "Arial Bold" );
	mLabelNested->setFontSize( 84 );
	mLabelNested->setAlignment( ui::TextAlignment::CENTER );
	mLabelNested->setTextColor( Color( 0, 0, 0.75f ) );

	mFilterSinglePass = make_shared<FilterSinglePass>();
	mFilterBlur = make_shared<ui::FilterBlur>();
	mFilterBlurNested = make_shared<ui::FilterBlur>();
	mFilterDropShadow = make_shared<ui::FilterDropShadow>();
	mFilterDropShadow->setDownsampleFactor( 1 );
	mFilterDropShadow->setShadowOffset( vec2( -6, 4 ) );

	//loadGlsl(); // uncomment to load live shaders in assets folder

	mImageView->addFilter( mFilterSinglePass );
	//mImageView->addFilter( mFilterBlur );
	mLabel->addFilter( mFilterDropShadow );
	mLabelNested->addFilter( mFilterBlurNested );

	{
		auto imageBorder = make_shared<ui::StrokedRectView>();
		imageBorder->setFillParentEnabled();
		imageBorder->setColor( Color( 0.9f, 0.5f, 0.0f ) );
		imageBorder->setLineWidth( 2 );
		mImageView->addSubview( imageBorder );
	}
#if 0
	{
		auto labelBorder = make_shared<ui::StrokedRectView>();
		labelBorder->setFillParentEnabled();
		labelBorder->setColor( Color( 0.9f, 0.5f, 0.0f ) );
		labelBorder->setLineWidth( 2 );
		mLabel->addSubview( labelBorder );
	}
#endif

	const Color toggleEnabledColor = { 0, 0.4f, 0.6f };

	mToggleSinglePass = make_shared<ui::Button>();
	mToggleSinglePass->setTitle( "single pass" );
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
	mToggleBlur->setTitle( "blur" );
	mToggleBlur->setAsToggle();
	mToggleBlur->setColor( toggleEnabledColor, ui::Button::State::ENABLED );
	mToggleBlur->setTitleColor( Color::white() );
	mToggleBlur->setEnabled( false );
	mToggleBlur->getSignalReleased().connect( [this] {
		// swap out the monkey ImageView's Filter for blur (or swap back)
		if( mToggleBlur->isEnabled() ) {
			mImageView->removeAllFilters();
			mImageView->addFilter( mFilterBlur );
			mToggleSinglePass->setEnabled( false );
		}
		else {
			mImageView->removeFilter( mFilterBlur );
		}
	} );

	mToggleBlurNested = make_shared<ui::Button>();
	mToggleBlurNested->setTitle( "blur nested" );
	mToggleBlurNested->setAsToggle();
	mToggleBlurNested->setColor( toggleEnabledColor, ui::Button::State::ENABLED );
	mToggleBlurNested->setTitleColor( Color::white() );
	mToggleBlurNested->setEnabled( true );
	mToggleBlurNested->getSignalReleased().connect( [this] {
		// toggle blur on the nested label, within the monkey ImageView
		if( mToggleBlurNested->isEnabled() )
			mLabelNested->addFilter( mFilterBlurNested );
		else
			mLabelNested->removeAllFilters();
	} );


	mToggleDropShadow = make_shared<ui::Button>();
	mToggleDropShadow->setTitle( "drop shadow" );
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
			CI_LOG_I( "-------- removing all filters from Label --------" );
			mLabel->removeAllFilters();
		}
	} );

	mSliderBlur = make_shared<ui::HSlider>();
	mSliderBlur->setTitle( "blur pixels" );
	mSliderBlur->setMax( 10 );
	mSliderBlur->setValue( mFilterBlur->getBlurPixels().x );
	mSliderBlur->getSignalValueChanged().connect( [this] {
		mFilterBlur->setBlurPixels( vec2( mSliderBlur->getValue() ) );
		mFilterBlurNested->setBlurPixels( vec2( mSliderBlur->getValue() ) );
	} );

	mSliderDropShadow = make_shared<ui::HSlider>();
	mSliderDropShadow->setTitle( "drop shadow pixels" );
	mSliderDropShadow->setMax( 10 );
	mSliderDropShadow->getSignalValueChanged().connect( [this] {
		// TODO: this should be shadow offset
		mFilterDropShadow->setBlurPixels( vec2( mSliderDropShadow->getValue() ) );
	} );


	mImageView->addSubview( mLabelNested );

	mContainerView->addSubviews( { 
		mImageView,
		mLabel,
		mToggleSinglePass, mToggleBlur, mToggleBlurNested, mToggleDropShadow,
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

	togglePos.x += toggleSize.x + controlPadding;
	mToggleBlurNested->setPos( togglePos );
	mToggleBlurNested->setSize( toggleSize );

	vec2 sliderPos = { controlPadding, togglePos.y + toggleSize.y + controlPadding }; 
	vec2 sliderSize = { 210, 40 };
	mSliderBlur->setPos( sliderPos );
	mSliderBlur->setSize( sliderSize );

	togglePos.x = ( containerBounds.getWidth() + controlPadding ) / 2;
	mToggleDropShadow->setPos( togglePos );
	mToggleDropShadow->setSize( toggleSize );

	sliderPos.x = togglePos.x;
	mSliderDropShadow->setPos( sliderPos );
	mSliderDropShadow->setSize( sliderSize );

	// Sizing the Views with Filters attached: the Label's size is intentionally different in x and y than the ImageView, to test FrameBuffers being reused
	// by multiple Filters within the same draw loop
	mImageView->setPos( { controlPadding, mSliderBlur->getBounds().y2 + controlPadding } );
	mImageView->setSize( { ( containerBounds.getWidth() - controlPadding ) / 2.0f - controlPadding, containerBounds.getHeight() - mImageView->getPosY() - controlPadding } );

	const vec2 sizeDiff = { 40, 40 };
	mLabel->setPos( { togglePos.x, mSliderDropShadow->getBounds().y2 + controlPadding } );
	mLabel->setSize( vec2( containerBounds.getWidth() - mLabel->getPosX() - controlPadding - sizeDiff.x, mImageView->getHeight() - sizeDiff.y ) );

	// Nested Label with blur filter lives in the ImageView, at the top right
	mLabelNested->setPos( vec2( 220, 80 ) );
	mLabelNested->setSize( vec2( 240, 100 ) );
}

bool FilterTest::keyDown( ci::app::KeyEvent &event )
{
	if( event.getChar() == ' ' ) {
		loadGlsl();
		return true;
	}
	return false;
}

void FilterTest::loadGlsl()
{
	// load blur shader
	try {
		mGlslBlur = gl::GlslProg::create( app::loadAsset( "glsl/blur.vert" ), app::loadAsset( "glsl/blur.frag" ) );
		if( mFilterBlur )
			mFilterBlur->setGlslProg( mGlslBlur );
		if( mFilterBlurNested )
			mFilterBlurNested->setGlslProg( mGlslBlur );

		CI_LOG_I( "blur glsl loaded." );
	}
	catch( std::exception &exc ) {
		CI_LOG_EXCEPTION( "failed to compile GlslProg for blur.", exc );
	}

	// load dropshadow shader
	try {
		mGlslDropshadow = gl::GlslProg::create( app::loadAsset( "glsl/blur.vert" ), app::loadAsset( "glsl/dropshadow.frag" ) );
		if( mFilterDropShadow )
			mFilterDropShadow->setGlslProg( mGlslDropshadow );

		CI_LOG_I( "dropshadow glsl loaded." );
	}
	catch( std::exception &exc ) {
		CI_LOG_EXCEPTION( "failed to compile GlslProg for dropshadow.", exc );
	}
}
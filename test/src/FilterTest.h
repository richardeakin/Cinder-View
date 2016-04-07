#pragma once

#include "ui/Suite.h"
#include "ui/ui.h"

#include "cinder/gl/TextureFont.h"

#include "../../lib/Cinder-FileWatcher/src/mason/FileWatcher.h" // TEMP

class FilterSinglePass : public ui::Filter {
public:
	FilterSinglePass();

	void process( ui::Renderer *ren, const ui::Filter::Pass &pass  ) override;

	ci::gl::GlslProgRef	mGlsl;
	mason::ScopedWatch	mWatchGlsl;

private:
};

typedef std::shared_ptr<class FilterBlur>	FilterBlurRef;

class FilterBlur : public ui::Filter {
  public:
	FilterBlur();

	void configure( const ci::ivec2 &size, ui::Filter::PassInfo *info ) override;
	void process( ui::Renderer *ren, const ui::Filter::Pass &frame  ) override;

	ci::gl::GlslProgRef	mGlsl;
	mason::ScopedWatch mWatchGlsl;

	const ci::vec2&	getBlurPixels() const						{ return mBlurPixels; }
	void			setBlurPixels( const ci::vec2 &pixels )		{ mBlurPixels = pixels; }

  private:
	ci::vec2	mBlurPixels = ci::vec2( 1 );
};

typedef std::shared_ptr<class FilterDropShadow>	FilterDropShadowRef;

class FilterDropShadow : public ui::Filter {
public:
	FilterDropShadow();

	void configure( const ci::ivec2 &size, ui::Filter::PassInfo *info ) override;
	void process( ui::Renderer *ren, const ui::Filter::Pass &frame  ) override;

	ci::gl::GlslProgRef	mGlsl;
	mason::ScopedWatch mWatchGlsl;

	const ci::vec2&	getBlurPixels() const						{ return mBlurPixels; }
	void			setBlurPixels( const ci::vec2 &pixels )		{ mBlurPixels = pixels; }

	const ci::vec2&	getShadowOffset() const						{ return mShadowOffset; }
	void			setShadowOffset( const ci::vec2 &pixels )	{ mShadowOffset= pixels; }

private:
	ci::vec2	mBlurPixels = ci::vec2( 1 );
	ci::vec2	mShadowOffset = ci::vec2( 10 );
};

class FilterTest : public ui::SuiteView {
  public:
	FilterTest();

	void layout() override;

  private:
	bool keyDown( ci::app::KeyEvent &event ) override;

	std::shared_ptr<FilterSinglePass>	mFilterSinglePass;
	FilterBlurRef						mFilterBlur;
	FilterDropShadowRef					mFilterDropShadow;

	ui::ViewRef				mContainerView;
	ui::ImageViewRef        mImageView;
	ui::ButtonRef			mToggleSinglePass, mToggleBlur, mToggleDropShadow;
	ui::HSliderRef			mSliderBlur, mSliderDropShadow;
	ui::LabelRef			mLabel;
};

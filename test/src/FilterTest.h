#pragma once

#include "ui/Suite.h"
#include "ui/ui.h"

#include "cinder/gl/TextureFont.h"

#include "../../lib/Cinder-FileWatcher/src/mason/FileWatcher.h" // TEMP

class FilterBlur : public ui::Filter {
  public:
	FilterBlur();

	ui::FrameBufferRef process( ui::Renderer *ren, const ui::FrameBufferRef &inputFrameBuffer ) override;

	ci::gl::GlslProgRef	mGlslBlur;
	mason::ScopedWatch mWatchGlsl;

	const ci::vec2&	getBlurPixels() const						{ return mBlurPixels; }
	void			setBlurPixels( const ci::vec2 &pixels )		{ mBlurPixels = pixels; }

  private:
	ci::vec2	mBlurPixels = ci::vec2( 1 );
};

typedef std::shared_ptr<FilterBlur>	FilterBlurRef;

class FilterTest : public ui::SuiteView {
  public:
	FilterTest();

	void layout() override;

  private:
	bool keyDown( ci::app::KeyEvent &event ) override;

	FilterBlurRef			mFilterBlur;

	ui::ViewRef				mContainerView;
	ui::ImageViewRef        mImageView;
	ui::ButtonRef			mToggleBlur, mToggleDropShadow;
	ui::HSliderRef			mSliderBlur, mSliderDropShadow;
};

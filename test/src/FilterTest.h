#pragma once

#include "ui/Suite.h"
#include "ui/ui.h"

#include "cinder/gl/TextureFont.h"

#include "../../blocks/Cinder-FileWatcher/src/mason/FileWatcher.h" // TEMP

class FilterSinglePass : public ui::Filter {
public:
	FilterSinglePass();

	void process( ui::Renderer *ren, const ui::Filter::Pass &pass  ) override;

	ci::gl::GlslProgRef	mGlsl;
	mason::ScopedWatch	mWatchGlsl;

private:
};


class FilterTest : public ui::SuiteView {
  public:
	FilterTest();

	void layout() override;

  private:
	bool keyDown( ci::app::KeyEvent &event ) override;
	void loadGlsl();

	std::shared_ptr<FilterSinglePass>	mFilterSinglePass;
	ui::FilterBlurRef					mFilterBlur;
	ui::FilterDropShadowRef				mFilterDropShadow;

	ci::gl::GlslProgRef					mGlslBlur, mGlslDropshadow;
	mason::ScopedWatch					mWatchGlslBlur, mWatchGlslDropshadow;

	ui::ViewRef				mContainerView;
	ui::ImageViewRef        mImageView;
	ui::ButtonRef			mToggleSinglePass, mToggleBlur, mToggleDropShadow;
	ui::HSliderRef			mSliderBlur, mSliderDropShadow;
	ui::LabelRef			mLabel;
};

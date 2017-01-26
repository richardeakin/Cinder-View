#pragma once

#include "ui/Suite.h"
#include "ui/ui.h"

#include "cinder/gl/TextureFont.h"

class FilterSinglePass : public ui::Filter {
public:
	FilterSinglePass();

	void process( ui::Renderer *ren, const ui::Filter::Pass &pass  ) override;

	ci::gl::GlslProgRef	mGlsl;

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

	ui::ViewRef				mContainerView;
	ui::ImageViewRef        mImageView;
	ui::ButtonRef			mToggleSinglePass, mToggleBlur, mToggleDropShadow;
	ui::HSliderRef			mSliderBlur, mSliderDropShadow;
	ui::LabelRef			mLabel;
};

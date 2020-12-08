#pragma once

#include "vu/Suite.h"
#include "vu/vu.h"

#include "cinder/gl/TextureFont.h"

class FilterSinglePass : public vu::Filter {
public:
	FilterSinglePass();

	void process( vu::Renderer *ren, const vu::Filter::Pass &pass  ) override;

	ci::gl::GlslProgRef	mGlsl;

private:
};


class FilterTest : public vu::SuiteView {
  public:
	FilterTest();

	void layout() override;

  private:
	bool keyDown( ci::app::KeyEvent &event ) override;
	void loadGlsl();

	std::shared_ptr<FilterSinglePass>	mFilterSinglePass;
	vu::FilterBlurRef					mFilterBlur, mFilterBlurNested;
	vu::FilterDropShadowRef				mFilterDropShadow;

	ci::gl::GlslProgRef					mGlslBlur, mGlslDropshadow;

	vu::ViewRef				mContainerView;
	vu::ImageViewRef        mImageView;
	vu::ButtonRef			mToggleSinglePass, mToggleBlur, mToggleBlurNested, mToggleDropShadow;
	vu::HSliderRef			mSliderBlur, mSliderDropShadow;
	vu::LabelRef			mLabel, mLabelNested;
};

#pragma once

#include "view/Suite.h"

#include "view/Slider.h"
#include "view/Button.h"
#include "view/Label.h"
#include "view/ImageView.h"

class ViewTest : public view::SuiteView {
public:
	ViewTest();

	void layout() override;

private:
	void keyEvent( ci::app::KeyEvent &event );

	view::ViewRef				mContainerView;
	view::StrokedRectViewRef	mBorderView;

	view::HSliderRef			mHSlider;
	view::VSliderRef			mVSlider;
	view::ButtonRef				mButton, mToggle;
	view::LabelRef				mLabel, mLabelClipped;
	view::LabelGridRef			mLabelGrid;
	view::ImageViewRef			mImageView;
};

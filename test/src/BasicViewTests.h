#pragma once

#include "view/Suite.h"

#include "view/Slider.h"
#include "view/Button.h"
#include "view/Label.h"
#include "view/ImageView.h"

class BasicViewTests : public view::SuiteView {
public:
	BasicViewTests();

	void layout() override;

private:
	void keyEvent( ci::app::KeyEvent &event );

	view::ViewRef				mContainerView;
	view::StrokedRectViewRef	mBorderView;

	view::LabelRef				mLabel, mLabelClipped;
	view::LabelGridRef			mLabelGrid;
	view::ImageViewRef			mImageView;
};

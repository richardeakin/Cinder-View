#pragma once

#include "ui/ui.h"

class BasicViewTests : public ui::SuiteView {
public:
	BasicViewTests();

	void layout() override;

private:
	bool keyDown( ci::app::KeyEvent &event ) override;

	ui::ViewRef				mContainerView;
	ui::StrokedRectViewRef	mBorderView;

	ui::LabelRef				mLabel, mLabelClipped;
	ui::LabelGridRef			mLabelGrid;
	ui::ImageViewRef			mImageView;
};

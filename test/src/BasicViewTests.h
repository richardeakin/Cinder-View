#pragma once

#include "ui/ui.h"

class BasicViewTests : public ui::SuiteView {
public:
	BasicViewTests();

	void layout() override;
	void update() override;

	bool keyDown( ci::app::KeyEvent &event ) override;

private:
	void loadImageViewShader();

	ui::ViewRef				mContainerView;
	ui::StrokedRectViewRef	mBorderView;

	ui::LabelRef				mLabel, mLabelClipped;
	ui::LabelGridRef			mLabelGrid;
	ui::ImageViewRef			mImageView;
};

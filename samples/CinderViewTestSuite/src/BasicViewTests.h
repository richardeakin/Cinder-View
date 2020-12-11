#pragma once

#include "vu/vu.h"

class BasicViewTests : public vu::SuiteView {
public:
	BasicViewTests();

	void layout() override;
	void update() override;

	bool keyDown( ci::app::KeyEvent &event ) override;

private:
	void loadImageViewShader();

	vu::ViewRef				mContainerView;
	vu::StrokedRectViewRef	mBorderView;

	vu::LabelRef				mLabel, mLabelClipped;
	vu::LabelGridRef			mLabelGrid;
	vu::ImageViewRef			mImageView;
};

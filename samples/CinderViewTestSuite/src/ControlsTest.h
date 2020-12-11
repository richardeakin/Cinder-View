#pragma once

#include "vu/Suite.h"

#include "cinder/gl/TextureFont.h"

class ControlsTest : public vu::SuiteView {
public:
	ControlsTest();

	void layout() override;

private:
	bool keyDown( ci::app::KeyEvent &event ) override;

	vu::HSliderRef			mHSlider;
	vu::VSliderRef			mVSlider;
	vu::ButtonRef			mButton, mImageButton, mToggle;
	vu::CheckBoxRef			mCheckBox;
    vu::TextFieldRef        mTextField1, mTextField2, mTextField3;
};

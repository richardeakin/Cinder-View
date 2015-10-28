#pragma once

#include "view/Suite.h"

#include "view/Slider.h"
#include "view/Button.h"

class ControlsTest : public view::SuiteView {
public:
	ControlsTest();

	void layout() override;

private:
	void keyEvent( ci::app::KeyEvent &event );

	view::HSliderRef			mHSlider;
	view::VSliderRef			mVSlider;
	view::ButtonRef				mButton, mToggle;
};

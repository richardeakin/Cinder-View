#pragma once

#include "ui/Suite.h"

#include "ui/Slider.h"
#include "ui/Button.h"

class ControlsTest : public ui::SuiteView {
public:
	ControlsTest();

	void layout() override;

private:
	void keyEvent( ci::app::KeyEvent &event );

	ui::HSliderRef			mHSlider;
	ui::VSliderRef			mVSlider;
	ui::ButtonRef				mButton, mToggle;
};

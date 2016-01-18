#pragma once

#include "ui/Suite.h"

#include "ui/Slider.h"
#include "ui/Button.h"

#include "cinder/gl/TextureFont.h"

class ControlsTest : public ui::SuiteView {
public:
	ControlsTest();

	void layout() override;

private:
	bool keyDown( ci::app::KeyEvent &event ) override;

	ui::HSliderRef			mHSlider;
	ui::VSliderRef			mVSlider;
	ui::ButtonRef			mButton, mToggle;
};

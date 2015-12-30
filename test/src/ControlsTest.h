#pragma once

#include "ui/Suite.h"

#include "ui/Slider.h"
#include "ui/Button.h"

#include "cinder/gl/TextureFont.h"

typedef std::shared_ptr<class TouchOverlayView> TouchOverlayViewRef;

class ControlsTest : public ui::SuiteView {
public:
	ControlsTest();

	void layout() override;

private:
	void keyEvent( ci::app::KeyEvent &event );

	ui::HSliderRef			mHSlider;
	ui::VSliderRef			mVSlider;
	ui::ButtonRef			mButton, mToggle;
	TouchOverlayViewRef     mTouchOverlay;
};

class TouchOverlayView : public ui::View {
  public:
	TouchOverlayView();

	void draw( ui::Renderer *ren ) override;

  private:
	ci::gl::TextureFontRef  mTextureFont;
};


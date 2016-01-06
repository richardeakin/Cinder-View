#pragma once

#include "ui/Suite.h"

#include "ui/Slider.h"
#include "ui/Button.h"

#include "cinder/gl/TextureFont.h"

typedef std::shared_ptr<class TouchOverlayView> TouchOverlayViewRef;

class MultiTouchTest : public ui::SuiteView {
  public:
	MultiTouchTest();

	void layout() override;

	void update() override;

  private:
	void keyEvent( ci::app::KeyEvent &event );

	void setupControls();
	void setupDraggables();
	void layoutControls();
	void layoutDraggables();
	void injectTouches();

	ui::VSliderRef			mVSlider1, mVSlider2;
	ui::ButtonRef			mButton, mToggle;
	ui::ViewRef             mControlsContainer, mDraggablesContainer;
	TouchOverlayViewRef     mTouchOverlay;
};

class TouchOverlayView : public ui::View {
  public:
	TouchOverlayView();

	void draw( ui::Renderer *ren ) override;

  private:
	ci::gl::TextureFontRef  mTextureFont;
};


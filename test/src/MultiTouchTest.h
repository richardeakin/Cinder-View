#pragma once

#include "cinder/Tween.h"

#include "ui/Suite.h"

#include "cinder/gl/gl.h"

typedef std::shared_ptr<class TouchOverlayView> TouchOverlayViewRef;

class MultiTouchTest : public ui::SuiteView {
  public:
	MultiTouchTest();

	void layout() override;

	void update() override;

  private:
	bool keyDown( ci::app::KeyEvent &event ) override;

	void setupControls();
	void setupDraggables();
	void layoutControls();
	void layoutDraggables();
	void injectTouches();
	void injectContinuousTouches();
	void endContinuousTouches();

	ui::VSliderRef			mVSlider1, mVSlider2;
	ui::ButtonRef			mButton, mToggle;
	ui::ViewRef             mControlsContainer, mDraggablesContainer;
	TouchOverlayViewRef     mTouchOverlay;

	bool mEnableContinuousInjection = false;

	struct TestTouch {
		enum class Phase { UNUSED, BEGAN, MOVED, ENDED };

		ci::Anim<ci::vec2> mPos;
		Phase mPhase = Phase::UNUSED;
	};

	std::vector<TestTouch> mTestTouches;
};

class TouchOverlayView : public ui::View {
  public:
	TouchOverlayView();

	void draw( ui::Renderer *ren ) override;

  private:
	ci::gl::TextureFontRef  mTextureFont;
	ci::gl::BatchRef		mBatchCircle;
};


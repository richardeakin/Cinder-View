#pragma once

#include "cinder/Tween.h"

#include "vu/Suite.h"

#include "cinder/gl/gl.h"

class MultiTouchTest : public vu::SuiteView {
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

	vu::VSliderRef			mVSlider1, mVSlider2;
	vu::ButtonRef			mButton, mToggle;
	vu::ViewRef             mControlsContainer, mDraggablesContainer;

	bool mEnableContinuousInjection = false;

	struct TestTouch {
		enum class Phase { UNUSED, BEGAN, MOVED, ENDED };

		ci::Anim<ci::vec2> mPos;
		Phase mPhase = Phase::UNUSED;
	};

	std::vector<TestTouch> mTestTouches;
};

class TouchOverlayView : public vu::View {
  public:
	TouchOverlayView();

	void draw( vu::Renderer *ren ) override;

  private:
	ci::gl::TextureFontRef  mTextureFont;
	ci::gl::BatchRef		mBatchCircle;
};


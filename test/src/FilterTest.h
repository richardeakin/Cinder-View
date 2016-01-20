#pragma once

#include "ui/Suite.h"

#include "ui/ImageView.h"

#include "cinder/gl/TextureFont.h"

class FilterTest : public ui::SuiteView {
  public:
	FilterTest();

	void layout() override;

  private:
	bool keyDown( ci::app::KeyEvent &event ) override;

	ui::ViewRef				mContainerView;
	ui::ImageViewRef        mImageView;
};

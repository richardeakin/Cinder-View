#pragma once

#include "ui/Suite.h"

#include "ui/ImageView.h"

#include "cinder/gl/TextureFont.h"

#include "../../blocks/Cinder-FileWatcher/src/mason/FileWatcher.h" // TEMP

class FilterBlur : public ui::Filter {
  public:
	FilterBlur();

	void process( ui::Renderer *ren, const ui::FrameBufferRef &frameBuffer ) override;

	ci::gl::GlslProgRef	mGlslBlur;
	mason::ScopedWatch mWatchGlsl;
};

typedef std::shared_ptr<FilterBlur>	FilterBlurRef;

class FilterTest : public ui::SuiteView {
  public:
	FilterTest();

	void layout() override;

  private:
	bool keyDown( ci::app::KeyEvent &event ) override;

	ui::ViewRef				mContainerView;
	ui::ImageViewRef        mImageView;
	FilterBlurRef			mFilterBlur;
};

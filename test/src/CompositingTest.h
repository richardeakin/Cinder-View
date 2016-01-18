#pragma once

#include "ui/Suite.h"
#include "ui/Label.h"

class CompositingTest : public ui::SuiteView {
public:
	CompositingTest();

	void layout() override;
	void update() override;
  private:
	bool keyDown( ci::app::KeyEvent &event ) override;


	ui::StrokedRectViewRef	    mContainerView;

	ui::LabelRef mLabelA, mLabelB, mLabelC, mLabelD;
};

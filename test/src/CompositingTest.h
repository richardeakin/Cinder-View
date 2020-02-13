#pragma once

#include "vu/Suite.h"
#include "vu/Label.h"

class CompositingTest : public vu::SuiteView {
public:
	CompositingTest();
	~CompositingTest();

	void layout() override;
	void update() override;
  private:
	bool keyDown( ci::app::KeyEvent &event ) override;


	vu::StrokedRectViewRef	    mContainerView;

	vu::LabelRef mLabelA, mLabelB, mLabelC, mLabelD;
};

#pragma once

#include "ui/Suite.h"
#include "ui/Label.h"

class CompositingTest : public ui::SuiteView {
public:
	CompositingTest();

	void layout() override;

  private:
	void keyEvent( ci::app::KeyEvent &event );

	ui::ViewRef				mContainerView;

	ui::LabelRef mLabelA, mLabelB, mLabelC, mLabelD;
};

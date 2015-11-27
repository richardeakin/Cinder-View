#pragma once

#include "ui/Suite.h"
#include "ui/Label.h"

class LayerTest : public ui::SuiteView {
public:
	LayerTest();

	void layout() override;

private:
	void keyEvent( ci::app::KeyEvent &event );

	ui::ViewRef				mContainerView;
//	ui::StrokedRectViewRef	mBorderView;

	ui::LabelRef				mLabel1, mLabel2, mLabel3;
};

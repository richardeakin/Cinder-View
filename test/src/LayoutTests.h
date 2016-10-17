#pragma once

#include "ui/Suite.h"

#include "ui/Layout.h"

class LayoutTests : public ui::SuiteView {
public:
	LayoutTests();

	void layout() override;
	bool keyDown( ci::app::KeyEvent &event ) override;

private:

	ui::ViewRef	mVerticalGroupView;
	ui::VerticalLayoutRef	mVerticalLayout;
	ui::HorizontalLayoutRef mHorizontalLayout;
};

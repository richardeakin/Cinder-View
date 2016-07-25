#pragma once

#include "ui/Suite.h"

class LayoutTests : public ui::SuiteView {
public:
	LayoutTests();

	void layout() override;
	bool keyDown( ci::app::KeyEvent &event ) override;

private:

	ui::ViewRef	mVerticalGroup;
};

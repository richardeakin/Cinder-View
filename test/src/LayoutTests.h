#pragma once

#include "ui/Suite.h"
#include "ui/Layout.h"

#include <unordered_map>

class LayoutTests : public ui::SuiteView {
public:
	LayoutTests();

	void layout() override;
	bool keyDown( ci::app::KeyEvent &event ) override;

private:

	ui::ViewRef	mVerticalGroupView;
	ui::VerticalLayoutRef	mVerticalLayout;
	ui::HorizontalLayoutRef mHorizontalLayout;

	std::unordered_map<ui::ViewRef, ci::vec2>	mInitialSizes;
};

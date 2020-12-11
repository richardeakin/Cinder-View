#pragma once

#include "vu/Suite.h"
#include "vu/Layout.h"

#include <unordered_map>

class LayoutTests : public vu::SuiteView {
public:
	LayoutTests();

	void layout() override;
	bool keyDown( ci::app::KeyEvent &event ) override;

private:
	void addLabels( const vu::ViewRef &view, size_t count );

	vu::ViewRef	mVerticalGroupView, mHorizontalGroupView;
	vu::VerticalLayoutRef	mVerticalLayout;
	vu::HorizontalLayoutRef mHorizontalLayout;

	std::unordered_map<vu::ViewRef, ci::vec2>	mInitialSizes;
};

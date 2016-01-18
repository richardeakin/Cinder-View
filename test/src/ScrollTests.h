#pragma once

#include "ui/Suite.h"
#include "ui/ScrollView.h"
#include "ui/Button.h"
#include "ui/Label.h"

class ScrollTests : public ui::SuiteView {
public:
	ScrollTests();

	void layout() override;

private:
	bool keyDown( ci::app::KeyEvent &event ) override;

	ui::ScrollViewRef			mScrollView;
	ui::PagingScrollViewRef	mHorizontalPager, mVerticalPager;
};

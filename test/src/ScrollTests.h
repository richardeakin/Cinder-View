#pragma once

#include "ui/Suite.h"
#include "ui/ScrollView.h"

class ScrollTests : public ui::SuiteView {
public:
	ScrollTests();

	void layout() override;

private:
	bool keyDown( ci::app::KeyEvent &event ) override;

	ui::ScrollViewRef			mScrollViewFree, mScrollViewNested, mScrollViewWithLayout;
	ui::PagingScrollViewRef		mHorizontalPager, mVerticalPager;
};

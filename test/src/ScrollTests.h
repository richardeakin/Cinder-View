#pragma once

#include "ui/Suite.h"
#include "ui/ScrollView.h"

class ScrollTests : public ui::SuiteView {
public:
	ScrollTests();

protected:
	void layout() override;
	void update() override;
	bool keyDown( ci::app::KeyEvent &event ) override;

private:

	ui::ScrollViewRef			mScrollViewFree, mScrollViewNested, mScrollViewWithLayout;
	ui::PagingScrollViewRef		mHorizontalPager, mVerticalPager;
	
	ui::LabelGridRef			mInfoLabel;
};

#pragma once

#include "vu/Suite.h"
#include "vu/ScrollView.h"

class ScrollTests : public vu::SuiteView {
public:
	ScrollTests();

protected:
	void layout() override;
	void update() override;
	bool keyDown( ci::app::KeyEvent &event ) override;

private:

	vu::ScrollViewRef			mScrollViewFree, mScrollViewNested, mScrollViewWithLayout;
	vu::PagingScrollViewRef		mHorizontalPager, mVerticalPager;
	
	vu::LabelGridRef			mInfoLabel;
};

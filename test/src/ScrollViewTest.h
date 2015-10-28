#pragma once

#include "view/Suite.h"
#include "view/ScrollView.h"
#include "view/Button.h"
#include "view/Label.h"

class ScrollViewTest : public view::SuiteView {
public:
	ScrollViewTest();

	void layout() override;

private:
	void keyEvent( ci::app::KeyEvent &event );

	view::ScrollViewRef			mScrollView;
	view::PagingScrollViewRef	mHorizontalPager, mVerticalPager;
};

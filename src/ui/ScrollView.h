/*
 Copyright (c) 2015, Richard Eakin - All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided
 that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice, this list of conditions and
 the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
 the following disclaimer in the documentation and/or other materials provided with the distribution.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "ui/View.h"

namespace ui {

typedef std::shared_ptr<class ScrollView>			ScrollViewRef;
typedef std::shared_ptr<class PagingScrollView>		PagingScrollViewRef;

class ScrollView : public View {
  public:
	ScrollView( const ci::Rectf &bounds = ci::Rectf::zero() );
	virtual ~ScrollView();

	virtual void addContentView( const ViewRef &view, bool updateContentLayout = true );
	void addContentViews( const std::vector<ViewRef> &views );

	ViewRef getContentView( size_t pageIndex ) const;

	// TODO: why is this returning the container? return the content views added with addContentView
	// - can get those with getContentView()->getSubviews() but these names are confusing
	// - can rename this to mContainerView and getContainerView()
	View*	getContentView();
	const View*	getContentView() const;
	size_t	getNumContentViews() const;


	// TODO: make this more generic and virtual so all Views can be used to convert between coord systems
	// - I think this conversion will just work in a more generic case since mContentView's position is just offset
	ci::vec2    convertPointToParent( const ViewRef &contentView ) const;

	void					setContentOffset( const ci::vec2 &offset );
	const ci::vec2&			getContentOffset() const	{ return mContentOffset; }
	ci::Anim<ci::vec2>&		getContentOffsetAnim()		{ return mContentOffset; }

	//! Returns whether the user is currently dragging on the ScrollView
	bool isDragging() const			{ return mDragging; }
	//! Returns whether the contents are freely decelerating, e.g. after the user has finished dragging.
	bool isDecelerating() const		{ return mDecelerating; }
	//! Returns whether scrolling is enabled or not.
	bool isScrollingEnabled() const	{ return mScrollingEnabled; }
	//! Sets whether scrolling is enabled or not.
	void setScrollingEnabled( bool enable = true )	{ mScrollingEnabled = enable; }

	//! Sets the friction factor to apply to velocity to slow it down (inside bounds). Default: 0.05
	void setDecelerationFactorInside( float value )			{ mDecelerationFactorInside = value; }
	//! Returns the friction factor to apply to velocity to slow it down (inside bounds). Default: 0.05
	float getDecelerationFactorInside() const				{ return mDecelerationFactorInside; }
	//! Sets the friction factor to apply to velocity to slow it down (outside bounds). Default: 0.15
	void setDecelerationFactorOutside( float value )		{ mDecelerationFactorOutside = value; }
	//! Returns the friction factor to apply to velocity to slow it down (outside bounds). Default: 0.15
	float getDecelerationFactorOutside() const				{ return mDecelerationFactorOutside; }
	//! Sets the percentage of [target - offset] to pull the offset back in bounds. Default: 0.2
	void setConstraintStiffness( float value )				{ mConstraintStiffness = value; }
	//! Returns the percentage of [target - offset] to pull the offset back in bounds. Default: 0.2
	float getConstraintStiffness() const					{ return mConstraintStiffness; }
	//! Sets the percentage of out of boundaries offset to use, making the content feel like it is being stretched outside. Default: 0.5
	void setStretchFactor( float value )					{ mStretchFactor = value; }
	//! Returns the percentage of out of boundaries offset to use, making the content feel like it is being stretched outside. Default: 0.5
	float getStretchFactor() const							{ return mStretchFactor; }
	//! Sets the velocity (length) so that less than this is considered stopped. Default: 10
	void setMinVelocityConsideredAsStopped( float value )	{ mMinVelocityConsideredAsStopped = value; }
	//! Returns the velocity (length) so that less than this is considered stopped. Default: 10
	float getMinVelocityConsideredAsStopped() const			{ return mMinVelocityConsideredAsStopped; }
	//! Sets an offset (length) so that less than this is considered stopped. Default: 0.1
	void setMinOffsetUntilStopped( float value )			{ mMinOffsetUntilStopped = value; }
	//! Returns an offset (length) so that less than this is considered stopped. Default: 0.1
	float getMinOffsetUntilStopped() const					{ return mMinOffsetUntilStopped; }
	//! Sets the max speed (pixels) that can be applied as a result of seeking toward target offset. Default: 300.0
	void setMaxSpeed( float value )							{ mMaxSpeed = value; }
	//! Returns the max speed (pixels) that can be applied as a result of seeking toward target offset. Default: 300.0
	float getMaxSpeed() const								{ return mMaxSpeed; }

	void setVerticalScrollingEnabled( bool enable )			{ mVerticalScrollingEnabled = enable; }
	bool isVerticalScrollingEnabled() const					{ return mVerticalScrollingEnabled; }
	void setHorizontalScrollingEnabled( bool enable )		{ mHorizontalScrollingEnabled = enable; }
	bool isHorizontalScrollingEnabled() const				{ return mHorizontalScrollingEnabled; }

	//! Signal emitted whenever the content scrolls (both by dragging and when decelerating).
	ci::signals::Signal<void ()>& getSignalDidScroll()		{ return mSignalDidScroll; }
	//! Signal emitted when dragging begins.
	ci::signals::Signal<void ()>& getSignalDragBegin()		{ return mSignalDragBegin; }
	//! Signal emitted when dragging ends.
	ci::signals::Signal<void ()>& getSignalDragEnd()		{ return mSignalDragEnd; }

  protected:
	void layout()	override;
	void update()	override;

	bool touchesBegan( cinder::app::TouchEvent &event )	override;
	bool touchesMoved( ci::app::TouchEvent &event )	override;
	bool touchesEnded( ci::app::TouchEvent &event )	override;

	virtual void				onDecelerationEnded()	{}
	virtual const ci::Rectf&	getDeceleratingBoundaries() const;

	void calcContentSize();
	void calcOffsetBoundaries();
	void updateOffset( const ci::vec2 &currentPos, const ci::vec2 &previousPos );
	void updateDeceleratingOffset();

	// --------------------------------------------
	// Touch tracking
	struct StoredTouch {
		ci::vec2	position;
		double		eventSeconds;
	};

	std::list<StoredTouch>	mStoredTouches;
	StoredTouch				mFirstTouch;
	ci::vec2				mTouchVelocity;
	size_t					mMaxStoredTouches = 10;
	// --------------------------------------------

	bool					mDragging		= false;
	bool					mDecelerating	= false;
	ci::vec2				mTargetOffset;

  private:
	void updateContentViewOffset( const ci::vec2 &offset );
	void storeTouchPos( const ci::vec2 &pos );
	void calcTouchVelocity();

	class ContentView;
	std::shared_ptr<ContentView>	mContentView;
  	ci::Anim<ci::vec2>				mContentOffset;
	ci::vec2						mContentSize;

	ci::Rectf				mOffsetBoundaries = ci::Rectf::zero();

	float mDecelerationFactorInside			= 0.05f;
	float mDecelerationFactorOutside		= 0.15f;
	float mConstraintStiffness				= 0.2f;
	float mStretchFactor					= 0.5f;
	float mMinVelocityConsideredAsStopped	= 10;
	float mMinOffsetUntilStopped			= 0.1f;  // TODO: add max offset too (but should still move smoothly perhaps tanh).
	float mMaxSpeed							= 300.0f;

	bool							mScrollingEnabled = true;
	bool							mVerticalScrollingEnabled = true;
	bool							mHorizontalScrollingEnabled = true;

	ci::signals::Signal<void ()>	mSignalDidScroll, mSignalDragBegin, mSignalDragEnd;
};

class PagingScrollView : public ScrollView {
  public:
	enum Axis {
		HORIZONTAL,
		VERTICAL
	};

	enum LayoutMode {
		SNAP_TO_BOUNDS,
		SNAP_TO_CONTENT_BOUNDS
	};

	PagingScrollView( const ci::Rectf &bounds = ci::Rectf::zero() );
	virtual ~PagingScrollView();

	void addContentView( const ViewRef &view, bool updateContentLayout = true ) override;
	void removeAllSubviews() override;

	void nextPage( bool animate = true );
	void previousPage( bool animate = true );

	void setPage( size_t index, bool animate = true );
	size_t getPage() const						{ return mCurrentPageIndex; }
	size_t getNumPages() const;

	ci::vec2 getTargetOffsetForPage( size_t index ) const;

	void setAxis( Axis axis );
	Axis getAxis() const						{ return mAxis; }

	void		setLayoutMode( LayoutMode mode );
	LayoutMode	getLayoutMode() const			{ return mLayoutMode; }

	void setPageMargin( const ci::vec2 &margin );
	const ci::vec2&	getPageMargin() const	{ return mPageMargin; }

	//! Set the threshold to consider a velocity as a swipe during touchesEnded (default: 500).
	void	setSwipeVelocityThreshold( float value )	{ mSwipeVelocityThreshold = value; }
	//! Returns the threshold to consider a velocity as a swipe during touchesEnded (default: 500).
	float	getSwipeVelocityThreshold() const			{ return mSwipeVelocityThreshold; }
	//! Sets the minimum distance (pixels) to discount a touch sequence from being counted as a swipe (default: 50).
	void	setSwipeDistanceThreshold( float value )	{ mSwipeDistanceThreshold = value; }
	//! Returns the minimum distance (pixels) to discount a touch sequence from being counted as a swipe (default: 50).
	float	getSwipeDistanceThreshold() const			{ return mSwipeDistanceThreshold; }

	ci::signals::Signal<void ()>& getSignalPageWillChange()		{ return mSignalPageWillChange; }
	ci::signals::Signal<void ()>& getSignalPageDidChange()		{ return mSignalPageDidChange; }

  protected:

	void layout()		override;
	bool touchesEnded( ci::app::TouchEvent &event )	override;

	void				onDecelerationEnded() override;
	const ci::Rectf&	getDeceleratingBoundaries() const	override;

	void layoutPages();
	void layoutPage( size_t index );
	void calcDeceleratingBoundaries();
	void handlePageUpdate( bool animate );
	bool isOnFirstPage() const;
	bool isOnLastPage() const;

  private:

	Axis			mAxis;
	LayoutMode		mLayoutMode = LayoutMode::SNAP_TO_BOUNDS;
	size_t			mCurrentPageIndex = 0;
	ci::vec2		mPageMargin = ci::vec2( 0 );
	ci::Rectf		mDeceleratingBoundaries = ci::Rectf::zero();
	float			mSwipeVelocityThreshold	= 500;
	float			mSwipeDistanceThreshold	= 50;
	bool			mPageIsChangingAnimated = false;

	ci::signals::Signal<void ()>	mSignalPageWillChange, mSignalPageDidChange;
};

} // namespace ui
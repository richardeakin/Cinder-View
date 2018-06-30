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

#include "ui/ScrollView.h"
#include "ui/Graph.h"
#include "cinder/Log.h"

using namespace ci;
using namespace std;

//#define LOG_SCROLL_CONTENT( stream )	CI_LOG_I( stream )
#define LOG_SCROLL_CONTENT( stream )	( (void)( 0 ) )

#define LOG_SCROLL_TRACKING( stream )	CI_LOG_I( stream )
////#define LOG_SCROLL_TRACKING( stream )	( (void)( 0 ) )

namespace ui {

// ----------------------------------------------------------------------------------------------------
// ScrollView::ContentView
// ----------------------------------------------------------------------------------------------------

class ScrollView::ContentView : public View {
  public:
	ContentView( ScrollView *parent )
		: View( parent->getBoundsLocal() )
	{
	}
};

// ----------------------------------------------------------------------------------------------------
// ScrollView
// ----------------------------------------------------------------------------------------------------

ScrollView::ScrollView( const ci::Rectf &bounds )
	: View( bounds )
{
	setClipEnabled();
	setInterceptsTouches();

	mContentView = make_shared<ContentView>( this );
	mContentView->setLabel( "ScrollView-ContentView" );
	addSubview( mContentView );

	mSwipeTracker = make_unique<SwipeTracker>();
}

ScrollView::~ScrollView()
{
}

void ScrollView::addContentView( const ViewRef &view, bool updateContentLayout )
{
	mContentView->addSubview( view );

	if( updateContentLayout ) {
		calcContentSize();
		calcOffsetBoundaries();
	}
}

void ScrollView::addContentViews( const vector<ViewRef> &views )
{
	for( const auto &view : views ) {
		addContentView( view, false );
	}

	calcContentSize();
	calcOffsetBoundaries();
}

View* ScrollView::getContentView()
{
	return mContentView.get();
}

const View* ScrollView::getContentView() const
{
	return mContentView.get();
}

ViewRef ScrollView::getContentView( size_t index ) const
{
	if( index >= mContentView->getSubviews().size() ) {
		return nullptr;
	}

	return mContentView->getSubview( index );
}

size_t ScrollView::getNumContentViews() const
{
	return mContentView->getSubviews().size();
}

ci::vec2 ScrollView::convertPointToParent( const ViewRef &contentView ) const
{
	vec2 offset = mContentView->getPos();

	return contentView->getPos() + offset;
}

void ScrollView::setContentOffset( const ci::vec2 &offset, bool animated )
{
	if( animated ) {
		mTargetOffset = offset;
		mDecelerating = true;
		mOffsetBoundaries = Rectf( offset.x, offset.y, offset.x, offset.y );
	}
	else {
		mContentOffset = offset;
	}
}

void ScrollView::calcContentSize()
{
	// Start with the size of the ScrollView, then increase content size as necessary.
	mContentView->setSize( getSize() );
	if( mContentView->getLayout() ) {
		LOG_SCROLL_CONTENT( "(using Layout)" );
		mContentView->getLayout()->layout( mContentView.get() );
	}

	vec2 size = mContentView->getSize();
	for( const auto &view : mContentView->getSubviews() ) {
		auto viewBounds = view->getBounds();
		LOG_SCROLL_CONTENT( "view: " << view );
		LOG_SCROLL_CONTENT( "\t- size before: " << size );
		if( size.x < viewBounds.x2 )
			size.x = viewBounds.x2;
		if( size.y < viewBounds.y2 )
			size.y = viewBounds.y2;

		LOG_SCROLL_CONTENT( "\t- size after: " << size );
	}

	mContentView->setSize( size ); // TODO: should this trigger layout or not?

	if( mDisableScrollingWhenContentFits && size.x <= getWidth() && size.y <= getHeight() ) {
		setScrollingEnabled( false );
	}

	LOG_SCROLL_CONTENT( "content size: " << mContentSize );
}

void ScrollView::calcOffsetBoundaries()
{
	const auto &contentSize = mContentView->getSize();
	mOffsetBoundaries = Rectf( 0, 0, contentSize.x - getWidth(), contentSize.y - getHeight() );
	LOG_SCROLL_CONTENT( "mOffsetBoundaries (before): " << mOffsetBoundaries );

	if( mOffsetBoundaries.x2 < 0 )
		mOffsetBoundaries.x2 = 0;
	if( mOffsetBoundaries.y2 < 0 )
		mOffsetBoundaries.y2 = 0;

	LOG_SCROLL_CONTENT( "mContentSize: " << contentSize << ", mOffsetBoundaries: " << mOffsetBoundaries << ", getSize(): " << getSize() );
}

void ScrollView::layout()
{
	calcContentSize();
	calcOffsetBoundaries();
}

void ScrollView::update()
{
	bool hasContentViews = ! mContentView->getSubviews().empty();
	if( hasContentViews && ! isUserInteracting() && isDecelerating() ) {
		updateDeceleratingOffset();
	}
}

void ScrollView::updateOffset( const ci::vec2 &currentPos, const ci::vec2 &previousPos )
{
	vec2 diff = currentPos - previousPos;
	vec2 offset = mContentOffset() - diff;

	// if offset will be out of boundaries, apply stretch factor
	if( offset.x < mOffsetBoundaries.x1 || offset.x > mOffsetBoundaries.x2 )
		diff.x *= mStretchFactor;
	if( offset.y < mOffsetBoundaries.y1 || offset.y > mOffsetBoundaries.y2 )
		diff.y *= mStretchFactor;

	vec2 contentOffset = mContentOffset() - diff;
	updateContentViewOffset( contentOffset );

	// restrict the target offset to content boundaries.
	mTargetOffset = mOffsetBoundaries.closestPoint( mContentOffset );

	LOG_SCROLL_TRACKING( "mContentOffset: " << mContentOffset() << ", mTargetOffset: " << mTargetOffset );
}

void ScrollView::updateDeceleratingOffset()
{
	// apply velocity to content offset
	float deltaTime = 1.0f / (float)getGraph()->getTargetFrameRate();
	vec2 contentOffset = mContentOffset() - mScrollVelocity * deltaTime;

	const Rectf &boundaries = getDeceleratingBoundaries();
	float decelFactor = boundaries.contains( contentOffset ) ? mDecelerationFactorInside : mDecelerationFactorOutside;
	mScrollVelocity *= 1 - decelFactor;

	mTargetOffset = boundaries.closestPoint( contentOffset );

	vec2 delta = mTargetOffset - contentOffset;
	vec2 velocity = delta * mConstraintStiffness;
	float speed = length( velocity );
	if( speed > mMaxSpeed ) {
		velocity = velocity * mMaxSpeed / speed;
	}
	contentOffset += velocity;

	auto velLength = length( mScrollVelocity );
	auto offsetLength = length( mTargetOffset - contentOffset );

	if( velLength < mMinVelocityConsideredAsStopped && offsetLength < mMinOffsetUntilStopped ) {
		// snap to boundaries and finish deceleration
		contentOffset = mTargetOffset;
		mDecelerating = false;
		mScrollVelocity = vec2( 0 );

		// make sure scroll signal gets called before a page ended signal
		updateContentViewOffset( contentOffset );
		mSignalDidScroll.emit(); 
		onDecelerationEnded();
	}
	else {
		updateContentViewOffset( contentOffset );
		mSignalDidScroll.emit();
	}
}

void ScrollView::updateContentViewOffset( const vec2 &offset )
{
	if( mScrollingEnabled && mVerticalScrollingEnabled )
		mContentOffset().y = offset.y;
	if( mScrollingEnabled && mHorizontalScrollingEnabled )
		mContentOffset().x = offset.x;

	// Move all of the content's position based on mContentOffset. Their world positions will be marked dirty.
	mContentView->setPos( - mContentOffset() );
}

const Rectf& ScrollView::getDeceleratingBoundaries() const
{
	return mOffsetBoundaries;
}

// ----------------------------------------------------------------------------------------------------
// Events
// ----------------------------------------------------------------------------------------------------

bool ScrollView::touchesBegan( app::TouchEvent &event )
{
	if( ! getActiveTouches().empty() ) {
		return false;
	}	

	auto &firstTouch = event.getTouches().front();
	vec2 pos = toLocal( firstTouch.getPos() );
	LOG_SCROLL_TRACKING( "intercepting touches: " << mInterceptedTouches.size() << ",  pos:" << pos );

	mSwipeTracker->clear();
	mSwipeTracker->storeTouchPos( pos, getGraph()->getElapsedSeconds() );
	mSwipeVelocity = vec2( 0 );

	mDragging = false; // will set to true once touchesMoved() is fired
	calcOffsetBoundaries(); // reset offset boundaries, which may have been modified if content offset is animating

	firstTouch.setHandled();
	return true;
}

bool ScrollView::touchesMoved( app::TouchEvent &event )
{
	vec2 pos = toLocal( event.getTouches().front().getPos() );
	vec2 lastPos = mSwipeTracker->getLastTouchPos();
	updateOffset( pos, lastPos );
	mSwipeTracker->storeTouchPos( pos, getGraph()->getElapsedSeconds() );
	
	LOG_SCROLL_TRACKING( "intercepting touches: " << mInterceptedTouches.size() << ",  pos:" << pos );

	if( ! mDragging ) {
		mDragging = true;
		mSignalDragBegin.emit();
	}

	if( ! mContentView->getSubviews().empty() )
		mSignalDidScroll.emit();

	return true;
}

bool ScrollView::touchesEnded( app::TouchEvent &event )
{
	vec2 pos = toLocal( event.getTouches().front().getPos() );
	vec2 lastPos = mSwipeTracker->getLastTouchPos();
	LOG_SCROLL_TRACKING( "intercepting touches: " << mInterceptedTouches.size() << ",  pos:" << pos );

	updateOffset( pos, lastPos );
	mSwipeTracker->storeTouchPos( pos, getGraph()->getElapsedSeconds() );

	mSwipeVelocity = mSwipeTracker->calcSwipeVelocity();
	mScrollVelocity = mSwipeVelocity;

	if( mDragging ) {
		mDragging = false;
		mSignalDragEnd.emit();
	}

	if( ! mContentView->getSubviews().empty() ) {
		mDecelerating = true;
		mSignalDidScroll.emit();
	}

	return true;
}

bool ScrollView::shouldInterceptTouches( std::vector<ci::app::TouchEvent::Touch> &touches )
{
	// TODO: perform hit tests on content views and see if there is one under a touch that is interactive and non hidden
	// - need to indicate back to color which touches?
	//     - might be able to do that by calling setHandled() on the touch itself

	LOG_SCROLL_TRACKING( "frame: " << getGraph()->getCurrentFrame() << ", touches: " << touches.size() );


	return true;
}

bool ScrollView::shouldInterceptedTouchesContinue( std::vector<ci::app::TouchEvent::Touch> &touches )
{
	CI_ASSERT( mSwipeTracker->getNumStoredTouches() > 0 );

	const double durationForTap = 0.03f;

	double duration = mSwipeTracker->getLastTouchTime() - mSwipeTracker->getFirstTouchTime();

	vec2 dist = mSwipeTracker->calcSwipeDistance(); // TODO: use

	LOG_SCROLL_TRACKING( "frame: " << getGraph()->getCurrentFrame() << ", touches: " << touches.size() << ", dragging: " << mDragging 
		<< ", interacting: " << isUserInteracting() << ", gesture duration: " << duration << ", dist: " << dist );

	// determine if complete gesture duration was short enough to be considered a tap
	// TODO NEXT: mDragging isn't set during touchesBegan() currently - should it?
	// - otherwise need to know that there is an active touch being processed.
	// - maybe an intercepted touch should go through the full touchesBegan() procedure instead of immediately returning?
	if( ! isUserInteracting() && duration < durationForTap ) {
		LOG_SCROLL_TRACKING( "\t- tap: allow touch to continue." );
		return true;
	}

	return false;
}

// ----------------------------------------------------------------------------------------------------
// PagingScrollView
// ----------------------------------------------------------------------------------------------------

PagingScrollView::PagingScrollView( const Rectf &bounds )
	: ScrollView( bounds )
{
	// Default to horizontal scrolling, but nothing yet to layout so skip that
	mAxis = Axis::HORIZONTAL;
	setVerticalScrollingEnabled( false );
}

PagingScrollView::~PagingScrollView()
{
}

void PagingScrollView::addContentView( const ViewRef &view, bool updateContentLayout )
{
	auto contentView = getContentView();
	contentView->addSubview( view );
	layoutPage( contentView->getSubviews().size() - 1 );

	if( updateContentLayout ) {
		calcContentSize();
		calcOffsetBoundaries();
		calcDeceleratingBoundaries();
	}
}

ViewRef	PagingScrollView::getPageView( size_t index ) const
{
	if( index >= getNumPages() ) {
		CI_ASSERT_NOT_REACHABLE();
		return nullptr;
	}

	return getContentView( index );
}

void PagingScrollView::removeAllSubviews()
{
	ScrollView::removeAllSubviews();

	calcDeceleratingBoundaries();
	mCurrentPageIndex = 0;
}

size_t PagingScrollView::getNumPages() const
{
	return getNumContentViews();
}

void PagingScrollView::nextPage( bool animate )
{
	if( mCurrentPageIndex == getNumContentViews() - 1 )
		return;

	mSignalPageWillChange.emit( mCurrentPageIndex + 1 );
	mCurrentPageIndex++;
	handlePageUpdate( animate );
}

void PagingScrollView::previousPage( bool animate )
{
	if( mCurrentPageIndex == 0 )
		return;

	mSignalPageWillChange.emit( mCurrentPageIndex - 1 );
	mCurrentPageIndex--;
	handlePageUpdate( animate );
}

void PagingScrollView::setPage( size_t index, bool animate )
{
	CI_ASSERT_MSG( index < getNumContentViews(), "index out of bounds" );

	mSignalPageWillChange.emit( index );
	mCurrentPageIndex = index;
	handlePageUpdate( animate );
}

void PagingScrollView::setAxis( Axis axis )
{
	if( mAxis == axis )
		return;

	mAxis = axis;
	if( mAxis == Axis::HORIZONTAL ) {
		setHorizontalScrollingEnabled( true );
		setVerticalScrollingEnabled( false );
	}
	else {
		setHorizontalScrollingEnabled( false );
		setVerticalScrollingEnabled( true );
	}

	layoutPages( true );
}

void PagingScrollView::setLayoutMode( LayoutMode mode )
{
	if( mLayoutMode == mode )
		return;

	mLayoutMode = mode;
	layoutPages( true );
}

void PagingScrollView::setPageMargin( const vec2 &margin )
{
	mPageMargin = margin;
	layoutPages( true );
}

void PagingScrollView::layout()
{
	layoutPages( false );
	ScrollView::layout();
}

bool PagingScrollView::touchesEnded( app::TouchEvent &event )
{
	bool wasDecelerating = isDecelerating();
	bool handled = ScrollView::touchesEnded( event );

	if( getNumPages() == 0 )
		return handled;

	vec2 pageOffset = getTargetOffsetForPage( mCurrentPageIndex );

	vec2 diff = mSwipeTracker->calcSwipeDistance();

	if( mAxis == HORIZONTAL ) {
		float halfSize = getWidth() / 2.0f;
		bool didSwipe = ( fabsf( mSwipeVelocity.x ) > mSwipeVelocityThreshold ) && ( fabsf( diff.x ) > mSwipeDistanceThreshold );

		if( ( didSwipe && diff.x < 0 ) || ( ! wasDecelerating && ( getContentOffset().x - pageOffset.x > halfSize && ! isOnLastPage() ) ) )
			nextPage();
		else if( ( didSwipe && diff.x > 0 ) || ( ! wasDecelerating && ( pageOffset.x - getContentOffset().x > halfSize && ! isOnFirstPage() ) ) )
			previousPage();
	}
	else {
		// VERTICAL
		float halfSize = getHeight() / 2.0f;
		bool didSwipe = ( fabsf( mSwipeVelocity.y ) > mSwipeVelocityThreshold ) && ( fabsf( diff.y ) > mSwipeDistanceThreshold );

		if( ( didSwipe && diff.y < 0 ) || ( ! wasDecelerating && ( getContentOffset().y - pageOffset.y > halfSize && ! isOnLastPage() ) ) )
			nextPage();
		else if( ( didSwipe && diff.y > 0 ) || ( ! wasDecelerating && ( pageOffset.y - getContentOffset().y > halfSize && ! isOnFirstPage() ) ) )
			previousPage();
	}

	return handled;
}

void PagingScrollView::layoutPages( bool updateBoundaries )
{
	for( size_t i = 0; i < getNumPages(); i++ ) {
		layoutPage( i );
	}

	if( updateBoundaries ) {
		calcContentSize();
		calcOffsetBoundaries();
	}

	calcDeceleratingBoundaries();
}

void PagingScrollView::layoutPage( size_t index )
{
	CI_ASSERT( index < getNumPages() );

	auto &pageView = getContentView()->getSubview( index );

	if( mLayoutMode == SNAP_TO_BOUNDS ) {
		if( mAxis == HORIZONTAL ) {
			// layout horizontal
			float originX = getWidth() * index;
			pageView->setBounds( Rectf( originX + mPageMargin.x, mPageMargin.y, originX + getWidth() - mPageMargin.x, getHeight() - mPageMargin.y ) );
		}
		else {
			// layout vertical
			float originY = getHeight() * index;
			pageView->setBounds( Rectf( mPageMargin.x, originY + mPageMargin.y, getWidth() - mPageMargin.x, originY + getHeight() - mPageMargin.y ) );
		}
	}
	else {
		// SNAP_TO_CONTENT_BOUNDS
		if( index == 0 ) {
			pageView->setBounds( Rectf( mPageMargin.x, mPageMargin.y, mPageMargin.x + pageView->getBounds().getWidth(), mPageMargin.y + pageView->getBounds().getHeight() ) );
		}
		else {
			Rectf prevBounds = getContentView()->getSubview( index - 1 )->getBounds();
			if( mAxis == HORIZONTAL ) {
				pageView->setBounds( Rectf( prevBounds.x2 + mPageMargin.x, mPageMargin.y, prevBounds.x2 + mPageMargin.x + pageView->getBounds().getWidth(), mPageMargin.y + prevBounds.getHeight() ) );
			}
			else {
				// layout vertical
				pageView->setBounds( Rectf( mPageMargin.x, prevBounds.y2 + mPageMargin.y, mPageMargin.x + prevBounds.getHeight(), prevBounds.y2 + mPageMargin.y + pageView->getBounds().getHeight() ) );
			}
		}
	}

	LOG_SCROLL_CONTENT( "index: " << index << ", bounds: " << getContentView()->getBounds() );
}

vec2 PagingScrollView::getTargetOffsetForPage( size_t index ) const
{
	CI_ASSERT( index < getNumPages() );

	Rectf contentBounds = getContentView()->getSubview( index )->getBounds();
	return mAxis == HORIZONTAL ? vec2( contentBounds.x1 - mPageMargin.x, 0 ) : vec2( 0, contentBounds.y1 - mPageMargin.y );
}

void ScrollView::onDecelerationEnded()
{
	calcOffsetBoundaries();
}

void PagingScrollView::onDecelerationEnded()
{
	if( mPageIsChangingAnimated ) {
		mPageIsChangingAnimated = false;
		mSignalPageDidChange.emit();
	}
}

const Rectf& PagingScrollView::getDeceleratingBoundaries() const
{
	return mDeceleratingBoundaries;
}

void PagingScrollView::calcDeceleratingBoundaries()
{
	if( getNumPages() == 0 ) {
		mDeceleratingBoundaries = Rectf( 0, 0, 0, 0 );
		return;
	}

	vec2 pageOffset = getTargetOffsetForPage( mCurrentPageIndex );

	if( mAxis == HORIZONTAL )
		mDeceleratingBoundaries = Rectf( pageOffset.x, 0, pageOffset.x, 0 );
	else
		mDeceleratingBoundaries = Rectf( 0, pageOffset.y, 0, pageOffset.y );
}

void PagingScrollView::handlePageUpdate( bool animate )
{
	calcDeceleratingBoundaries();
	if( animate ) {
		mDecelerating = true;
		mPageIsChangingAnimated = true;
	}
	else {
		// jump to the current offset
		updateDeceleratingOffset();
		setContentOffset( mTargetOffset, false );
		mSignalPageDidChange.emit();
	}

	LOG_SCROLL_CONTENT( "current page: " << mCurrentPageIndex );
}

bool PagingScrollView::isOnFirstPage() const
{
	return mCurrentPageIndex == 0;
}

bool PagingScrollView::isOnLastPage() const
{
	return mCurrentPageIndex == getNumPages() - 1;
}

} // namespace ui

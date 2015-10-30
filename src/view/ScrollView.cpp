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

#include "view/ScrollView.h"

#include "cinder/gl/scoped.h"
#include "cinder/gl/wrapper.h"

#include "cinder/app/App.h" // TODO: remove me. currently used for app::getElapsedSeconds()

using namespace ci;
using namespace std;

//#define LOG_SCROLL( stream )	CI_LOG_I( stream )
#define LOG_SCROLL( stream )	( (void)( 0 ) )

namespace view {

class ScrollView::ContentView : public View {
  public:
	ContentView( ScrollView *parent )
		: View( parent->getBoundsLocal() )
	{
	}
};

ScrollView::ScrollView( const ci::Rectf &bounds )
	: View( bounds )
{
	setClipEnabled();
	mContentView = make_shared<ContentView>( this );
	addSubview( mContentView );
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

size_t ScrollView::getNumContentViews() const
{
	return mContentView->getSubviews().size();
}

void ScrollView::setContentOffset( const ci::vec2 &offset )
{
	mContentOffset = offset;
}

void ScrollView::calcContentSize()
{
	vec2 size = vec2( 0 );
	for( const auto &view : mContentView->getSubviews() ) {
		auto viewBounds = view->getBounds();
		LOG_SCROLL( "view: " << view );
		LOG_SCROLL( "\t- size before: " << size );
		if( size.x < viewBounds.x2 )
			size.x = viewBounds.x2;
		if( size.y < viewBounds.y2 )
			size.y = viewBounds.y2;

		LOG_SCROLL( "\t- size after: " << size );
	}

	mContentSize = size; // TODO: can remove mContentSize and just use mContentView->getSize() instead?
	mContentView->setSize( size ); // TODO: should this trigger layout or not?

	LOG_SCROLL( "content size: " << mContentSize );
}

void ScrollView::calcOffsetBoundaries()
{
	mOffsetBoundaries = Rectf( 0, 0, mContentSize.x - getWidth(), mContentSize.y - getHeight() );
	LOG_SCROLL( "mOffsetBoundaries (before): " << mOffsetBoundaries );

	if( mOffsetBoundaries.x2 < 0 )
		mOffsetBoundaries.x2 = 0;
	if( mOffsetBoundaries.y2 < 0 )
		mOffsetBoundaries.y2 = 0;

	LOG_SCROLL( "mContentSize: " << mContentSize << ", mOffsetBoundaries: " << mOffsetBoundaries << ", getSize(): " << getSize() );
}

void ScrollView::layout()
{
	calcOffsetBoundaries();
}

void ScrollView::update()
{
	bool hasContentViews = ! mContentView->getSubviews().empty();
	if( hasContentViews && ! isUserInteracting() && isDecelerating() ) {
		updateDeceleratingOffset();
	}
}

void ScrollView::draw()
{
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

	LOG_SCROLL( "mContentOffset: " << mContentOffset() << ", mTargetOffset: " << mTargetOffset );
}

void ScrollView::updateDeceleratingOffset()
{
	// apply velocity to content offset
	const float targetFrameRate = app::getFrameRate(); // TODO: need to get time + framerate from a source that is independant of app (something akin to a Context)
	float deltaTime = 1.0f / targetFrameRate;
	vec2 contentOffset = mContentOffset() - mTouchVelocity * deltaTime;

	const Rectf &boundaries = getDeceleratingBoundaries();
	float decelFactor = boundaries.contains( contentOffset ) ? mDecelerationFactorInside : mDecelerationFactorOutside;
	mTouchVelocity *= 1 - decelFactor;

	mTargetOffset = boundaries.closestPoint( contentOffset );

	vec2 delta = mTargetOffset - contentOffset;
	vec2 velocity = delta * mConstraintStiffness;
	float speed = length( velocity );
	if( speed > mMaxSpeed ) {
		velocity = velocity * mMaxSpeed / speed;
	}
	contentOffset += velocity;

	auto velLength = length( mTouchVelocity );
	auto offsetLength = length( mTargetOffset - contentOffset );

	if( velLength < mMinVelocityConsideredAsStopped && offsetLength < mMinOffsetUntilStopped ) {
		contentOffset = mTargetOffset;
		mDecelerating = false;
	}

	updateContentViewOffset( contentOffset );
	mSignalDidScroll.emit();
}

void ScrollView::updateContentViewOffset( const vec2 &offset )
{
	if( mVerticalScrollingEnabled )
		mContentOffset().y = offset.y;
	if( mHorizontalScrollingEnabled )
		mContentOffset().x = offset.x;

	// Move all of the content's position based on mContentOffset. A full re-layout isn't necessary, so just mark world positions as dirty
	mContentView->setPos( - mContentOffset(), false );
	mContentView->setWorldPosDirty();
}

void ScrollView::calcTouchVelocity()
{
	if( mStoredTouches.size() < 2 )
		return;

	vec2 touchVelocity = vec2( 0 );
	int samples = 0;
	auto lastIt = --mStoredTouches.end();
	for( auto it = mStoredTouches.begin(); it != lastIt; ++it ) {
		auto nextIt = it;
		++nextIt;
		double dt = nextIt->eventSeconds - it->eventSeconds;
		if( dt > 0.001 ) {
			touchVelocity += ( nextIt->position - it->position ) / float( dt );
			samples += 1;
		}
	}

	if( samples > 0 ) {
		mTouchVelocity = touchVelocity / float( samples );
	}
}


// ----------------------------------------------------------------------------------------------------
// MARK: - Events
// ----------------------------------------------------------------------------------------------------

bool ScrollView::touchesBegan( const app::TouchEvent &event )
{
	const auto &firstTouch = event.getTouches().front();
	vec2 pos = toLocal( firstTouch.getPos() );
	mStoredTouches.clear();
	storeTouchPos( pos );

	mFirstTouch = mStoredTouches.front();

	return true;
}

bool ScrollView::touchesMoved( const ci::app::TouchEvent &event )
{
	vec2 pos = toLocal( event.getTouches().front().getPos() );
	vec2 lastPos = mStoredTouches.back().position;
	updateOffset( pos, lastPos );
	storeTouchPos( pos );

	if( ! mContentView->getSubviews().empty() )
		mSignalDidScroll.emit();

	return true;
}

bool ScrollView::touchesEnded( const ci::app::TouchEvent &event )
{
	vec2 pos = toLocal( event.getTouches().front().getPos() );
	vec2 lastPos = mStoredTouches.back().position;
	updateOffset( pos, lastPos );
	storeTouchPos( pos );

	calcTouchVelocity();

	if( ! mContentView->getSubviews().empty() ) {
		mDecelerating = true;
		mSignalDidScroll.emit();
	}

	return true;
}

// ----------------------------------------------------------------------------------------------------
// MARK: - ScrollView Other
// ----------------------------------------------------------------------------------------------------

const Rectf& ScrollView::getDeceleratingBoundaries() const
{
	return mOffsetBoundaries;
}

void ScrollView::storeTouchPos( const ci::vec2 &pos )
{
	if( mStoredTouches.size() >= mMaxStoredTouches )
		mStoredTouches.pop_front();

	StoredTouch touch;
	touch.position = pos;
	touch.eventSeconds = app::getElapsedSeconds();
	mStoredTouches.push_back( touch );
}

// ----------------------------------------------------------------------------------------------------
// MARK: - PagingScrollView
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

	mCurrentPageIndex++;
	handlePageUpdate( animate );
}

void PagingScrollView::previousPage( bool animate )
{
	if( mCurrentPageIndex == 0 )
		return;

	mCurrentPageIndex--;
	handlePageUpdate( animate );
}

void PagingScrollView::setPage( size_t index, bool animate )
{
	CI_ASSERT_MSG( index < getNumContentViews(), "index out of bounds" );

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

	layoutPages();
}

void PagingScrollView::setLayoutMode( LayoutMode mode )
{
	if( mLayoutMode == mode )
		return;

	mLayoutMode = mode;
	layoutPages();
}

void PagingScrollView::setPageMargin( const vec2 &margin )
{
	mPageMargin = margin;
	layoutPages();
}

void PagingScrollView::layout()
{
	layoutPages();
	ScrollView::layout();
}

bool PagingScrollView::touchesEnded( const app::TouchEvent &event )
{
	bool wasDecelerating = isDecelerating();
	bool parentResult = ScrollView::touchesEnded( event );

	if( getNumPages() == 0 )
		return parentResult;

	vec2 pageOffset = getTargetOffsetForPage( mCurrentPageIndex );

	vec2 diff = mStoredTouches.back().position - mFirstTouch.position;

	if( mAxis == HORIZONTAL ) {
		float halfSize = getWidth() / 2.0f;
		bool didSwipe = ( fabsf( mTouchVelocity.x ) > mSwipeVelocityThreshold ) && ( fabsf( diff.x ) > mSwipeDistanceThreshold );

		if( ( didSwipe && diff.x < 0 ) || ( ! wasDecelerating && ( getContentOffset().x - pageOffset.x > halfSize && ! isOnLastPage() ) ) )
			nextPage();
		else if( ( didSwipe && diff.x > 0 ) || ( ! wasDecelerating && ( pageOffset.x - getContentOffset().x > halfSize && ! isOnFirstPage() ) ) )
			previousPage();
	}
	else {
		// VERTICAL
		float halfSize = getHeight() / 2.0f;
		bool didSwipe = ( fabsf( mTouchVelocity.y ) > mSwipeVelocityThreshold ) && ( fabsf( diff.y ) > mSwipeDistanceThreshold );

		if( ( didSwipe && diff.y < 0 ) || ( ! wasDecelerating && ( getContentOffset().y - pageOffset.y > halfSize && ! isOnLastPage() ) ) )
			nextPage();
		else if( ( didSwipe && diff.y > 0 ) || ( ! wasDecelerating && ( pageOffset.y - getContentOffset().y > halfSize && ! isOnFirstPage() ) ) )
			previousPage();
	}
}

void PagingScrollView::layoutPages()
{
	for( size_t i = 0; i < getNumPages(); i++ ) {
		layoutPage( i );
	}

	calcContentSize();
	calcOffsetBoundaries();
	calcDeceleratingBoundaries();
}

void PagingScrollView::layoutPage( size_t index )
{
	CI_ASSERT( index < getNumPages() );

	auto &pageView = getContentView()->getSubview( index );

	if( mLayoutMode == SNAP_TO_BOUNDS ) {
		if( mAxis == HORIZONTAL ) {
			// layout horiztonal
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

	LOG_SCROLL( "index: " << index << ", bounds: " << contentView->getBounds() );
}

vec2 PagingScrollView::getTargetOffsetForPage( size_t index ) const
{
	CI_ASSERT( index < getNumPages() );

	Rectf contentBounds = getContentView()->getSubview( index )->getBounds();
	return mAxis == HORIZONTAL ? vec2( contentBounds.x1 - mPageMargin.x, 0 ) : vec2( 0, contentBounds.y1 - mPageMargin.y );
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
	if( animate )
		mDecelerating = true;
	else {
		// jump to the current offset
		updateDeceleratingOffset();
		setContentOffset( mTargetOffset );
	}

	LOG_SCROLL( "current page: " << mCurrentPageIndex );
	mSignalDidChangePage.emit();
}

bool PagingScrollView::isOnFirstPage() const
{
	return mCurrentPageIndex == 0;
}

bool PagingScrollView::isOnLastPage() const
{
	return mCurrentPageIndex == getNumPages() - 1;
}

} // namespace view

/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "ui/View.h"
#include "ui/Graph.h"

#include "glm/gtc/epsilon.hpp"

#include "cinder/CinderAssert.h"
#include "cinder/gl/gl.h"
#include "cinder/Log.h"
#include "cinder/System.h"

using namespace std;
using namespace ci;

namespace ui {

const float BOUNDS_EPSILON = 0.00001f;

// ----------------------------------------------------------------------------------------------------
// Responder
// ----------------------------------------------------------------------------------------------------

bool View::becomeFirstResponder()
{
	getGraph()->setFirstResponder( shared_from_this() );
	return true; // TODO: not sure if this needs to return a value or not
}

bool View::resignFirstResponder()
{
	if( ! isFirstResponder() )
		return false;

	getGraph()->resignFirstResponder();
	return true; // TODO: same as above return
}

bool View::isFirstResponder() const
{ 
	return this == getGraph()->getFirstResponder().get();
}

ViewRef	View::getNextResponder() const
{
	if( mNextResponder )
		return mNextResponder;
	else if( mParent )
		return mParent->shared_from_this();
	else
		return nullptr;
}

// ----------------------------------------------------------------------------------------------------
// View
// ----------------------------------------------------------------------------------------------------

View::View( const ci::Rectf &bounds )
	: mPos( bounds.getUpperLeft() ), mSize( vec2( bounds.x2 - bounds.x1, bounds.y2 - bounds.y1 ) )
{
}

View::~View()
{
	for( auto &subview : mSubviews )
		subview->mParent = nullptr;

	if( mLayer ) {
		if( isLayerRoot() )
			getGraph()->removeLayer( mLayer );
	}
}

void View::setPos( const vec2 &position )
{
	if( glm::all( glm::epsilonEqual( getPos(), position, BOUNDS_EPSILON ) ) )
		return;

	mPos = position;
	if( mBackground )
		mBackground->setPos( getPos() );

	setWorldPosDirty();
}

void View::setSize( const vec2 &size )
{
	if( glm::all( glm::epsilonEqual( getSize(), size, BOUNDS_EPSILON ) ) )
		return;

	mSize = glm::max( vec2( 0 ), size );
	if( mBackground )
		mBackground->setSize( mSize );

	setNeedsLayout();
}

void View::setBounds( const ci::Rectf &bounds )
{
	vec2 pos = bounds.getUpperLeft();
	if( ! glm::all( glm::epsilonEqual( getPos(), pos, BOUNDS_EPSILON ) ) ) {
		setPos( pos );
	}

	vec2 size = bounds.getSize();
	if( ! glm::all( glm::epsilonEqual( getSize(), size, BOUNDS_EPSILON ) ) ) {
		setSize( size );
	}
}

Rectf View::getBounds() const
{
	return Rectf( mPos().x, mPos().y, mPos().x + mSize().x, mPos().y + mSize().y );
}

Rectf View::getBoundsLocal() const
{
	return Rectf( vec2( 0 ), getSize() );
}

Rectf View::getBoundsForFrameBuffer() const
{
	return Rectf( vec2( 0 ), getSize() );
}

bool View::isBoundsAnimating() const
{
	return ( ! mPos.isComplete() || ! mSize.isComplete() );
}

bool View::isTransparent() const
{
	return mAlpha < 0.9999f;
}

bool View::isLayerRoot() const
{
	auto layer = getLayer();
	return ( layer && layer->getRootView() == this );
}

void View::setClipEnabled( bool enable )
{
	mClipEnabled = enable;
}

bool View::isClipEnabled() const
{
	return mClipEnabled;
}

void View::addSubview( const ViewRef &view )
{
	CI_ASSERT( view );
	CI_ASSERT( view.get() != this );

	// first set the parent to be us, which will remove it from any existing parent (including this view).
	view->setParent( this );
	mSubviews.push_back( view );

	setNeedsLayout();
	view->setNeedsLayout();
}

void View::addSubviews( const vector<ViewRef> &views )
{
	for( const auto &view : views ) {
		addSubview( view );
	}
}

void View::insertSubview( const ViewRef &view, size_t index )
{
	CI_ASSERT( view );
	CI_ASSERT( view.get() != this );
	CI_ASSERT( index <= mSubviews.size() );

	// first set the parent to be us, which will remove it from any existing parent (including this view).
	view->setParent( this );

	auto it = mSubviews.begin();
	std::advance( it, index );
	mSubviews.insert( it, view );
}

void View::insertSubviewAbove( const ViewRef &view, const ViewRef &viewBelow )
{
	CI_ASSERT( view && viewBelow );
	CI_ASSERT( view.get() != this && viewBelow.get() != this );

	auto it = std::find( mSubviews.begin(), mSubviews.end(), viewBelow );
	if( it == mSubviews.end() ) {
		CI_LOG_W( "viewBelow labeled '" << viewBelow->getLabel() << "' not a child of this View '" << getLabel() << "'" );
	}
	else if( it != ( mSubviews.end() - 1 ) ) {
		++it;
	}

	view->setParent( this );
	mSubviews.insert( it, view );
}

void View::insertSubviewBelow( const ViewRef &view, const ViewRef &viewAbove )
{
	CI_ASSERT( view && viewAbove );
	CI_ASSERT( view.get() != this && viewAbove.get() != this );

	auto it = std::find( mSubviews.begin(), mSubviews.end(), viewAbove );
	if( it == mSubviews.end() ) {
		CI_LOG_W( "viewAbove labeled '" << viewAbove->getLabel() << "' not a child of this View '" << getLabel() << "'" );
	}
	else if( it != mSubviews.begin() ) {
		--it;
	}

	view->setParent( this );
	mSubviews.insert( it, view );
}

void View::removeSubview( const ViewRef &view )
{
	for( auto it = mSubviews.begin(); it != mSubviews.end(); ++it ) {
		if( view == *it ) {
			view->mParent = nullptr;
			if( view->mAcceptsFirstResponder )
				view->resignFirstResponder();

			if( mIsIteratingSubviews )
				view->mMarkedForRemoval = true;
			else
				mSubviews.erase( it );

			return;
		}
	}
}

void View::removeAllSubviews()
{
	if( mIsIteratingSubviews ) {
		for( auto &view : mSubviews ) {
			view->mParent = nullptr;
			if( view->mAcceptsFirstResponder )
				view->resignFirstResponder();

			view->mMarkedForRemoval = true;
		}
	}
	else {
		for( auto &view : mSubviews ) {
			view->mParent = nullptr;
			if( view->mAcceptsFirstResponder )
				view->resignFirstResponder();
		}
		mSubviews.clear();
	}
}

void View::removeFromParent()
{
	if( ! mParent )
		return;

	mParent->removeSubview( shared_from_this() );
	setNeedsLayout();
}

ViewRef& View::getSubview( size_t index )
{
	CI_ASSERT( index < mSubviews.size() );

	auto iter = mSubviews.begin();
	std::advance( iter, index );
	return *iter;
}

const ViewRef& View::getSubview( size_t index ) const
{
	CI_ASSERT( index < mSubviews.size() );

	auto iter = mSubviews.begin();
	std::advance( iter, index );
	return *iter;
}

bool View::containsSubview( const ViewRef &view )
{
	return find( mSubviews.begin(), mSubviews.end(), view ) != mSubviews.end();
}

void View::setParent( View *parent )
{
	removeFromParent();
	mParent = parent;
	mGraph = parent->getGraph();
}

void View::setFillParentEnabled( bool enable )
{
	if( mFillParent == enable )
		return;

	mFillParent = enable;
	setNeedsLayout();
}

void View::setNeedsLayout()
{
	mNeedsLayout = true;

	for( const auto &subview : mSubviews ) {
		if( subview->mFillParent )
			subview->setNeedsLayout();
	}
}

void View::layoutIfNeeded()
{
	if( mNeedsLayout )
		layoutImpl();

	for( const auto &subview : mSubviews )
		subview->layoutIfNeeded();
}

void View::setWorldPosDirty()
{
	mWorldPosDirty = true;
	for( const auto &subview : mSubviews )
		subview->setWorldPosDirty();
}

void View::addFilter( const FilterRef &filter )
{
	mFilters.push_back( filter );
	if( isLayerRoot() ) {
		mLayer->setFiltersNeedConfiguration();
	}
}

void View::removeFilter( const FilterRef &filter )
{
	mFilters.erase( remove( mFilters.begin(), mFilters.end(), filter ), mFilters.end() );
}

void View::removeAllFilters()
{
	mFilters.clear();
}

void View::layoutImpl()
{
	mWorldPosDirty = true;

	if( mBackground )
		mBackground->layoutImpl();

	if( mFillParent ) {
		auto parent = getParent();
		if( parent ) {
			setPos( vec2( 0 ) );
			setSize( parent->getSize() );
		}
	}

	if( mLayout )
		mLayout->layout( this );

	layout();
	mNeedsLayout = false;
	mSignalViewDidLayout.emit();
}

void View::updateImpl()
{
	CI_ASSERT( mGraph );

	bool hasBackground = (bool)mBackground;
	bool needsLayer = false;

	// if pos is animating, update background and world pos
	if( ! mPos.isComplete() ) {
		if( hasBackground )
			mBackground->setPos( getPos() );

		setWorldPosDirty();
	}
	// if size is animating, update background and layout
	if( ! mSize.isComplete() ) {
		setNeedsLayout();
		if( hasBackground )
			mBackground->setSize( getSize() );
	}

	// handle transparency that needs a Layer for compositing
	if( mRenderTransparencyToFrameBuffer && isTransparent() ) {
		needsLayer = true;
	}

	// handle Filters as they need a Layer and FrameBuffer to render with
	if( ! mFilters.empty() ) {
		needsLayer = true;
	}

	if( needsLayer ) {
		if( ! mLayer ) {
			getGraph()->setNeedsLayer( this );
		}
	}
	else {
		// TODO: a bit hacky that Graph is always getting handled here, and that its Layer could potentially be removed when it always needs one
		if( mLayer && getGraph() != this )
			getGraph()->removeLayer( mLayer );
	}

	if( needsLayout() )
		layoutImpl();

	if( hasBackground ) {
		mBackground->mGraph = mGraph;
		mBackground->updateImpl();
	}

	update();
}

void View::drawImpl( Renderer *ren )
{
	ren->pushBlendMode( mBlendMode ); // TEMPORARY: this will be handled by Layer

	if( mBackground ) {
		ren->pushColor();
		mBackground->draw( ren );
		ren->popColor();
	}

	ren->pushColor();
	draw( ren );
	ren->popColor();

	ren->popBlendMode();
}

void View::clearViewsMarkedForRemoval()
{
	mSubviews.erase(
			remove_if( mSubviews.begin(), mSubviews.end(),
			           []( const ViewRef &view ) {
				           return view->mMarkedForRemoval;
			           } ),
			mSubviews.end() );
}

const View* View::hitTest( const ci::app::TouchEvent &event ) const
{
	for( auto rIt = getSubviews().rbegin(); rIt != getSubviews().rend(); ++rIt ) {
		const auto &view = *rIt;
		if( view->isHidden() || ! view->isInteractive() )
			continue;

		auto hitView = (*rIt)->hitTest( event );
		if( hitView ) {
			return hitView;
		}
	}
	
	for( const auto &touch : event.getTouches() ) {
		vec2 pos = toLocal( touch.getPos() );
		if( isPointInside( pos ) ) {
			return this;
		}
	}

	return nullptr;
}

bool View::isPointInside( const vec2 &localPos ) const
{
	return ( localPos.x >= 0 ) && ( localPos.x <= getWidth() ) && ( localPos.y >= 0 ) && ( localPos.y <= getHeight() );
}

vec2 View::toWorld( const vec2 &localPos ) const
{
	return localPos + getWorldPos();
}

Rectf View::toWorld( const Rectf &localRect ) const
{
	return localRect + getWorldPos();
}

vec2 View::toLocal( const vec2 &worldPos ) const
{
	return worldPos - getWorldPos();
}

Rectf View::toLocal( const Rectf &worldRect ) const
{
	return worldRect - getWorldPos();
}

const vec2& View::getWorldPos() const
{
	if( mWorldPosDirty ) {
		calcWorldPos();
		mWorldPosDirty = false;
	}

	return mWorldPos;
}

Rectf View::getWorldBounds() const
{
	auto pos = getWorldPos();
	return Rectf( pos.x, pos.y, pos.x + getWidth(), pos.y + getHeight() );
}

void View::calcWorldPos() const
{
	mWorldPos = getPos();

	auto parent = getParent();
	if( parent )
		mWorldPos += parent->getWorldPos();
}

void View::setBackgroundEnabled( bool enable )
{
	if( enable && ! mBackground ) {
		mBackground = make_shared<RectView>( getBounds() );
		mBackground->mParent = this;
	}
	else if( ! enable && mBackground )
		mBackground.reset();
}

bool View::isBackgroundEnabled() const
{
	return (bool)mBackground;
}

const RectViewRef& View::getBackground()
{
	setBackgroundEnabled();
	return mBackground;
}

float View::getAlphaCombined() const
{
	// TODO: Get this value from Renderer, which knows current alpha based on layer
	float alpha = mAlpha;
	if( mParent )
		alpha *= mParent->getAlphaCombined();

	return alpha;
}

void View::setLayout( const LayoutRef &layout )
{
	mLayout = layout;
	setNeedsLayout();
}

ViewRef View::getViewWithLabel( const std::string &label ) const
{
	auto it = find_if( mSubviews.begin(), mSubviews.end(), [&label] ( const ViewRef &subview ) {
		return subview->getLabel() == label;
	} );

	return it == mSubviews.end() ? nullptr : *it;
}

std::string View::getName() const
{
	return ( ! mLabel.empty() ? mLabel : System::demangleTypeName( typeid( *this ).name() ) );
}

std::ostream& operator<<( std::ostream &os, const View &rhs )
{
	os << System::demangleTypeName( typeid( rhs ).name() );
	if( ! rhs.getLabel().empty() )
		os << " (" << rhs.getLabel() << ")";

	os << " - pos: " << rhs.getPos() << ", world pos: " << rhs.getWorldPos() << ", size: " << rhs.getSize() << ", interactive: " << boolalpha << rhs.isInteractive() << ", hidden: " << rhs.isHidden() << ", clip: " << rhs.isClipEnabled() << dec;

	if( rhs.getLayer() ) {
		os << "\n[Layer";
		if( rhs.getLayer()->getFrameBuffer() ) {
			os << " FrameBuffer " << hex << rhs.getLayer()->getFrameBuffer().get() << dec << " size: " << rhs.getLayer()->getFrameBuffer()->getSize();
		}
		os << "]";
	}

	return os;
}

std::ostream& operator<<( std::ostream &os, const ViewRef &rhs )
{
	return os << *rhs;
}

namespace {

void printRecursive( ostream &os, const View &view, size_t depth )
{
	for( size_t i = 0; i < depth; i++ )
		os << "-- ";

	os << view << endl;

	for( const auto &subview : view.getSubviews() )
		printRecursive( os, *subview, depth + 1 );
}

void traverseRecursive( const ViewRef &view, const std::function<bool( const ViewRef & )>&applyFn )
{
	if( ! applyFn( view ) )
		return;

	for( const auto& subview : view->getSubviews() )
		traverseRecursive( subview, applyFn );
}

} // anonymous namespace

std::string printHierarchyToString( const View &view )
{
	stringstream str;
	printRecursive( str, view, 0 );

	return str.str();
}

std::string printHierarchyToString( const ViewRef &view )
{
	return printHierarchyToString( *view );
}

void traverse( const ViewRef &view, const std::function<bool( const ViewRef & )> &applyFn )
{
	traverseRecursive( view, applyFn );
}

// ----------------------------------------------------------------------------------------------------
// RectView
// ----------------------------------------------------------------------------------------------------

RectView::RectView( const ci::Rectf &bounds )
	: View( bounds )
{
	setInteractive( false );
	setBlendMode( BlendMode::PREMULT_ALPHA );
}

void RectView::draw( Renderer *ren )
{
	ren->setColor( getColor() );
	ren->drawSolidRect( getBoundsLocal() );
}

// ----------------------------------------------------------------------------------------------------
// StrokedRectView
// ----------------------------------------------------------------------------------------------------

StrokedRectView::StrokedRectView( const ci::Rectf &bounds )
	: RectView( bounds )
{
}

Rectf StrokedRectView::getBoundsForFrameBuffer() const
{
	switch( mPlacement ) {
		case Placement::CENTERED:
			return Rectf( vec2( - mLineWidth / 2.0f ), getSize() + mLineWidth / 2.0f );
		case Placement::INSIDE:
			return getBoundsLocal();
		case Placement::OUTSIDE:
			return Rectf( vec2( - mLineWidth ), getSize() + vec2( mLineWidth ) );
		default:
			CI_ASSERT_NOT_REACHABLE();
	}

	return Rectf::zero();
}

void StrokedRectView::draw( Renderer *ren )
{
	ren->setColor( getColor() );

	if( mLineWidth == 1 )
		ren->drawStrokedRect( getBoundsLocal() );
	else {
		Rectf rect = getBoundsLocal();
		switch( mPlacement ) {
			case Placement::CENTERED:
			break;
			case Placement::INSIDE:
				rect.inflate( vec2( 1.0f - mLineWidth / 2.0f ) );
			break;
			case Placement::OUTSIDE:
				rect.inflate( vec2( 1.0f + mLineWidth / 2.0f ) );
			break;
			default:
				CI_ASSERT_NOT_REACHABLE();
		}

		ren->drawStrokedRect( rect, mLineWidth );
	}
}

} // namespace ui

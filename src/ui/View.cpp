/*
 Copyright (c) 2014, Richard Eakin - All rights reserved.

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
#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/System.h"

using namespace std;
using namespace ci;

namespace ui {

const float BOUNDS_EPSILON = 0.00001f;

View::View( const ci::Rectf &bounds )
	: mPos( bounds.getUpperLeft() ), mSize( vec2( bounds.x2 - bounds.x1, bounds.y2 - bounds.y1 ) )
{
	mLayer = make_shared<Layer>( this );
}

View::~View()
{
	for( auto &subview : mSubviews )
		subview->mParent = nullptr;
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

	mSize = size;
	if( mBackground )
		mBackground->setSize( getSize() );

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

bool View::isBoundsAnimating() const
{
	return ( ! mPos.isComplete() || ! mSize.isComplete() );
}

void View::addSubview( const ViewRef &view )
{
	CI_ASSERT( view );

	// first set the parent to be us, which will remove it from any existing parent (including this view).
	view->setParent( this );
	mSubviews.push_back( view );
}

void View::addSubviews( const vector<ViewRef> &views )
{
	for( const auto &view : views ) {
		addSubview( view );
	}
}

void View::insertSubview( const ViewRef &view, size_t index )
{
	CI_ASSERT( view && index <= mSubviews.size() );

	// first set the parent to be us, which will remove it from any existing parent (including this view).
	view->setParent( this );

	auto it = mSubviews.begin();
	std::advance( it, index );
	mSubviews.insert( it, view );
}

void View::removeSubview( const ViewRef &view )
{
	for( auto it = mSubviews.begin(); it != mSubviews.end(); ++it ) {
		if( view == *it ) {
			view->mParent = nullptr;
			mSubviews.erase( it );
			return;
		}
	}
}

void View::removeAllSubviews()
{
	for( auto &view : mSubviews ) {
		view->mParent = nullptr;
	}

	mSubviews.clear();
}

void View::removeFromParent()
{
	if( ! mParent )
		return;

	mParent->removeSubview( shared_from_this() );
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

RendererRef View::getRenderer() const
{
	getGraph()->getRenderer();
}

RendererRef View::getRenderer()
{
	getGraph()->getRenderer();
}

void View::setNeedsLayout()
{
	mNeedsLayout = true;
	for( const auto &subview : mSubviews )
		subview->setNeedsLayout();
}

void View::setWorldPosDirty()
{
	mWorldPosDirty = true;
	for( const auto &subview : mSubviews )
		subview->setWorldPosDirty();
}

void View::propagateLayout()
{
	mWorldPosDirty = true;

	if( mBackground )
		mBackground->propagateLayout();

	if( mFillParent ) {
		auto parent = getParent();
		if( parent ) {
			setPos( vec2( 0 ) );
			setSize( parent->getSize() );
		}
	}

	layout();
	mNeedsLayout = false;

	for( auto &view : mSubviews ) {
		if( view->mNeedsLayout )
			view->propagateLayout();
	}
}

void View::propagateUpdate()
{
	for( auto& view : mSubviews )
		view->propagateUpdate();

	// if bounds is animating, update background's position and size, propagate layout
	bool needsLayout = mNeedsLayout;
	bool hasBackground = (bool)mBackground;

	if( ! mPos.isComplete() ) {
		if( hasBackground )
			mBackground->setPos( getPos() );

		setWorldPosDirty();
	}
	if( ! mSize.isComplete() ) {
		needsLayout = true;
		if( hasBackground )
			mBackground->setSize( getSize() );
	}

	if( needsLayout )
		propagateLayout();

	if( hasBackground )
		mBackground->propagateUpdate();

	mLayer->update();
	update();
}

void View::propagateDraw()
{
#if 1
	mLayer->draw();
#else
	if( mHidden )
		return;

	beginClip();

	gl::ScopedModelMatrix modelScope1;
	gl::translate( mPos() );

	if( mBackground )
		mBackground->draw();

	draw();

	for( auto &view : mSubviews )
		view->propagateDraw();

	endClip();
#endif
}

void View::drawImpl()
{
	auto renderer = getRenderer();

	renderer->pushColor();
	if( mBackground )
		mBackground->draw();

	draw();
	renderer->popColor();
}

bool View::hitTest( const vec2 &localPos ) const
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

const RectViewRef& View::getBackground()
{
	setBackgroundEnabled();
	return mBackground;
}

//float View::getAlphaCombined() const
//{
//	float alpha = mAlpha;
//	if( mParent )
//		alpha *= mParent->getAlphaCombined();
//
//	return alpha;
//}

std::string View::getName() const
{
	return ( ! mLabel.empty() ? mLabel : System::demangleTypeName( typeid( *this ).name() ) );
}

std::ostream& operator<<( std::ostream &os, const ViewRef &rhs )
{
	auto rhsPtr = rhs.get();
	os << System::demangleTypeName( typeid( *rhsPtr ).name() );
	if( ! rhs->getLabel().empty() )
		os << " (" << rhs->getLabel() << ")";

	os << " - pos: " << rhs->getPos() << ", world pos: " << rhs->getWorldPos() << ", size: " << rhs->getSize() << ", interactive: " << boolalpha << rhs->isInteractive() << ", hidden: " << rhs->isHidden() << dec;

	return os;
}

namespace {

void printRecursive( ostream &os, const ViewRef &view, size_t depth )
{
	for( size_t i = 0; i < depth; i++ )
		os << "-- ";

	os << view << endl;

	for( const auto &subview : view->getSubviews() )
		printRecursive( os, subview, depth + 1 );
}

} // anonymous namespace

void View::printHeirarchy( ostream &os )
{
	printRecursive( os, shared_from_this(), 0 );
}

void View::propagateTouchesBegan( ci::app::TouchEvent &event )
{
	if( mHidden || ! mInteractive )
		return;

	const auto &touch = event.getTouches().front();

	if( ! hitTest( toLocal( touch.getPos() ) ) )
		return;

	for( auto rIt = mSubviews.rbegin(); rIt != mSubviews.rend(); ++rIt ) {
		(*rIt)->propagateTouchesBegan( event );
		if( event.isHandled() )
			return;
	}

	bool handled = touchesBegan( event );
	if( handled ) {
		// Only allow this View to handle this touch in other UI events.
		mActiveTouches[touch.getId()] = touch;
		event.setHandled();
	}
}

void View::propagateTouchesMoved( ci::app::TouchEvent &event )
{
	if( mHidden || ! mInteractive )
		return;

	const auto &touch = event.getTouches().front();
	for( auto rIt = mSubviews.rbegin(); rIt != mSubviews.rend(); ++rIt ) {
		(*rIt)->propagateTouchesMoved( event );
		if(	event.isHandled() )
			return;

	}

	auto touchIt = mActiveTouches.find( touch.getId() );
	if( touchIt == mActiveTouches.end() )
		return;

	mActiveTouches[touch.getId()] = touch;
	bool handled = touchesMoved( event );
	event.setHandled( handled );
}

void View::propagateTouchesEnded( ci::app::TouchEvent &event )
{
	if( mHidden || ! mInteractive )
		return;

	const auto &touch = event.getTouches().front();

	for( auto rIt = mSubviews.rbegin(); rIt != mSubviews.rend(); ++rIt ) {
		(*rIt)->propagateTouchesEnded( event );
		if( event.isHandled() )
			return;
	}

	auto touchIt = mActiveTouches.find( touch.getId() );
	if( touchIt == mActiveTouches.end() )
		return;

	mActiveTouches.erase( touchIt );
	bool handled = touchesEnded( event );
	event.setHandled( handled );
}

void View::beginClip()
{
	if( mClipEnabled ) {
		Rectf worldBounds = getWorldBounds();
		ivec2 pos = worldBounds.getLowerLeft();
		pos.y = app::getWindowHeight() - pos.y; // flip y relative to window's bottom left

		auto ctx = gl::context();
		ctx->pushBoolState( GL_SCISSOR_TEST, GL_TRUE );
		ctx->pushScissor( std::pair<ivec2, ivec2>( pos, getSize() ) );
	}
}

void View::endClip()
{
	if( mClipEnabled ) {
		auto ctx = gl::context();
		ctx->popBoolState( GL_SCISSOR_TEST );
		ctx->popScissor();
	}
}

// ----------------------------------------------------------------------------------------------------
// MARK: - RectView
// ----------------------------------------------------------------------------------------------------

void RectView::draw()
{
	auto renderer = getRenderer();
	renderer->setColor( getColor() );
	renderer->drawSolidRect( getBoundsLocal() );
}


void StrokedRectView::draw()
{
	auto renderer = getRenderer();
	renderer->setColor( getColor() );

	if( mLineWidth == 1 )
		renderer->drawStrokedRect( getBoundsLocal() );
	else
		renderer->drawStrokedRect( getBoundsLocal(), mLineWidth );
}

} // namespace ui
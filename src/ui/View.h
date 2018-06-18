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

#pragma once

#include "ui/Export.h"
#include "ui/Layer.h"
#include "ui/Renderer.h"
#include "ui/Layout.h"

#include "cinder/app/TouchEvent.h"
#include "cinder/app/KeyEvent.h"
#include "cinder/app/MouseEvent.h"
#include "cinder/Tween.h"
#include "cinder/Rect.h"
#include "cinder/Color.h"
#include "cinder/Signals.h"

#include <list>
#include <memory>

namespace ui {

typedef std::shared_ptr<class Responder>		ResponderRef;
typedef std::shared_ptr<class View>				ViewRef;
typedef std::shared_ptr<class RectView>			RectViewRef;
typedef std::shared_ptr<class StrokedRectView>	StrokedRectViewRef;
class Graph;

class CI_UI_API View : public std::enable_shared_from_this<View> {
  public:
	View( const ci::Rectf &bounds = ci::Rectf::zero() );
	virtual ~View();

	void		addSubviews( const std::vector<ViewRef> &views );
	virtual void addSubview( const ViewRef &view );
	virtual void insertSubview( const ViewRef &view, size_t index );
	virtual void removeSubview( const ViewRef &view );
	virtual void removeAllSubviews();
	virtual void removeFromParent();
	virtual bool containsSubview( const ViewRef &view );

	template<typename ViewT, typename... Args>
	std::shared_ptr<ViewT> makeSubview( Args&&... args );

	void			setBounds( const ci::Rectf &bounds );
	virtual void	setPos( const ci::vec2 &position );
	virtual void	setSize( const ci::vec2 &size );
	virtual void	setAlpha( float alpha )							{ mAlpha = alpha; }

	float					getAlpha()	const		{ return mAlpha; }
	float					getAlphaCombined() const;
	//! Returns the bounds of this View, relative to its parent (or self if there is no parent).
	ci::Rectf				getBounds() const;
	//! Returns the bounds of this View, relative to itself (origin = [0,0]).
	ci::Rectf				getBoundsLocal() const;
	const ci::vec2&			getPos() const			{ return mPos; }
	const ci::vec2&			getSize() const			{ return mSize; }
	ci::vec2				getCenter() const		{ return mPos() + mSize() / 2.0f; }
	ci::vec2				getCenterLocal() const	{ return mSize() / 2.0f; }
	float					getPosX() const			{ return mPos().x; }
	float					getPosY() const			{ return mPos().y; }
	float					getWidth() const		{ return mSize().x; }
	float					getHeight() const		{ return mSize().y; }

	ci::Anim<float>*		animAlpha()			{ return &mAlpha; }
	ci::Anim<ci::vec2>*		animPos()			{ return &mPos; }
	ci::Anim<ci::vec2>*		animSize()			{ return &mSize; }

	const std::vector<ViewRef>&	getSubviews() const		{ return mSubviews; }
	std::vector<ViewRef>&	getSubviews()		{ return mSubviews; }
	const ViewRef&			getSubview( size_t index ) const;
	ViewRef&				getSubview( size_t index );
	const View*				getParent() const	{ return mParent; }
	View*					getParent()			{ return mParent; }

	//!
	const Graph*            getGraph() const    { return mGraph; }
	//!
	Graph*                  getGraph()          { return mGraph; }
	//!
	LayerRef	            getLayer() const	{ return mLayer; }
	//!
	bool                    isLayerRoot() const;

	void				setLayout( const LayoutRef &layout );
	LayoutRef			getLayout() const	{ return mLayout; }

	// Responder --------
	//!
	bool	isFirstResponder() const;
	//!
	bool	becomeFirstResponder();
	//!
	bool	resignFirstResponder();
	//!
	void	setAcceptsFirstResponder( bool b = true )	{ mAcceptsFirstResponder = b; }
	//!
	bool	getAcceptsFirstResponder() const			{ return mAcceptsFirstResponder; }
	//! Sets the next responder in the responder chain. Can be overridden if a View needs to insert its own subviews into the chain.
	virtual void	setNextResponder( const ViewRef &view )		{ mNextResponder = view; }
	//! Returns the next responder in the responder chain, either one set by user with setNextResponder() or the parent View.
	ViewRef	getNextResponder() const;

	//! Sets a label that can be used to identify this View
	void				setLabel( const std::string &label )	{ mLabel = label; }
	const std::string&	getLabel() const						{ return mLabel; }
	//! Returns the first label whose label matches the specified string, or an empty ViewRef.
	ViewRef				getViewWithLabel( const std::string &label ) const;

	//! Returns this View's label if it has one, otherwise it's typename
	std::string			getName() const;

	void setBackgroundEnabled( bool enable = true );
	bool isBackgroundEnabled() const;
	const RectViewRef& getBackground();

	const ci::vec2&		getWorldPos() const;
	ci::Rectf			getWorldBounds() const;
	ci::vec2			toWorld( const ci::vec2 &localPos ) const;
	ci::vec2			toLocal( const ci::vec2 &worldPos ) const;
	ci::Rectf			toWorld( const ci::Rectf &localRect ) const;
	ci::Rectf			toLocal( const ci::Rectf &worldRect ) const;

	virtual bool				hitTest( const ci::vec2 &localPos ) const;

	void	setHidden( bool hidden = true )			{ mHidden = hidden; }
	bool	isHidden() const						{ return mHidden; }
	void	setInteractive( bool enable = true )	{ mInteractive = enable; }
	bool	isInteractive() const					{ return mInteractive; }
	bool	isUserInteracting() const				{ return ! mActiveTouches.empty(); }
	bool	isBoundsAnimating() const;
	bool    isTransparent() const;

	const  std::map<uint32_t, ci::app::TouchEvent::Touch>&	getActiveTouches() const	{ return mActiveTouches; }

	// TODO: this needs to mark layer tree dirty, at least if there is compositing going on (should skip reconfigure otherwise)
	void setRenderTransparencyToFrameBufferEnabled( bool enable )	{ mRenderTransparencyToFrameBuffer = enable; }
	bool isRenderTransparencyToFrameBufferEnabled() const			{ return mRenderTransparencyToFrameBuffer; }

	void	setClipEnabled( bool enable = true );
	bool	isClipEnabled() const;

	void	    setBlendMode( BlendMode mode )			{ mBlendMode = mode; }
	BlendMode	getBlendMode() const					{ return mBlendMode; }

	void    addFilter( const FilterRef &filter );
	void	removeFilter( const FilterRef &filter );
	void	removeAllFilters();

	void	setFillParentEnabled( bool enable = true );
	bool	isFillParentEnabled() const					{ return mFillParent; }

	//! Informs layout propagation that this View and its subviews need layout() to be called.
	void	setNeedsLayout();
	//! Returns whether this View needs to have its layout() method called before the next update(). TODO: rename to getNeedsLayout()
	bool	needsLayout() const	{ return mNeedsLayout; }
	//! Lays out this view and its subviews immediately, if layout updates are pending.
	void	layoutIfNeeded();

	//! Signal emitted after this View has had it's layout() method called.
	ci::signals::Signal<void ()>&	getSignalViewDidLayout()	{ return mSignalViewDidLayout; }


	//! This is done when the world position should be recalculated but calling layout isn't necessary (ex. when ScrollView offset moves)
	void	setWorldPosDirty();

  protected:
	virtual void layout()		        {}
	virtual void update()		        {}
	virtual void draw( Renderer *ren )  {}

	//! Returns the bounds required for rendering this View to a FrameBuffer. \default is this View's local bounds. Override if this View needs a larger sized or FrameBuffer.
	virtual ci::Rectf   getBoundsForFrameBuffer() const;

	// Responder ------------------
	// TODO: rename these with 'can' or 'should' suffix? To indicate they are asking whether this is possible or not
	//! Return false if you cannot become first responder.
	virtual bool	willBecomeFirstResponder()	{ return true; }
	//! Return true if you won't resign first responder.
	virtual bool	willResignFirstResponder()	{ return true; }

	// Override to handle UI events. Return true if any touch was handled, false otherwise.
	virtual bool touchesBegan( ci::app::TouchEvent &event )	{ return false; }
	virtual bool touchesMoved( ci::app::TouchEvent &event )	{ return false; }
	virtual bool touchesEnded( ci::app::TouchEvent &event )	{ return false; }

	virtual bool keyDown( ci::app::KeyEvent &event )	{ return false; }
	virtual bool keyUp( ci::app::KeyEvent &event )		{ return false; }

  private:
	View( const View& )				= delete;
	View& operator=( const View& )	= delete;

	void setParent( View *parent );
	void calcWorldPos() const;
	void layoutImpl();
	void updateImpl();
	void drawImpl( Renderer *ren );
	void clearViewsMarkedForRemoval();


	typedef std::map<uint32_t, ci::app::TouchEvent::Touch> TouchMapT; // TODO just store this as vector and use std::find

	TouchMapT				mActiveTouches;

	bool					mInteractive = true;
	bool					mHidden = false;
	bool					mNeedsLayout = false;
	
	mutable bool			mWorldPosDirty = true;
	mutable ci::vec2		mWorldPos;

	ci::Anim<float>			mAlpha = 1.0f;
	ci::Anim<ci::vec2>		mPos;
	ci::Anim<ci::vec2>		mSize;
	std::string				mLabel;
	bool					mFillParent = false; // TODO: replace this with proper layout system
	BlendMode				mBlendMode = BlendMode::ALPHA;
	bool                    mClipEnabled = false;
	bool			        mRendersToFrameBuffer = false;
	bool			        mRenderTransparencyToFrameBuffer = true;
	bool                    mIsIteratingSubviews = false;
	bool                    mMarkedForRemoval = false;

	View*					mParent = nullptr;
	Graph*                  mGraph = nullptr;
	std::vector<ViewRef>	mSubviews;
	RectViewRef				mBackground;
	LayerRef				mLayer;
	std::vector<FilterRef>  mFilters;
	LayoutRef				mLayout;

	bool					mAcceptsFirstResponder = false;
	ViewRef					mNextResponder;

	ci::signals::Signal<void ()>	mSignalViewDidLayout;

	friend class Layer;
	friend class Graph;
};

CI_UI_API std::ostream& operator<<( std::ostream &os, const View &rhs );
CI_UI_API std::ostream& operator<<( std::ostream &os, const ViewRef &rhs );

//! Returns a string representation of the View hierarchy starting at \a view (for debugging purposes).
CI_UI_API std::string printHierarchyToString( const View &view );
//! Returns a string representation of the View hierarchy starting at \a view (for debugging purposes).
CI_UI_API std::string printHierarchyToString( const ViewRef &view );
//! Traverses the View hierarchy of \a view, top to bottom.
CI_UI_API void traverse( const ViewRef &view, const std::function<bool( const ViewRef & )> &applyFn );

template<typename ViewT, typename... Args>
std::shared_ptr<ViewT> View::makeSubview( Args&&... args )
{
	static_assert( std::is_base_of<View, ViewT>::value, "ViewT must inherit from ui::View" );

	std::shared_ptr<ViewT> result( new ViewT( std::forward<Args>( args )... ) );
	addSubview( result );
	return result;
}

class CI_UI_API RectView : public View {
  public:
	RectView( const ci::Rectf &bounds = ci::Rectf::zero() );

	void					setColor( const ci::ColorA &color )	{ mColor = color; }
	const ci::ColorA&		getColor() const					{ return mColor; }
	ci::Anim<ci::ColorA>*	animColor()							{ return &mColor; }
	//! note: deprecated, use animColor() instead
	ci::Anim<ci::ColorA>*	getColorAnim()						{ return &mColor; }

  protected:
	void draw( Renderer *ren ) override;

	ci::Anim<ci::ColorA>	mColor = { ci::ColorA::black() };
  private:
	friend class View;
};

class CI_UI_API StrokedRectView : public RectView {
  public:
	enum class Placement { CENTERED, INSIDE, OUTSIDE };

	StrokedRectView( const ci::Rectf &bounds = ci::Rectf::zero() );

	void				setLineWidth( float lineWidth )		{ mLineWidth = lineWidth; }
	float				getLineWidth() const				{ return mLineWidth; }
	ci::Anim<float>*	getLineWidthAnim()					{ return &mLineWidth; }

	void                setPlacement( Placement placement )	{ mPlacement = placement; };
	Placement           getPlacement() const                { return mPlacement; }

  protected:
	void draw( Renderer *ren ) override;
	ci::Rectf getBoundsForFrameBuffer() const   override;

  private:

	ci::Anim<float>		mLineWidth = { 1 };
	Placement mPlacement = Placement::CENTERED;
};

} // namespace ui


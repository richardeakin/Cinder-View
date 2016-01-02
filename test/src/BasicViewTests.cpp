#include "BasicViewTests.h"

#include "cinder/app/App.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"
#include "cinder/Log.h"

using namespace std;
using namespace ci;
using namespace mason;

#define PADDING 50.0f
#define ANIM_TIME 1.0f

class ChildView : public ui::RectView {
public:
	ChildView( const Rectf &bounds ) : RectView( bounds )	{}

protected:
	bool touchesBegan( app::TouchEvent &event )	override
	{
		vec2 pos = event.getTouches().front().getPos();
		vec2 localPos = toLocal( pos );

		CI_LOG_V( getLabel() << " local pos: " << localPos << ", world pos: " << getWorldPos() << ", event pos: " << pos );
		moveViewAnimated();

		return true;
	}

	bool touchesMoved( app::TouchEvent &event )		override
	{
		vec2 pos = event.getTouches().front().getPos();
		vec2 localPos = toLocal( pos );
		CI_LOG_V( getLabel() << " local pos: " << localPos << ", world pos: " << getWorldPos() << ", event pos: " << pos );

		return true;
	}

	bool touchesEnded( app::TouchEvent &event )		override
	{
		vec2 pos = event.getTouches().front().getPos();
		vec2 localPos = toLocal( pos );
		CI_LOG_V( getLabel() << " local pos: " << localPos << ", world pos: " << getWorldPos() << ", event pos: " << pos );

		return true;
	}

	void moveViewAnimated()
	{
		float w = getParent()->getBounds().getWidth() * 0.5f;
		app::timeline().apply( animPos(), vec2( randFloat( w ), randFloat( w ) ), ANIM_TIME, EaseInOutBack() );
		app::timeline().apply( animSize(), vec2( randFloat( 200 ), randFloat( 100 ) ), ANIM_TIME, EaseInOutBack() );
	}
};

BasicViewTests::BasicViewTests()
	: SuiteView()
{
	mContainerView = make_shared<View>();
	mContainerView->getBackground()->setColor( Color::gray( 0.75f ) );
	mContainerView->setLabel( "container" );

	Rectf childBounds = Rectf( 30, 30, 300, 100 );

	auto view1 = make_shared<ChildView>( childBounds );
	view1->setColor( Color( "green" ) );
	view1->setLabel( "view1" );

	childBounds += vec2( 0, childBounds.getHeight() + 10 );
	auto view2 = make_shared<ChildView>( childBounds );
	view2->setColor( Color( "blue" ) );
	view2->setLabel( "view2" );

	childBounds += vec2( 0, childBounds.getHeight() + 10 );
	auto view3 = make_shared<ChildView>( childBounds );
	view3->setColor( Color( "yellow" ) );
	view3->setLabel( "view3" );

	// view4 is placed as a smaller subview of view3
	auto view4 = make_shared<ChildView>( Rectf( 20, 20, 100, 50 ) );
	view4->setColor( Color( "brown" ) );
	view4->setLabel( "view4" );
	view3->addSubview( view4 );

	mContainerView->addSubview( view1 );
	mContainerView->addSubview( view2 );
	mContainerView->addSubview( view3 );
	mContainerView->addSubview( view1 ); // with this call, view1 will only be in root's view heirarchy once, but it will be moved to the top.

	mBorderView = make_shared<ui::StrokedRectView>();
	mBorderView->setColor( ColorA( "red", 1 ) );
	mContainerView->addSubview( mBorderView );

	mLabel = make_shared<ui::Label>();
	mLabel->setText( "a label" );
	mLabel->setAlignment( ui::TextAlignment::CENTER );
	mLabel->setTextColor( Color::white() );
	mLabel->getBackground()->setColor( Color( 0, 0, 0.4 ) );

	mLabelClipped = make_shared<ui::Label>();
	mLabelClipped->setText( "blah blah blah blah blah" );
	mLabelClipped->setClipEnabled();
	mLabelClipped->setTextColor( Color::white() );
	mLabelClipped->getBackground()->setColor( Color( 0, 0, 0.4 ) );

	mLabelGrid = make_shared<ui::LabelGrid>();
	mLabelGrid->getBackground()->setColor( ColorA( "yellow", 0.5f ) );
	mLabelGrid->setCell( 0, 0, "hey:" );
	mLabelGrid->setCell( 0, 1, "how ya goin:" );
	mLabelGrid->setCell( 1, 0, "yo" );
	mLabelGrid->setCell( 1, 1, "yea, good." );
	mLabelGrid->setCell( 0, 2, "writing ui code:" );
	mLabelGrid->setCell( 1, 2, "sucks" );

	mImageView = make_shared<ui::ImageView>();

	auto imageBorder = make_shared<ui::StrokedRectView>();
	imageBorder->setFillParentEnabled();
	imageBorder->setColor( ColorA( 0.9f, 0.5f, 0.0f, 0.7f ) );
	mImageView->addSubview( imageBorder );
	fs::path imageFilePath = app::getAssetPath( "images/monkey_hitchhike.jpg" );
	try {
		CI_LOG_I( "loading image view.." );
		auto image = make_shared<ui::Image>( loadImage( loadFile( imageFilePath ) ) );
		mImageView->setImage( image );
		CI_LOG_I( "complete" );
	}
	catch( std::exception &exc ) {
		CI_LOG_EXCEPTION( "failed to load image at path: " << imageFilePath, exc );
	}

	mContainerView->addSubviews( { mLabel, mLabelClipped, mLabelGrid, mImageView } );
	addSubview( mContainerView );

	connectKeyDown( signals::slot( this, &BasicViewTests::keyEvent ) );
}

void BasicViewTests::layout()
{
//	CI_LOG_I( "bounds: " << getBounds() );

	mContainerView->setBounds( Rectf( PADDING, PADDING, getWidth() - PADDING, getHeight() - PADDING ) );
	mBorderView->setSize( mContainerView->getSize() );

	Rectf labelBounds( PADDING, mContainerView->getHeight() - 70, 100 + PADDING, mContainerView->getHeight() - 44 );
	mLabel->setBounds( labelBounds );

	labelBounds += vec2( 0, labelBounds.getHeight() + 6 );
	mLabelClipped->setBounds( labelBounds );

	mLabelGrid->setBounds( Rectf( getCenterLocal().x - 40, PADDING, getCenterLocal().x + 200, PADDING + 60 ) );

	vec2 imageViewPos = vec2( mLabelGrid->getPos().x, mLabelGrid->getBounds().y2 + PADDING );
	mImageView->setPos( imageViewPos );
	mImageView->setSize( vec2( 200, 200 ) );
}

void BasicViewTests::keyEvent( app::KeyEvent &event )
{
	switch( event.getCode() ) {
		case app::KeyEvent::KEY_c: {
			app::timeline().apply( mContainerView->getBackground()->getColorAnim(), ColorA( randFloat(), randFloat(), randFloat(), 1.0f ), ANIM_TIME, EaseInOutExpo() );
			break;
		}
		case app::KeyEvent::KEY_SPACE: {
			app::timeline().apply( mContainerView->animPos(), vec2( randFloat( -PADDING, PADDING ), randFloat( -PADDING, PADDING ) ), ANIM_TIME, EaseOutExpo() );
			break;
		}
		case app::KeyEvent::KEY_a: {
			float nextAlpha = mContainerView->getAlpha() > 0.4f ? 0.4f : 1.0f;
			app::timeline().apply( mContainerView->animAlpha(), nextAlpha, 2.0f, EaseInOutExpo() );
			break;
		}
		case app::KeyEvent::KEY_r: {
			if( ! mContainerView->getSubviews().empty() ) {
				auto lastChild = mContainerView->getSubviews().back();
				lastChild->removeFromParent();
			}
			break;
		}
		case app::KeyEvent::KEY_o: {
			float nextBorderWidth = randFloat( 1, 30 );
			app::timeline().apply( mBorderView->getLineWidthAnim(), nextBorderWidth, 0.6f, EaseOutExpo() );
			break;
		}
		case app::KeyEvent::KEY_w: {
			CI_LOG_V( "world positions, root: " << mContainerView->getWorldPos() );
			app::console() << "subviews: " << endl;
			for( const auto &view : mContainerView->getSubviews() )
				app::console() << "(" << view->getLabel() << ") " << view->getWorldPos() << endl;

			break;
		}
		case app::KeyEvent::KEY_i: {
			auto nextMode = ui::ImageScaleMode( ( (int)mImageView->getScaleMode() + 1 ) % ui::ImageScaleMode::NUM_MODES );
			CI_LOG_I( "setting scale mode to: " << nextMode );
			mImageView->setScaleMode( nextMode );
			break;
		}
	}
}

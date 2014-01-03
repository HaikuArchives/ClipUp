#include "MainView.h"

#include <Window.h>

#if DEBUG
#include <iostream>
#endif

MainView::MainView()
:	BView( BRect( 0, 0, 10, 10), "_mainview", B_FOLLOW_ALL, B_WILL_DRAW|B_FULL_UPDATE_ON_RESIZE|B_FRAME_EVENTS ),
	fMouseDown( false )
{
}

void MainView::AttachedToWindow()
{
	BView::AttachedToWindow();
	ResizeTo( Window()->Frame().Width(), Window()->Frame().Height() );
}

void MainView::Draw( BRect updaterect )
{

	BRect	rect( Bounds() );
	rect.bottom = 4;

	SetHighColor( 0xFF, 0xCB, 0x00 );
	
	FillRect( rect );
	
	SetHighColor( 0xAF, 0x7B, 0x00 );
	
	StrokeLine( rect.RightBottom(), rect.RightTop() );
	
	SetHighColor( 0xFF, 0xFF, 0x50 );
	
	StrokeLine( rect.LeftTop() );
	StrokeLine( rect.LeftBottom() );
	
	rect.top = 5;
	rect.bottom = Bounds().bottom;
	
	SetHighColor( 241, 241, 241 );
	StrokeLine( rect.LeftTop(), rect.LeftBottom() );
	
	SetHighColor( 154, 154, 154 );
	StrokeLine( rect.RightBottom() );
	StrokeLine( rect.RightTop() );

}

void MainView::FrameResized(float new_width, float new_height)
{
	Draw( Bounds() );
}

void MainView::MouseDown(BPoint where)
{
	if (where.y>4) return;
	
	fMouseDown = true;
	fMouseOffset = where;
	fWindowMoveOffset = Window()->Frame().LeftTop();
	SetMouseEventMask( B_LOCK_WINDOW_FOCUS|B_SUSPEND_VIEW_FOCUS|B_NO_POINTER_HISTORY );
}

void MainView::MouseUp(BPoint where) {
	fMouseDown = false;
	BView::MouseUp( where );
}

void MainView::MouseMoved( BPoint where, uint32 code, const BMessage *a_message) {

	BPoint location;
	uint32 buttons;
	
	GetMouse( &location, &buttons );
	if (!buttons) MouseUp( where );
	
	if (!fMouseDown) return;
	
	BPoint newPosition = where - fMouseOffset + Window()->Frame().LeftTop();
	Window()->MoveTo(newPosition);
	
}

bool MainView::MouseIsDown() const
{
	return fMouseDown;
}


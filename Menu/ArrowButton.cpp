#include "ArrowButton.h"

#include <Bitmap.h>
#include <MessageRunner.h>
#include <Window.h>

#if DEBUG
#include <iostream>
#endif

#include "get_resource_bitmap.h"
#include "MainView.h"

ArrowButton::ArrowButton( BPoint offset, const char *name, const char *label, BMessage *msg )
	:	BPictureButton(BRect( 0, 0, 10, 14 ).OffsetToCopy( offset ),
			name,
			new BPicture(), new BPicture(),
			msg, B_TWO_STATE_BUTTON,
			B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP,
			B_WILL_DRAW ),
		fMessageRunner( NULL ),
		fMouseDown( false ),
		fForcedUp( true )
{
	SetLabel( label );
}

ArrowButton::~ArrowButton()
{
	delete fMessageRunner;
}

void ArrowButton::AttachedToWindow()
{
	
	BPictureButton::AttachedToWindow();

	ResizeTo( Parent()->Frame().Width(), Frame().Height() );
	
	SetViewColor( 240, 240, 240 );
	SetLowColor( 240, 240, 240 );
	CreatePictures();
	
}

void ArrowButton::Draw( BRect updateRect )
{
	BPictureButton::Draw( updateRect );
}

void ArrowButton::ChangeState( int32 state )
{
	if (Value()==state) return;
	
	SetValue(state);
	CreatePictures();

}

void ArrowButton::MessageReceived(BMessage *msg)
{
	if (msg->what == 'MsgR') {
		fMessageRunner = NULL;
		
		BPoint location;
		uint32 buttons;
		GetMouse( &location, &buttons );

		if (buttons && Bounds().Contains( location )) {
		
			MouseUp( location );
			fForcedUp = true;

		}
		
	}
	else {

		BPictureButton::MessageReceived(msg);

	}
}

status_t ArrowButton::Invoke(BMessage *msg)
{
	
	CreatePictures();

	return BPictureButton::Invoke( msg );
}

void ArrowButton::GetPreferredSize(float *width, float *height)
{

	BFont font;
	GetFont( &font );
	font.SetFace( B_BOLD_FACE );
	SetFont( &font );

	*width	= StringWidth( Label() ) + 24;
	*height	= 14;

}

void ArrowButton::MouseDown(BPoint where)
{
	if (!IsEnabled() || fForcedUp || fMouseDown || ((MainView *)Parent()->Parent())->MouseIsDown() ) return;
	
	delete fMessageRunner;
	fMessageRunner = new BMessageRunner(BMessenger(this),
		new BMessage('MsgR'), 1000000, 1);
	
	SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS);
	fMouseDown = true;
	
	BPictureButton::MouseDown( where );
}

void ArrowButton::MouseUp(BPoint where)
{

	fForcedUp = false;
	
	if (!IsEnabled() || !fMouseDown) return;
	
	delete fMessageRunner;
	fMessageRunner = NULL;

	fMouseDown = false;
	
	BPictureButton::MouseUp( where );
}

void ArrowButton::MouseMoved( BPoint where, uint32 code, const BMessage *a_message)
{

	if (!IsEnabled()) return;
	
	BPoint location;
	uint32 buttons;
	GetMouse( &location, &buttons );

	if (buttons) {
		if (Bounds().Contains( location )) {
			if (!Value()) MouseDown( where );
		}
		else {
			MouseUp( where );
		}
	}
	else {
		MouseUp( where );
	}
	
	BPictureButton::MouseMoved( where, code, a_message );
}

void ArrowButton::CreatePictures()
{
	
	SetDrawingMode( B_OP_COPY );
	
	BFont	font;
	font = *be_plain_font;
	font.SetSize( 11.0 );

	SetHighColor( 0, 0, 0 );

	BBitmap *background = get_resource_bitmap("arrowbutton_hg");
	
	BPicture *picture;
	
	SetFont( &font );

	BeginPicture( new BPicture() );
	DrawBitmap( background, BPoint(0,0) );
	DrawBitmap(get_resource_bitmap( Value() ? "pointer_b" : "pointer_r" ), BPoint(5.0, 3.0));
	DrawString( Label(), BPoint( 21, 12 ) );
	picture = EndPicture();

	if (Value()) {
		SetEnabledOn( picture );
		SetDisabledOn( picture );
	}
	else {
		SetEnabledOff( picture );
		SetDisabledOff( picture );
	}

	delete picture;
	
	font.SetFace( B_BOLD_FACE );
	SetFont( &font );

	BeginPicture( new BPicture() );
	DrawBitmap( background, BPoint( 0, 0 ) );
	DrawBitmap(get_resource_bitmap( "pointer_rb" ), BPoint(5.0, 3.0));
	DrawString( Label(), BPoint( 20, 12 ) );
	picture = EndPicture();

	if (Value()) {
		SetEnabledOff( picture );
		SetDisabledOff( picture );
	}
	else {
		SetEnabledOn( picture );
		SetDisabledOn( picture );
	}

	delete picture;

	Invalidate();
}

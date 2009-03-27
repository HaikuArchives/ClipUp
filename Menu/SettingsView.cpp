/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "SettingsView.h"

#include <Bitmap.h>
#include <Font.h>
#include <PictureButton.h>

#if DEBUG
#include <iostream>
#endif

#include "SlideView.h"

#include "constants.h"
#include "get_resource_bitmap.h"
#include "draw_button.h"

SettingsView::SettingsView( BRect rect )
	:	BView( rect, NULL, B_FOLLOW_TOP|B_FOLLOW_LEFT_RIGHT, B_FRAME_EVENTS|B_WILL_DRAW ),
		fLimitBmp( get_resource_bitmap("maximum") ),
		fTrashBmp( get_resource_bitmap("empty") )
{
}

SettingsView::~SettingsView()
{
	delete fLimitBmp;
	delete fTrashBmp;
}

void SettingsView::AttachedToWindow()
{
	BView::AttachedToWindow();
	if (Parent()) SetViewColor( Parent()->ViewColor() );
	
	BFont	font;
	font = *be_plain_font;
	font.SetSize( 10.0 );
	SetFont( &font );

	SetDrawingMode( B_OP_ALPHA );
	
	AddButtons();
}

void SettingsView::DetachedFromWindow()
{
	RemoveButtons();
	BView::DetachedFromWindow();
}

void SettingsView::GetPreferredSize(float *width, float *height)
{
	float w1 = StringWidth( kTxtEmptyHistory );
	float w2 = StringWidth( kTxtLimitHistoryListTo ) + 70;
	
	*width	= max_c( w1, w2 ) + 46;
	*height	= 58;
}

void SettingsView::FrameResized(float new_width, float new_height)
{
	RemoveButtons();
	AddButtons();
}

void SettingsView::AddButtons()
{

	// Button variables;

	const char *txt;
	BRect rect;
	BPoint text_position;
	BPicture *pic0, *pic1;
	
	// Slider and label

	AddChild( new SlideView(	BRect(0, 0, Frame().Width() - 16, 20).OffsetToCopy( 26, 2 ),
								kTxtLimitHistoryListTo,
								new BMessage('Limt') ) );
	
	// Trash button

	txt = kTxtEmptyHistory;
	
	rect = BRect(0, 0, Frame().Width() - 32, 16 );
	text_position = BPoint ( (rect.Width() - StringWidth(txt))/2, 12);
	
	BeginPicture( new BPicture() );
	draw_button( this, rect );
	SetHighColor( 0, 0, 0 );
	DrawString( txt, text_position );
	pic0 = EndPicture();
	
	BeginPicture( new BPicture() );
	draw_button( this, rect, true );
	SetHighColor( 240, 240, 240 );
	DrawString( txt, text_position );
	pic1 = EndPicture();

	AddChild( new BPictureButton( rect.OffsetToCopy( 26, 24 ), NULL, pic0, pic1, new BMessage('Empt'), B_ONE_STATE_BUTTON, B_FOLLOW_LEFT|B_FOLLOW_TOP ) );
	
}

void SettingsView::RemoveButtons()
{
	BView	*view;
	while ( (view=ChildAt(0)) ) {
		RemoveChild( view );
		delete view;
	}
}

void SettingsView::Draw( BRect updateRect )
{
	DrawBitmap( fLimitBmp, BPoint( 4, 4 ) );
	DrawBitmap( fTrashBmp, BPoint( 4, 24 ) );
}


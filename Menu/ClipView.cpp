/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "ClipView.h"

#include <AppFileInfo.h>
#include <Bitmap.h>
#include <Mime.h>
#include <PictureButton.h>
#include <Window.h>

#include "constants.h"
#include "BubbleHelp.h"
#include "DeskbarView.h"
#include "get_resource_bitmap.h"
#include "MultiClipboard.h"

ClipView::ClipView( BRect frame, Clip *clip )
	:	BControl( frame, "clipview", NULL, new BMessage(), B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP, B_WILL_DRAW),
		fClip( clip ),
		fDocIcon( NULL ),
		fAppIcon( NULL ),
		fIsProtected( clip->fIsProtected ),
		fPicture( NULL ),
		fProtectButtonPressed( false ),
		fLastWidth( 0 )
{

	// ---- Document icon ----
	bool	foundit = false;
	
	// Is it of type "class"
	if ( clip->fClip.what == B_ARCHIVED_OBJECT) {
		const char *title;
		if (clip->fClip.FindString("class", &title)==B_OK) {
			if (strcmp("BBitmap", title)==0) {
				fDocIcon = new BBitmap( BRect(0, 0, 15, 15), B_CMAP8 );
				BMimeType("image").GetIcon(fDocIcon, B_MINI_ICON);
			}
			else {
				fDocIcon = get_resource_bitmap("object");
			}
			foundit = true;
		}
	}
	
	// ansonsten alle MIMEs rausholen...
	if (!foundit) {
		
		// The programmer should use one of the following for a mime type
		uint	types[] = { B_MIME_TYPE, B_MESSAGE_TYPE, B_STRING_TYPE };

		// find a valid mime type
		for (int i=0; i<(int)(sizeof(types)/sizeof(uint32)); ++i) {
			
			for (int j=0; ; ++j ) {

				char		*nameFound;
				type_code	typeFound;
				int32 		countFound;
		
				if ( clip->fClip.GetInfo(types[i], j, &nameFound, &typeFound, &countFound)!=B_OK || foundit ) break;
				
				BMimeType	mime( nameFound );
				
				if (mime.IsValid()) {
				
					fDocIcon = new BBitmap( BRect(0,0,15,15), B_CMAP8 );
					if ( mime.GetIcon(fDocIcon, B_MINI_ICON)==B_OK ) {
						foundit = true;
					}
					else {
						BMimeType	supertype;
						if (mime.GetSupertype(&supertype)==B_OK) {

							mime.SetTo( supertype.Type() );
							if ( mime.GetIcon(fDocIcon, B_MINI_ICON)==B_OK ) {
								foundit = true;
							}
						}
					}
				}
			}
			if (foundit) break;
		}
	}
	
	// if nothing is found, set it to "unknown type"
	if (!foundit)
		fDocIcon = get_resource_bitmap("unknown_clip");

	// ---- app icon ----
	fAppIcon = new BBitmap( BRect(0, 0, 15, 15), B_CMAP8 );
	
	clip->fSignature += char(0);
	
	entry_ref ref;
	if (BMimeType(clip->fSignature.data()).GetAppHint(&ref)==B_OK) {

		BFile			file(&ref, B_READ_WRITE);
		BAppFileInfo	appFileInfo(&file);

		appFileInfo.GetIcon(fAppIcon, B_MINI_ICON);
	}
	else
		BMimeType(clip->fSignature.data()).GetIcon(fAppIcon, B_MINI_ICON);

	// ---- text & font ----
	fText = clip->BubbleText();
	fText.ReplaceAll("\n","·");
	fText.ReplaceAll("\r", "·");
	fText.ReplaceAll("\t", "·");
	
}

ClipView::~ClipView()
{
	delete fDocIcon;
	delete fAppIcon;

	delete fPicture;
}

void ClipView::AttachedToWindow()
{
	BControl::AttachedToWindow();
	
	SetViewColor( normal_color );
	SetHighColor( normal_color );

	gBubbleHelp.SetHelp(this, fClip->BubbleText() );
	CreatePicture();

	SetEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS);

	SetTarget(Window());
}

void ClipView::Draw( BRect frame )
{

	BPoint location;
	uint32 buttons;
	GetMouse( &location, &buttons, false );

	Bounds().Contains(location);
	
	SetLowColor( IsFocus() ? highlight_color : normal_color );
	FillRect( Bounds(), B_SOLID_LOW );
	DrawPicture( fPicture, BPoint( 0, 0 ) );
	
}

void ClipView::DrawProtectButton( bool pressed )
{

	pressed &= fProtectButtonPressed;
	
	const char *name = fClip->fIsProtected ?
						(pressed ? "locked1" : "locked" ) :
						(pressed ? "unlocked1" : "unlocked");
						
	BBitmap	*bmp = get_resource_bitmap( name );

	SetDrawingMode(B_OP_ALPHA);
	DrawBitmap(bmp, BPoint(Bounds().right - 16, 3));
	delete bmp;

	SetDrawingMode(B_OP_COPY);
}

void ClipView::CreatePicture()
{
	if (fLastWidth == Frame().Width()) return;
	fLastWidth = Frame().Width();

	// Einstellungen
	fProtectButtonRect = BRect( 0, 0, 21, 17 ).OffsetToCopy( Bounds().right - 20, 0 );

	delete fPicture;
	
	BeginPicture( new BPicture() );

	SetLowColor( normal_color );
	SetDrawingMode( B_OP_ALPHA );

	DrawBitmap( fAppIcon, BPoint( 4, 1 ) );
	DrawBitmap( fDocIcon, BPoint( 30, 1 ) );
	
	SetDrawingMode( B_OP_COPY );

	BFont	font(be_plain_font);
	font.SetSize( 10.0 );
	
	SetFont( &font );

	BString	text = fText;
	font.TruncateString(&text, B_TRUNCATE_END, fProtectButtonRect.left - 54 );
	
	SetHighColor( 0, 0, 0 );

	font_height height;
	font.GetHeight(&height);

	DrawString( text.String(), BPoint( 52, 1 + height.ascent + height.descent ) );

	DrawProtectButton();

	fPicture = EndPicture();

	Invalidate();
			
}

Clip *ClipView::SourceClip() const
{
	return fClip;
}

void ClipView::MouseDown(BPoint where)
{
	if (!IsEnabled()) return;
	
	if (fProtectButtonRect.Contains( where ) ) {
		fProtectButtonPressed = true;
		DrawProtectButton( true );
	}
}

void ClipView::MouseUp(BPoint where)
{
	if (!IsEnabled()) return;

	if (fProtectButtonPressed ) {
		if (fProtectButtonRect.Contains( where ))
			SubmitProtect();

		fProtectButtonPressed = false;

		BPoint location;
		uint32 buttons;
		GetMouse( &location, &buttons, false );
	
		SetLowColor( Bounds().Contains(location) ? highlight_color : normal_color );
		FillRect( fProtectButtonRect, B_SOLID_LOW );

		DrawProtectButton();
	}
	else if ( Bounds().Contains( where ) )
		Submit();

}

void ClipView::MakeFocus(bool focusState)
{

	if ( !((BControl *)Parent())->Value() ) {
		((BControl *)Parent())->SetValue(1);
		((BControl *)Parent())->Invoke();
	}

	BControl::MakeFocus( focusState );
}

void ClipView::Submit()
{
	Message()->what = 'clip';
	Invoke();
}

void ClipView::SubmitProtect()
{
	fClip->fIsProtected = !fClip->fIsProtected;
	Message()->what = 'prot';
	Invoke();
}

void ClipView::MouseMoved( BPoint where, uint32 code, const BMessage *a_message)
{
	static BPoint	old_position;
	
	if (!IsEnabled() || old_position==ConvertToScreen(where)) return;
	
 	MakeFocus( Bounds().Contains( where ) );
 	
 	old_position = ConvertToScreen(where);

	if (fProtectButtonPressed) DrawProtectButton(fProtectButtonRect.Contains( where ));
}

void ClipView::KeyDown(const char *bytes, int32 numBytes)
{
	switch (bytes[0]) {

		case B_SPACE:
		case B_RETURN:
			Submit();
		break;
		
		case 'p':
			SubmitProtect();
		break;
		
		default:			
			Window()->PostMessage(Window()->CurrentMessage());
	}
}

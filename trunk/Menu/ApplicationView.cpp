/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "ApplicationView.h"

#include <Application.h>
#include <AppFileInfo.h>
#include <Bitmap.h>
#include <Font.h>
#include <Mime.h>
#include <PictureButton.h>
#include <Resources.h>
#include <Roster.h>

#ifdef DEBUG
#include <iostream>
#endif

#include "constants.h"
#include "draw_button.h"
#include "URLView.h"

ApplicationView::ApplicationView( BRect rect )
	:	BView( rect, NULL, B_FOLLOW_TOP|B_FOLLOW_LEFT_RIGHT, B_FRAME_EVENTS|B_WILL_DRAW ),
		fIcon( new BBitmap(BRect(0,0,15,15), B_CMAP8 ) )
{

	// Fetch app icon
	size_t			size;
	BMessage		message;
	char			*buf;
	BResources		resources;
	entry_ref		ref;
	BFile			file;
	
	if (be_roster->FindApp(APP_SIGNATURE, &ref)==B_OK) {
		
		file.SetTo( &ref, B_READ_ONLY );
		BResources	resources( &file );
		
		buf = (char *)resources.LoadResource('MICN', "BEOS:M:STD_ICON", &size);
		if (buf) fIcon->SetBits( buf, size, 0, B_CMAP8);
	}
	
	// Release Informationen
	version_info info;
	BAppFileInfo( &file ).GetVersionInfo(&info, B_APP_VERSION_KIND);
	
	if (info.major) {
		fRelease = "Release ";
		fRelease << info.major;
		fRelease << ".";
		fRelease << info.middle;
		fRelease << ".";
		fRelease << info.minor;
		fRelease << " ";
		fRelease << kTxtVersionStatus[info.variety];
	}
	else
		fRelease = "";
	
	// Schriften
	fTextFont = *be_plain_font;
	fTextFont.SetSize( 11.0 );

	fTitleFont.SetFamilyAndFace("Dutch801 Rm BT", B_ITALIC_FACE);
	fTitleFont.SetSize( 22.0 );
}

ApplicationView::~ApplicationView() {
	delete fIcon;
}

void ApplicationView::AttachedToWindow()
{
	BView::AttachedToWindow();
	if (Parent()) SetViewColor( Parent()->ViewColor() );
	
	SetFont( &fTextFont );
	
	AddButtons();
}

void ApplicationView::DetachedFromWindow()
{
	RemoveButtons();
	BView::DetachedFromWindow();
}

void ApplicationView::Draw(BRect updateRect)
{

	SetHighColor( 0, 0, 0 );
	SetLowColor( ViewColor() );
	
	SetFont(&fTitleFont);
	BPoint position((Frame().Width() - StringWidth(kAppName) - 4) / 2 - B_MINI_ICON, 5);
	
	SetDrawingMode(B_OP_OVER);
	DrawBitmapAsync( fIcon, position );
	SetDrawingMode(B_OP_COPY);
	
	position += BPoint(B_MINI_ICON + 4, 15);
	
	SetHighColor( 195, 195, 195 );
	DrawString(kAppName, position + BPoint(1,1));
	SetHighColor( 0, 0, 0 );
	DrawString(kAppName, position);
	
	SetFont( &fTextFont );
	position = BPoint( (Frame().Width() - StringWidth(fRelease.String())) / 2, 39 );
	DrawString(fRelease.String(), position );
	position = BPoint( (Frame().Width() - StringWidth(kCopyright)) / 2, 52);
	DrawString(kCopyright, position);
	
}

void ApplicationView::GetPreferredSize(float *width, float *height)
{
	float w = fTitleFont.StringWidth( kAppName ) + 20;
	w = max_c( w, fTextFont.StringWidth( fRelease.String() ) );
	w = max_c( w, fTextFont.StringWidth( kCopyright ) );
	w = max_c( w, fTextFont.StringWidth( kSite ) );
	w = max_c( w, fTextFont.StringWidth( kEmail ) );
	w = max_c( w, fTextFont.StringWidth( kTxtQuit ) + 12 );

	*width	= w + 8;
	*height	= 134;
	
}

void ApplicationView::FrameResized(float new_width, float new_height)
{
	RemoveButtons();
	AddButtons();
	Invalidate();
}

void ApplicationView::AddButtons()
{

	// add links
	rgb_color linkcolor = { 10, 40, 150, 255 };
	rgb_color hovercolor = { 20, 50, 255, 255 };
	rgb_color clickcolor = { 240, 0, 0, 255 };

	// E-Mail
	BRect	rect(  BRect(0, 0, StringWidth(kEmail), 13).OffsetToCopy((Frame().Width() - StringWidth(kEmail)) / 2, 61) );
	URLView *email = new URLView( rect, "E-mail", kEmailText, kEmail );
	email->SetFontSize( 11.0 );
	email->SetColor( linkcolor );
	email->SetHoverColor( hovercolor );
	email->SetClickColor( clickcolor );
	email->SetUnderlineThickness( 0 );
	email->SetPopUpEnabled( false );
	email->AddAttribute( "META:name", kEmailText );
	email->AddAttribute( "META:url", kSite );
	AddChild( email );

	// URL
	rect = BRect(0, 0, StringWidth(kSite), 13).OffsetToCopy((Frame().Width() - StringWidth(kSite)) / 2, 75);
	URLView *web = new URLView( rect, "Web", kSiteText, kSite );
	web->SetFontSize( 11.0 );
	web->SetColor( linkcolor );
	web->SetHoverColor( hovercolor );
	web->SetClickColor( clickcolor );
	web->SetPopUpEnabled( false );
	web->SetUnderlineThickness( 0 );
	AddChild( web );

	// Quit button
	const char *txt;
	BPoint text_position;
	BPicture *pic0, *pic1;
	
	txt = kTxtQuit;
	
	rect = BRect ( 0, 0, StringWidth(txt) + 14, 18 );
	text_position = BPoint ( (rect.Width() - StringWidth(txt)) / 2, 13 );

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

	AddChild( new BPictureButton( rect.OffsetToCopy( (Bounds().Width() - rect.Width())/2, 96 ), NULL, pic0, pic1, new BMessage('Quit'), B_ONE_STATE_BUTTON, B_FOLLOW_LEFT|B_FOLLOW_TOP ) );
}

void ApplicationView::RemoveButtons()
{
	BView	*view;
	while ( (view=ChildAt(0)) ) {
		RemoveChild( view );
		delete view;
	}
}

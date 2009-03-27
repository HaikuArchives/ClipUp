#include "ArrowedView.h"

#include <Region.h>

#if DEBUG
#include <iostream>
#endif
#include <string>

#include "constants.h"
#include "Prefs.h"

ArrowedView::ArrowedView( BRect frame, const char *name, const char *label )
	:	BControl( frame, name, label, new BMessage('Arro'), B_FOLLOW_LEFT_RIGHT|B_FOLLOW_TOP, B_WILL_DRAW )

{

	fArrowButton = new ArrowButton( BPoint(0, 0), NULL, label, new BMessage('Arrw') );
	AddChild( fArrowButton );
	
	ResizeTo( Frame().Width(), fArrowButton->Frame().Height() + 1 );
	
}

ArrowedView::~ArrowedView()
{
}

void ArrowedView::DetachedFromWindow()
{
	// save prefs
	if (Name()==0) return;
	
	string name = "arrow_open_";
	name += Name();
	name += char(0);
		
	prefs.RemoveName(name.data() );
	prefs.AddInt32(name.data(), Value() );
	
	BView::DetachedFromWindow();
}

void ArrowedView::AttachedToWindow()
{

	// initialisation
	BView::AttachedToWindow();
	
	SetViewColor( normal_color );
	SetLowColor( normal_color );

	fArrowButton->SetTarget( this );
	
	// load prefs
	if (Name()==0) return;

	string name = "arrow_open_";
	name += Name();
	name += char(0);
		
	SetValue( prefs.FindInt32(name.data()) );

}

void ArrowedView::MessageReceived(BMessage *msg)
{

	if (msg->what=='Arrw') {
		SetValue( msg->FindInt32("be:value") );
		Invoke();
	}
	else {
		BControl::MessageReceived( msg );
	}

}

void ArrowedView::SetValue( int32 value )
{
	BControl::SetValue( value );
	fArrowButton->ChangeState( value );
}

void ArrowedView::Draw( BRect updateRect )
{

	if (CountChildren()==1) {

		const char *kEmpty = "<Empty>";
		
		BFont	font;
		font = *be_plain_font;
		font.SetSize( 10.0 );
	
		SetFont( &font );
		
		BPoint	pos( 22, 27 );
		
		SetHighColor( 255, 255, 255 );
		DrawString( kEmpty, pos );
		
		SetHighColor( 154, 154, 154 );
		DrawString( kEmpty, pos - BPoint( 1, 1 ) );
	}
}

void ArrowedView::SizeChangeComplete()
{
	// for inherited classes: do some update after resizing
}

void ArrowedView::GetPreferredSize(float *width, float *height)
{
	fArrowButton->GetPreferredSize(width, height);
	(*height) ++;

	if (Value()) {
		if (CountChildren()==1) {
			*height += 15;
		}
		else {
			*width	= max_c( *width, fMaxWidth );
			*height	= max_c( *height, fMaxHeight );

			(*height) ++;
		}
	}
}

void ArrowedView::SetSizeLimit( float max_width, float max_height )
{

	fMaxWidth = max_width;
	fMaxHeight = max_height;

}

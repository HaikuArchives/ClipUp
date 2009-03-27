#include "SlideView.h"

#include <String.h>

#if DEBUG
#include <iostream>
#endif

#include "constants.h"
#include "Prefs.h"
#include "RealtimeSlider.h"

SlideView::SlideView( BRect frame, const char *label, BMessage *message, int32 minValue,
			int32 maxValue )
	:	BControl( frame, NULL, label, message, B_FOLLOW_LEFT, B_WILL_DRAW )
{
	BFont	font;
	font = *be_plain_font;
	font.SetSize( 10.0 );
	SetFont( &font );

	BString	number;
	number << maxValue;
	
	fNumberRect = BRect( 0, 0, StringWidth(number.String()) + 6, 14);
	fNumberRect.OffsetTo( StringWidth( label ) + 4, (frame.Height() - fNumberRect.Height())/2 );
	
	BRect rect( Bounds() );
	rect.left = fNumberRect.right + 1;
	rect.top += 1;
	rect.right -= 10;
	
	fSlider = new RealtimeSlider(	rect,
									NULL,
									NULL,
									new BMessage('slid'),
									minValue,
									maxValue,
									B_BLOCK_THUMB,
									B_FOLLOW_LEFT | B_FOLLOW_TOP );
	fSlider->SetDefaultValue( 10 );

	AddChild( fSlider );
	
	int32	value;
	if (prefs.FindInt32("clip_limit", &value)!=B_OK) {
		value = DEFAULT_LIMIT;
		prefs.AddInt32("clip_limit", Value() );
	}

	SetValue( value );
	fSlider->SetValue( value );
	
}

void SlideView::AttachedToWindow()
{
	BControl::AttachedToWindow();
	fSlider->SetTarget( this );
}

void SlideView::MessageReceived( BMessage *msg )
{
	if (msg->what=='slid') {
		SetValue( msg->FindInt32("be:value") );
		if (!fSlider->IsMouseDown()) {
			prefs.RemoveName("clip_limit");
			prefs.AddInt32("clip_limit", Value() );
			Invoke();
		}
	}
	else {
		BControl::MessageReceived(msg);
	}
}

void SlideView::Draw( BRect bounds )
{
	BControl::Draw( bounds );
	
	SetHighColor( 0, 0, 0 );
	DrawString( Label(), BPoint( 0, fNumberRect.top + 11 ) );
	
	SetLowColor( 245, 245, 245 );
	FillRoundRect( fNumberRect, 1.5, 1.5, B_SOLID_LOW );
	
	BRect rect = fNumberRect.InsetByCopy( 1.0, 1.0 );

	SetHighColor( 255, 255, 255 );
	StrokeLine( rect.LeftBottom(), rect.RightBottom() );
	StrokeLine( rect.RightTop() );
	
	SetHighColor( 200, 200, 200 );
	StrokeLine( rect.LeftTop() );
	StrokeLine( rect.LeftBottom() );

	SetHighColor( 80, 80, 80 );
	StrokeRoundRect( fNumberRect, 1.5, 1.5 );

	BString str;
	str << Value();

	SetHighColor( 50, 50, 50 );
	DrawString( str.String(), BPoint( fNumberRect.right - StringWidth(str.String()) - 2, fNumberRect.top + 11 ) );
	
}

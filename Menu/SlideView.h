#ifndef _SLIDE_VIEW_H
#define _SLIDE_VIEW_H

#include <Control.h>

class RealtimeSlider;

class SlideView : public BControl
{

public:
				SlideView( 	BRect frame,
							const char *label,
							BMessage *message,
							int32 minValue = 0,
							int32 maxValue = 100);
				
void			AttachedToWindow();

void			MessageReceived( BMessage *msg );
void			Draw( BRect bounds );

private:

BRect			fNumberRect;
RealtimeSlider	*fSlider;
};

#endif
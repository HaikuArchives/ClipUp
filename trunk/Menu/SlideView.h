/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

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

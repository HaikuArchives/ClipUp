/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "draw_button.h"

#include <View.h>
#include <Window.h>

void draw_button(BView *view, BRect frame, bool pressed)
{

	if ( !view->Window() || !view->Window()->Lock() ) return;
	
	char	c_border = 96;
	char	c_hg = pressed ? 40 : 228;
	
	view->SetHighColor(c_border, c_border, c_border);
	view->StrokeRoundRect(frame, 1.5, 1.5);
	
	frame.InsetBy( 1.0, 1.0 );

	view->SetHighColor(c_hg, c_hg, c_hg);
	view->SetLowColor(c_hg, c_hg, c_hg);
	view->FillRect( frame );

	view->Window()->Unlock();
	
};

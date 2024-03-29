/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _POPUPWIN_H
#define _POPUPWIN_H

#include <Roster.h>
#include <Window.h>

#include <vector>

class ArrowedView;

class PopUpWindow : public BWindow {

public:
						PopUpWindow();

virtual void			WindowActivated(bool state);

virtual void			MessageReceived(BMessage *msg);

virtual	void			Show();
virtual	void			Hide();

void					ResizeToPreferredSize( bool with_effect = true );
void					GetPreferredSize( float *width, float *height );

private:

void					SizeEffect( float new_width, float new_height );

void					RemoveArrowedViews();
void					AddArrowedViews( bool is_update );

BView					*fMainView;

std::vector<ArrowedView *>	fViews;

team_id					fActiveApp;
mode_mouse				fMouseMode;

};

extern PopUpWindow *gPopUpWindow;

#endif

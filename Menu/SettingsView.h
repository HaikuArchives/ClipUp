#ifndef _SETTINGS_VIEW_H
#define _SETTINGS_VIEW_H

#include <View.h>

class SettingsView : public BView
{
public:
			SettingsView( BRect rect );
			~SettingsView();
			
void		AttachedToWindow();
void		DetachedFromWindow();

void		Draw( BRect updateRect );

void		GetPreferredSize(float *width, float *height);
void		FrameResized(float new_width, float new_height);

void		AddButtons();
void		RemoveButtons();

private:

BBitmap		*fLimitBmp, *fTrashBmp;

};

#endif
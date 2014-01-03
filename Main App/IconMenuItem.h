#ifndef _ICON_MENU_ITEM_H
#define _ICON_MENU_ITEM_H

#include <MenuItem.h>

class IconMenuItem : public BMenuItem {

public:
			IconMenuItem( const char *label,
							BMessage *message,
							BBitmap *icon = NULL,
							char shortcut = 0,
							uint32 modifiers = 0);
			IconMenuItem(BMenu *menu, BMessage *message = NULL,
							BBitmap *icon = NULL);
			
			~IconMenuItem();
						
void		GetContentSize(float *width, float *height);
void		DrawContent();

private:

BBitmap		*fIcon;

};

#endif
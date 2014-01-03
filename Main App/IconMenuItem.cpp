#include "IconMenuItem.h"

#include <Bitmap.h>
#include <Mime.h>

#if __INTEL__
#include <iostream.h>
#endif

IconMenuItem::IconMenuItem( const char *label, BMessage *message, BBitmap *icon,
							char shortcut, uint32 modifiers)
	:	BMenuItem( label, message, shortcut, modifiers ),
		fIcon( icon )
{
}

IconMenuItem::IconMenuItem(BMenu *menu, BMessage *message, BBitmap *icon)
	:	BMenuItem( menu, message ),
		fIcon( icon )
{
}
							
IconMenuItem::~IconMenuItem() {
	delete fIcon;
}
						
void IconMenuItem::GetContentSize(float *width, float *height) {
	
	BMenuItem::GetContentSize(width, height);
	
	if (fIcon) {
		*height = max_c ( *height, B_MINI_ICON );
		*width += B_MINI_ICON + 6;
	}
}

void IconMenuItem::DrawContent() {

	if (fIcon) {

		Menu()->SetDrawingMode( B_OP_ALPHA );
		BPoint	location = ContentLocation();
		Menu()->DrawBitmap( fIcon, BPoint( location.x, location.y - 1) );

		if (!IsEnabled()) {

			rgb_color old_color = Menu()->HighColor();

			rgb_color color = Menu()->ViewColor();
			color.alpha = 140;
			Menu()->SetHighColor(color);
			Menu()->FillRect( fIcon->Bounds().OffsetToCopy(BPoint( location.x, location.y - 1)));

			Menu()->SetHighColor( old_color );
		}
		
		Menu()->SetDrawingMode( B_OP_COPY );
		font_height f_height;
		Menu()->GetFontHeight(&f_height);

		float height = f_height.ascent + f_height.descent;
		
		Menu()->MovePenBy( B_MINI_ICON + 6, max_c( 0.0, (B_MINI_ICON - height)/2) );

	}

	BMenuItem::DrawContent();

}


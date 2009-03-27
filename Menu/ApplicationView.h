#ifndef _APPLICATION_VIEW_H
#define _APPLICATION_VIEW_H

#include <String.h>
#include <View.h>

class ApplicationView : public BView
{

public:
			ApplicationView( BRect rect );
			~ApplicationView();
			
void		AttachedToWindow();
void		DetachedFromWindow();

void		Draw(BRect updateRect);
		
void		GetPreferredSize(float *width, float *height);
void		FrameResized(float new_width, float new_height);

void		AddButtons();
void		RemoveButtons();

private:

BBitmap			*fIcon;
BString			fRelease;
BFont			fTitleFont, fTextFont;
};

#endif
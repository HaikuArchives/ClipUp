#ifndef _MAIN_VIEW_H
#define _MAIN_VIEW_H

#include <View.h>

class MainView : public BView {

public:

			MainView();

void		AttachedToWindow();

void		Draw( BRect rect );
void		FrameResized(float new_width, float new_height);

void		MouseDown(BPoint where);
void		MouseUp(BPoint where);
void		MouseMoved(	BPoint where, uint32 code, const BMessage *a_message);

bool		MouseIsDown() const;

private:

bool		fMouseDown;
BPoint		fMouseOffset;
BPoint		fWindowMoveOffset;

};

#endif
#ifndef ARROWBUTTON_H
#define ARROWBUTTON_H

#include <PictureButton.h>

class BMessageRunner;

class ArrowButton : public BPictureButton {

public:
					ArrowButton( BPoint offset, const char *name, const char *label, BMessage *msg );
virtual				~ArrowButton();

void				AttachedToWindow();

void				Draw( BRect updateRect );

void				ChangeState( int32 value );

virtual	void		MessageReceived(BMessage *msg);

virtual status_t	Invoke(BMessage *msg = NULL);

virtual	void		MouseDown(BPoint where);
virtual	void		MouseUp(BPoint where);
virtual	void		MouseMoved(	BPoint where,
								uint32 code,
								const BMessage *a_message);

virtual void		GetPreferredSize(float *width, float *height);

private:

void				CreatePictures();
BMessageRunner		*fMessageRunner;
bool				fMouseDown;
bool				fForcedUp;			// Letzter MouseUp wurde erzwungen durch MessageRunner
};

#endif
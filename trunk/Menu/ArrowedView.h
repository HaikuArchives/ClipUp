/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _ARROWED_VIEW_H
#define _ARROWED_VIEW_H

#include <View.h>

#include "ArrowButton.h"

// Value() is the expansion status: TRUE = expanded, FALSE = compressed

class ArrowedView : public BControl {

public:
				ArrowedView( BRect frame, const char *name, const char *label );
virtual			~ArrowedView();
					
virtual void	AttachedToWindow();
virtual	void	DetachedFromWindow();

virtual	void	MessageReceived(BMessage *msg);

virtual void	SetValue( int32 value );

virtual void	Draw( BRect updateRect );

virtual void	SizeChangeComplete();

void			GetPreferredSize(float *width, float *height);
void			SetSizeLimit( float max_width, float max_height );

protected:

ArrowButton		*fArrowButton;
float			fMaxWidth, fMaxHeight;

};

#endif

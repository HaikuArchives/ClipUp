/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _CLIP_VIEW_H
#define _CLIP_VIEW_H

#include <String.h>
#include <Control.h>

#include "Clip.h"

class BBitmap;
class BPicture;

class ClipView : public BControl {

public:
				ClipView( BRect frame, Clip *clip );
				~ClipView();

void			AttachedToWindow();
void			Draw( BRect frame );
void			DrawProtectButton( bool pressed = false );

void			CreatePicture();

Clip			*SourceClip() const;

void			KeyDown(const char *bytes, int32 numBytes);

void			MouseDown(BPoint where);
void			MouseUp(BPoint where);
void			MouseMoved(	BPoint where, uint32 code,
					const BMessage *a_message);

void			MakeFocus(bool focusState = true);

void			Submit();
void 			SubmitProtect();

private:

Clip			*fClip;
BBitmap			*fDocIcon, *fAppIcon;
BString			fText;
bool			fIsProtected;
BPicture		*fPicture;

bool			fProtectButtonPressed;
BRect			fProtectButtonRect;

float			fLastWidth;
};

#endif

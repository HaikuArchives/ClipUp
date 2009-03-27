/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _CLIP_LIST_VIEW_H
#define _CLIP_LIST_VIEW_H

#include <List.h>

#include "ArrowedView.h"

enum clv_type { CLV_CURRENT_CLIP, CLV_UNPROTECTED_CLIPS, CLV_PROTECTED_CLIPS };

class ClipListView : public ArrowedView
{

public:
				ClipListView(	clv_type type,
								BRect frame,
								const char *name,
								const char *label );

virtual void	AttachedToWindow();

virtual void	SizeChangeComplete();
virtual void	SetEnabled( bool on );

inline void		AddClip( int i, int pos );
void			UpdateClips();

private:

BList			fClipViews;
clv_type		fType;

};

#endif

/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#ifndef _MULTI_CLIPBOARD_H
#define _MULTI_CLIPBOARD_H

#include <Bitmap.h>
#include <List.h>
#include <Message.h>
#include <Path.h>

#include <string>

class Clip;

class MultiClipboard
	:	public BList
{

public:

					MultiClipboard();
					~MultiClipboard();

void				FetchSystemClipboard( bool getApplicationName = true );

int32				Limit() { return fLimit; }
void				SetLimit( int32 limit );
void				ApplyLimit();

void				MakeEmpty();
void				RemoveHistory();

status_t			ActivateClip( Clip *clip );
status_t			ActivateClip( int32 pos );

const char			*GetCurrentBubbleText() const;

private:

BPath				fSavePath;
int32				fLimit;

bool				fInit;
};

extern MultiClipboard	gMultiClipboard;

#endif

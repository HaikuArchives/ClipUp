#ifndef _MULTI_CLIPBOARD_H
#define _MULTI_CLIPBOARD_H

#include <List.h>
#include <Path.h>

#define	SAVE_FILENAME	"clipup_clips"
#define DEFAULT_LIMIT	10

class IconMenuItem;

class MultiClipboard : public BList {

public:

				MultiClipboard();
				~MultiClipboard();

void			FetchSystemClipboard();
void			Empty();

void			SetLimit( int32 limit );

IconMenuItem	*CreateMenuItem( int32 index );

private:

BPath			fSavePath;
int32			fLimit;

};

#endif
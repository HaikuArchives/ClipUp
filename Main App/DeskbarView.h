#ifndef _DESKBAR_VIEW_H
#define _DESKBAR_VIEW_H

#include <View.h>
#include <OS.h>

#define VIEW_NAME			"ClipUp Deskbar View"
#define VIEW_RECT			BRect(0, 0, 15, 15)

#define INPUT_PORT_NAME		"ClipUp input port"
#define OUTPUT_PORT_NAME	"ClipUp output port"

#define CLIP_LIMIT_BASE		'Limt'

class _EXPORT DeskbarView;

class BBitmap;
class BMessageRunner;
class MultiClipboard;

class DeskbarView : public BView
{

public:

				DeskbarView();
				DeskbarView(BMessage *archive);
				~DeskbarView();
				
void			Init( bool complete = true );
		
void			AttachedToWindow(void);
void			Draw(BRect rect);
void			MessageReceived( BMessage *msg );

void			OpenPopUpMenu( bool send_paste = false );

void			MouseDown(BPoint where);
void			Pulse();

static void		AddToDeskbar();
static void		RemoveFromDeskbar();
		
static			BArchivable *Instantiate(BMessage *data);
status_t		Archive(BMessage *data, bool deep = true) const;

private:

bool			fMouseDown;
bool			fSendPaste;

BBitmap			*fIcon;
BPopUpMenu		*fPopUpMenu;
MultiClipboard	*fMultiClipboard;
BMessageRunner	*fMessageRunner;
port_id			fPort;
};

#endif

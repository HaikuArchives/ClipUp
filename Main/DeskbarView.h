#ifndef _DESKBAR_VIEW_H
#define _DESKBAR_VIEW_H

#include <View.h>
#include <OS.h>

#define VIEW_NAME			"ClipUp2 Deskbar View"
#define VIEW_RECT			BRect(0, 0, 15, 15)

#define INPUT_PORT_NAME		"ClipUp input port"
#define OUTPUT_PORT_NAME	"ClipUp output port"

class _EXPORT DeskbarView;

class BBitmap;
class BMessageRunner;
class Clip;
class MultiClipboard;
class PopUpWindow;

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

void			SetHelp();

void			OpenPopUpWindow( bool send_paste = false );
void			UpdateClip();

void			MouseDown(BPoint where);
void			Pulse();

static void		AddToDeskbar();
static void		RemoveFromDeskbar();
		
static			BArchivable *Instantiate(BMessage *data);
status_t		Archive(BMessage *data, bool deep = true) const;

private:

bool			fSendPaste;

BBitmap			*fIcon;
BMessageRunner	*fMessageRunner;
port_id			fPort;
};

extern DeskbarView *gDeskbarView;

#endif

#include "DeskbarView.h"

#include <Alert.h>
#include <AppFileInfo.h>
#include <Bitmap.h>
#include <Clipboard.h>
#include <Deskbar.h>
#include <Entry.h>
#include <File.h>
#include <MenuItem.h>
#include <Message.h>
#include <MessageRunner.h>
#include <PopUpMenu.h>
#include <Resources.h>
#include <Roster.h>
#include <Screen.h>
#include <String.h>
#include <Window.h>

#include <string>

#if DEBUG
#include <iostream.h>
#endif

#include "BubbleHelp.h"
#include "constants.h"
#include "MultiClipboard.h"
#include "PopUpWindow.h"
#include "Prefs.h"

extern "C" _EXPORT BView *instantiate_deskbar_item();

DeskbarView *gDeskbarView = NULL;

BView *instantiate_deskbar_item()
{
	return new DeskbarView();
}

DeskbarView::DeskbarView()
	:	BView( VIEW_RECT, VIEW_NAME, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP_BOTTOM, 
			B_WILL_DRAW|B_PULSE_NEEDED)
{

	// Dieser Konstruktor wird über main() aufgerufen, das Programm wird aber gleich 
	// wieder beendet. Daher nur minimale Initialisierung

	Init( false );

}

DeskbarView::DeskbarView(BMessage *archive)
	: BView(archive)
{
	Init();
}

DeskbarView::~DeskbarView() {

	if (gPopUpWindow) gPopUpWindow->PostMessage(B_QUIT_REQUESTED);

	delete fIcon;
	delete fMessageRunner;
	
	be_clipboard->StopWatching(BMessenger(this));
}

void DeskbarView::Init( bool complete ) {

	// Init global pointer
	gDeskbarView = this;
	
	// some presets
	fIcon = NULL;
	fMessageRunner = NULL;
	
	if (!complete) return;
	
	// Icon
	entry_ref ref;
	be_roster->FindApp( APP_SIGNATURE, &ref);

	BFile		file(&ref, B_READ_ONLY);
	BResources	resources( &file );

	size_t		groesse;
	BMessage	msg;
	char		*buf = (char *)resources.LoadResource('BBMP', "deskbaricon", &groesse);
	
	if (buf) {
		msg.Unflatten(buf);
		fIcon = new BBitmap( &msg );
	}

	// Initialisieren
	fPort = create_port(20, INPUT_PORT_NAME);

	// Limit finden, nötigenfalls initialisieren
	int32	limit = prefs.FindInt32("clip_limit");
	if (limit<=1) {
		limit = DEFAULT_LIMIT;
		prefs.AddInt32("clip_limit", limit );
	}
	gMultiClipboard.SetLimit( limit );

	// Clipboard-Überwachung aktivieren
	be_clipboard->StartWatching(BMessenger(this));

	// Create PopUpWindow
	gPopUpWindow = new PopUpWindow();
	// We have to run it else we can't lock it!
	gPopUpWindow->Run();
	
}

void DeskbarView::AttachedToWindow(void) 
{
	SetViewColor(Parent()->ViewColor());
	SetDrawingMode( B_OP_ALPHA );

	BView::AttachedToWindow();
	
	be_clipboard->StartWatching(BMessenger(this));

	SetHelp();

}

void DeskbarView::Draw(BRect rect)
{
	DrawBitmap(fIcon, BPoint(0.0, 0.0));
}

void DeskbarView::SetHelp() {

	gBubbleHelp.SetHelp(this, gMultiClipboard.GetCurrentBubbleText() );

}

void DeskbarView::OpenPopUpWindow( bool send_paste ) {

	gPopUpWindow->Lock();
	gPopUpWindow->Hide();
	gPopUpWindow->Unlock();

	fSendPaste = send_paste;
	
	BScreen screen;
	BPoint where;

	float width, height;
	gPopUpWindow->GetPreferredSize( &width, &height );

	if (!send_paste) {
		uint32 buttons;
		GetMouse( &where, &buttons );
		ConvertToScreen(&where);

		where.x -= width / 2;
	
		if (where.y + width>screen.Frame().bottom)
			where.y -= height + 1;
		else where.y ++;
		
	}
	else {
		where.x = (screen.Frame().Width() - width) / 2;
		where.y = (screen.Frame().Height() - height) / 2 - 4;
	}

	gPopUpWindow->Lock();
	gPopUpWindow->MoveTo( where );
	gPopUpWindow->Show();
	gPopUpWindow->Unlock();
	
}

void DeskbarView::MouseDown(BPoint where)
{
	BView::MouseDown( where );
	
 	OpenPopUpWindow();
}

void DeskbarView::Pulse() {

	port_info info;
	get_port_info(fPort, &info);
	if (info.queue_count) {
		int32 code;
		read_port(fPort, &code, NULL, 0 );

		if (code=='CtSV')
			OpenPopUpWindow( true );
	}
}

void DeskbarView::MessageReceived( BMessage *msg ) {

	// Limit ändern
	if (msg->what>CLIP_LIMIT_BASE && msg->what <= CLIP_LIMIT_BASE + 100) {
		gMultiClipboard.SetLimit( msg->what - CLIP_LIMIT_BASE );
		return;
	}

	switch (msg->what) {

		case 'Draw': {
				Draw( Bounds() );
			} break;
		
		case B_QUIT_REQUESTED: {

				BAlert *alert = new BAlert( "", kTxtQuitQuestion, 
					kTxtQuitYes, kTxtQuitNo, NULL, B_WIDTH_AS_USUAL, 
					B_STOP_ALERT);
					
				if (!alert->Go()) {

					gMultiClipboard.RemoveHistory();

					BDeskbar deskbar;
					deskbar.RemoveItem(VIEW_NAME);
				}

			} break;

		case B_CLIPBOARD_CHANGED: {
				
				Draw( Bounds() );
				
				SetHighColor( 60, 60, 100, 150 );
				for (int i=0; i<4; ++i)
					StrokeLine( BPoint(4.0, 5.0 + i * 2), BPoint(11.0, 5.0 + i * 2) );

				gMultiClipboard.FetchSystemClipboard();
				
				SetHelp();
				
				delete fMessageRunner;
				fMessageRunner = new BMessageRunner(BMessenger(this), 
					new BMessage('Draw'), 100000, 1);
				
			} break;
		
		default:
			BView::MessageReceived( msg );
	}
}

void DeskbarView::UpdateClip()
{
	if (fSendPaste) {

		port_id port = find_port(OUTPUT_PORT_NAME);

		if (port!=B_NAME_NOT_FOUND)
			write_port( port, 'CtrV', NULL, 0 );
		
	}
	
	SetHelp();
}

void DeskbarView::AddToDeskbar()
{
#if B_BEOS_VERSION >= B_BEOS_VERSION_5
	BDeskbar *deskbar = new BDeskbar();
	entry_ref ref;
	be_roster->FindApp(APP_SIGNATURE, &ref);
	deskbar->AddItem(&ref);
	delete deskbar;
#else
	BDeskbar *deskbar = new BDeskbar();
	DeskbarView *replicant = new DeskbarView( VIEW_RECT );
	
	err = deskbar->AddItem(replicant);
	delete replicant;
	delete deskbar;
#endif
}

void DeskbarView::RemoveFromDeskbar() {

	BDeskbar deskbar;
	deskbar.RemoveItem(VIEW_NAME);

}

BArchivable *DeskbarView::Instantiate(BMessage *data)
{
	if(!validate_instantiation(data, VIEW_NAME))
		return NULL;
	return new DeskbarView(data);
}

status_t DeskbarView::Archive(BMessage *data, bool deep) const
{
	BView::Archive(data, deep);
	data->AddString("add_on", APP_SIGNATURE);
	data->AddString("class", VIEW_NAME);
	return B_OK;
}

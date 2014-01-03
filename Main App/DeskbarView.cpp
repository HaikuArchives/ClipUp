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
#include <Roster.h>
#include <String.h>
#include <Window.h>

#if __INTEL__
#include <iostream.h>
#endif

#include "App.h"
#include "BubbleHelp.h"
#include "IconMenuItem.h"
#include "MultiClipboard.h"
#include "Prefs.h"

extern "C" _EXPORT BView *instantiate_deskbar_item();

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

	delete fIcon;
	delete fPopUpMenu;
	delete fMultiClipboard;
	delete fMessageRunner;
	
	be_clipboard->StopWatching(BMessenger(this));
}

void DeskbarView::Init( bool complete ) {

	fIcon = NULL;
	fPopUpMenu = NULL;
	fMultiClipboard = NULL;
	fMessageRunner = NULL;
	
	if (!complete) return;
	
	// Icon
	entry_ref ref;
	be_roster->FindApp( APP_SIGNATURE, &ref);

	BFile			file(&ref, B_READ_ONLY);
	BAppFileInfo	appFileInfo(&file);
	
	fIcon = new BBitmap(BRect(0,0,15,15), B_CMAP8 );
	appFileInfo.GetIcon(fIcon, B_MINI_ICON);

	// Initialisieren
	fMouseDown = false;
	fMultiClipboard = new MultiClipboard();
	fPort = create_port(20, INPUT_PORT_NAME);

	// Limit finden, nötigenfalls initialisieren
	int32	limit = prefs.FindInt32("clip_limit");
	if (limit<=1) {
		limit = DEFAULT_LIMIT;
		prefs.AddInt32("clip_limit", limit );
	}
	fMultiClipboard->SetLimit( limit );

	// Clipboard-Überwachung aktivieren
	be_clipboard->StartWatching(BMessenger(this));

}

void DeskbarView::AttachedToWindow(void) 
{
	SetViewColor(Parent()->ViewColor());
	SetDrawingMode( B_OP_ALPHA );

	BView::AttachedToWindow();
	
	be_clipboard->StartWatching(BMessenger(this));

	gBubbleHelp.SetHelp(this, "ClipUp");
				
}

void DeskbarView::Draw(BRect rect)
{
	DrawBitmap(fIcon, BPoint(0.0, 0.0));
}

void DeskbarView::OpenPopUpMenu( bool send_paste ) {
	
 	if (fPopUpMenu && fPopUpMenu->Window()) return;
 	
 	fSendPaste = send_paste;
	
	delete fPopUpMenu;
	
	fPopUpMenu = new BPopUpMenu("", false, false);
	fPopUpMenu->SetFontSize( 10.0 );
	
	BMenuItem	*item;

	int count = fMultiClipboard->CountItems();
	
	if (!count) {
		item = new BMenuItem("No clips", NULL);
		item->SetEnabled( false );
		fPopUpMenu->AddItem( item );
	}
	else {
		for (int i=0; i<count; ++i) {
				
			if (i==1) fPopUpMenu->AddItem( new BSeparatorItem() );

			fPopUpMenu->AddItem( (BMenuItem *)fMultiClipboard->CreateMenuItem( i ) );
			
		}

	}
	
	fPopUpMenu->AddItem( new BSeparatorItem() );

	BMenu *menu = new BMenu("Limit history list to");

	int32	limits[] = { 5, 10, 15, 20, 25, 50 };

	for (int i=0; i<(int)(sizeof(limits)/sizeof(int32)); ++i) {
		BString mt;
		mt << limits[i];
		mt << " clips";
		item = new BMenuItem(mt.String(), new BMessage(CLIP_LIMIT_BASE + limits[i]));
		if (limits[i]==prefs.FindInt32("clip_limit")) item->SetMarked( true );
		menu->AddItem(item);
	}
	menu->SetRadioMode( true );
	menu->SetTargetForItems(BMessenger(this));

	item = (BMenuItem *)new IconMenuItem(menu, NULL, get_resource_bitmap("maximum"));
	fPopUpMenu->AddItem( item );
	
	item = (BMenuItem *)new IconMenuItem("Empty history list and clipboard", new BMessage('Empt'), get_resource_bitmap("empty"));
	if (!count) item->SetEnabled( false );
	fPopUpMenu->AddItem( item );
	
	fPopUpMenu->AddItem( new BSeparatorItem() );

	item = new BMenuItem("About...", new BMessage(B_ABOUT_REQUESTED));
	fPopUpMenu->AddItem( item );

	fPopUpMenu->AddItem( new BSeparatorItem() );

	item = new BMenuItem("Quit", new BMessage(B_QUIT_REQUESTED));
	fPopUpMenu->AddItem( item );
	
	fPopUpMenu->SetTargetForItems(BMessenger(this));

	BPoint where;
	uint32 buttons;
	GetMouse( &where, &buttons );

	ConvertToScreen(&where);
	BRect r(-5, -5, 5, 5);
	r = ConvertToScreen(r);
	r.OffsetTo(where);
	
	fPopUpMenu->Go(where, true, true, r, true);

}

void DeskbarView::MouseDown(BPoint where) {
	
	OpenPopUpMenu();

}

void DeskbarView::Pulse() {

	port_info info;
	get_port_info(fPort, &info);
	if (info.queue_count) {
		int32 code;
		read_port(fPort, &code, NULL, 0 );
		if (code=='CtSV')
			OpenPopUpMenu( true );
	}
}

void DeskbarView::MessageReceived( BMessage *msg ) {

	// Clip verschieben
	if (msg->what>='Clip' && (int)(msg->what - 'Clip') < fMultiClipboard->CountItems() ) {

		if (msg->what>'Clip') {
		
			be_clipboard->StopWatching(BMessenger(this));
	
			if (be_clipboard->Lock()) {
			
				be_clipboard->Clear();
			
				BMessage *clip = be_clipboard->Data();
			
				if (clip) {
					
					BMessage *new_clip = (BMessage *)fMultiClipboard->RemoveItem(msg->what - 'Clip');
					fMultiClipboard->AddItem( new_clip, 0 );
					
					*clip = *new_clip;
	
					be_clipboard->Commit();
					
				}
				
				be_clipboard->Unlock();
				
			}
	
			be_clipboard->StartWatching(BMessenger(this));
		
		}
			
		if (fSendPaste) {
			port_id port = find_port(OUTPUT_PORT_NAME);

			if (port!=B_NAME_NOT_FOUND)
				write_port( port, 'CtrV', NULL, 0 );

		}
		
		return;
	}
	
	// Limit ändern
	if (msg->what>CLIP_LIMIT_BASE && msg->what <= CLIP_LIMIT_BASE + 100) {
		fMultiClipboard->SetLimit( msg->what - CLIP_LIMIT_BASE );
		return;
	}

	switch (msg->what) {

		case 'Empt': {
				
				fMultiClipboard->Empty();

				be_clipboard->StopWatching(BMessenger(this));
		
				if (be_clipboard->Lock()) {
				
					be_clipboard->Clear();
					be_clipboard->Commit();
					
					be_clipboard->Unlock();
					
				}
				be_clipboard->StartWatching(BMessenger(this));
		
			} break;
		
		case 'Draw': {
				Draw( Bounds() );
			} break;
		
		case B_ABOUT_REQUESTED: {
			
				BAlert *alert = new BAlert( "About ClipUp", 
											"ClipUp V1.1\n"
											"The update for your clipboard.\n\n"
											"ClipUp is freeware.\n"
											"Copyright ©2001 by Werner Freytag\n\n"
											"e-mail:	werner@pecora.de\n"
											"homepage:	http://www.pecora.de",
											"Ok, thank you!");
				alert->Go();
					
			} break;

		case B_QUIT_REQUESTED: {

				BAlert *alert = new BAlert( "", 
					"Do you really want to quit?\n\n"
					"All saved clips will be deleted!",
					"Yes, do it", "No!", NULL, B_WIDTH_AS_USUAL, B_STOP_ALERT);
					
				if (!alert->Go()) {

					fMultiClipboard->Empty();

					BDeskbar deskbar;
					deskbar.RemoveItem(VIEW_NAME);
				}

			} break;

		case B_CLIPBOARD_CHANGED: {
				
				Draw( Bounds() );
				
				SetHighColor( 60, 60, 100, 150 );
				for (int i=0; i<4; ++i)
					StrokeLine( BPoint(4.0, 5.0 + i * 2), BPoint(11.0, 5.0 + i * 2) );

				fMultiClipboard->FetchSystemClipboard();
				
				delete fMessageRunner;
				fMessageRunner = new BMessageRunner(BMessenger(this), 
					new BMessage('Draw'), 100000, 1);

			} break;
		
		default:
			BView::MessageReceived( msg );
	}
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

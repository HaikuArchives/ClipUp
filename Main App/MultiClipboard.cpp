#include "MultiClipboard.h"

#include <Bitmap.h>
#include <Clipboard.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <Font.h>
#include <Message.h>
#include <Mime.h>
#include <String.h>
#include <string.h>

#if __INTEL__
#include <iostream.h>
#endif

#include "App.h"
#include "IconMenuItem.h"
#include "Prefs.h"

MultiClipboard::MultiClipboard()
:	fLimit( DEFAULT_LIMIT )
{

	// FindPath
	find_directory(B_USER_SETTINGS_DIRECTORY, &fSavePath);
	fSavePath.SetTo( fSavePath.Path(), SAVE_FILENAME );
	
	// Clips einlesen
	BFile		file( fSavePath.Path(), B_READ_ONLY );

	BMessage fileMsg;
	if (fileMsg.Unflatten( &file )==B_OK) {

		BMessage restored_clip;
	
		for ( int i=0; (fileMsg.FindMessage("clip", i, &restored_clip)==B_OK); ++i ) {

			BMessage *copy = new BMessage( restored_clip );
			AddItem( copy );
			
			if (i==0 && be_clipboard->Lock()) {		// ersten Clip wieder in's Clipboard schreiben

				be_clipboard->Clear();
			
				BMessage *clip = be_clipboard->Data();
			
				if (clip) {
					*clip = *copy;
					be_clipboard->Commit();
				}
				
				be_clipboard->Unlock();
			}

		}
		
	}
	
	if (!CountItems()) FetchSystemClipboard();
}

MultiClipboard::~MultiClipboard() {

	BMessage fileMsg;
	BMessage *clip;
	
	for (int i=0; i<CountItems();++i) {
		clip = (BMessage *)ItemAt(i);
		fileMsg.AddMessage("clip", clip);
	}
	
	BFile		file( fSavePath.Path(), B_WRITE_ONLY|B_CREATE_FILE );
	file.SetSize( 0 );
	fileMsg.Flatten( &file );

	Empty();

}

void MultiClipboard::FetchSystemClipboard() {

	if (be_clipboard->Lock()) {
	
		BMessage *clip = be_clipboard->Data();
		
		if (clip && !clip->IsEmpty()) {
			BMessage *copy = new BMessage( *clip );

			const ssize_t size = copy->FlattenedSize();

			char *copy_buf = NULL; // Buffer wird erst bei Bedarf reserviert

			int32	foundClipPosition = -1;
			
			for (int i=0; i<CountItems(); ++i) {
				BMessage *compare = (BMessage *)ItemAt(i);
				
				if (size==compare->FlattenedSize()) {
					if (!copy_buf) {
						copy_buf = new char[size];
						copy->Flatten( copy_buf, size );
					}
					char	*compare_buf = new char[size];
					compare->Flatten( compare_buf, size );
					
					if ( memcmp(copy_buf, compare_buf, size)==0 )
						foundClipPosition = i;

					delete compare_buf;
					if (foundClipPosition!=-1) break;
				}
			}
			
			delete copy_buf;

			if ( foundClipPosition!=-1 ) {
				MoveItem( foundClipPosition, 0 );
			}

			else {
				AddItem( copy, 0 );

				while (CountItems()>fLimit)
					delete (BMessage *)RemoveItem( fLimit );
			}

		}

	   be_clipboard->Unlock();
	
	}

}

void MultiClipboard::SetLimit( int32 limit ) {

	fLimit = limit + 1; // Wir haben ja immer noch zusätzlich das normale Clipboard

	if (CountItems()>limit)
		RemoveItems(fLimit, CountItems() - fLimit);

	// Prefs aktualisieren
	prefs.RemoveName("clip_limit");
	prefs.AddInt32("clip_limit", limit );
}

void MultiClipboard::Empty() {

	BMessage *msg;
	while ( (msg = (BMessage *)RemoveItem(0L)) )
		delete msg;
	
}

IconMenuItem *MultiClipboard::CreateMenuItem( int32 index ) {

	BMessage *msg = (BMessage *)ItemAt( index );
	if (!msg) return NULL;

	int32	type_found = false;
	BString	title;
	
	BBitmap	*icon = NULL;
	
	// Ist es eine Klasse?
	if ( msg->what == B_ARCHIVED_OBJECT) {
		
		if (msg->FindString("class", &title)==B_OK) {
			type_found = true;
			if (strcmp("BBitmap", title.String())==0) {
				icon = new BBitmap(BRect(0,0,15,15), B_CMAP8);
				BMimeType("image").GetIcon(icon, B_MINI_ICON);
			}
			else {
				icon = get_resource_bitmap("object");
			}
		}
	}
	
	// ansonsten alle MIMEs rausholen...
	if (!type_found) {
		
		char		*nameFound;
		type_code	typeFound;
		int32 		countFound;
		bool		icon_found = false;
		bool		multi_found = false;

		BString		plain_text;
		BMimeType	first_mime;
		
		icon = new BBitmap(BRect(0,0,15,15), B_CMAP8);

		for (int i=0; ; ++i) {
		
			// Wir gehen davon aus, dass der Programmierer sich für nur einen der Typen entscheidet!
			uint	types[] = { B_MIME_TYPE, B_MESSAGE_TYPE, B_STRING_TYPE };
			for (int j=0; j<(int)(sizeof(types)/sizeof(uint32)); ++j) {
				if ( (msg->GetInfo(types[j], i, &nameFound, &typeFound, &countFound) == B_OK) )
					break;
			}
				
			if (!countFound) break;
			
			BMimeType	mime( nameFound );
			if (!mime.IsValid()) break;
			
			if ( !icon_found && mime.GetIcon(icon, B_MINI_ICON)==B_OK )
				icon_found = true;
			
			if (mime=="text/plain") {

				const char	*data;
				ssize_t	numBytes;
				
				BFont font( be_plain_font );
				font.SetSize( 10.0 );
				
				msg->FindData(nameFound, typeFound, i, (const void **)&data, &numBytes );
				plain_text = "\"";
				plain_text.Append(data, min_c( 200, numBytes ) );
				plain_text.ReplaceAll("\n\r", "·");
				plain_text.ReplaceAll("\n","·");
				plain_text.ReplaceAll("\r", "·");
				plain_text.ReplaceAll("\t", "·");
				font.TruncateString(&plain_text, B_TRUNCATE_END, 150.0 );
				plain_text.Append("\"");
				
/*				const char *text = plain_text.String();
				cout << text << ": ";
				for (int k=0; text[k]; ++k) {
					cout << "[" << i << "]: " << (int)(text[k]) << " ";
				}
				cout << endl;
*/
			}
			
			if (type_found) {
				multi_found = true;
			}
			else {
				char description[B_MIME_TYPE_LENGTH];
				title << ((mime.GetShortDescription(description)==B_OK) ? description : nameFound);

				if (!icon_found) first_mime.SetTo( mime.Type() );
			}
			
			type_found = true;

		}
		
		if (plain_text.Length()) {
			title = plain_text;
		}
		else {
			if (multi_found) title << " [multipart]";
		}
		
		if (!icon_found) {
			BMimeType	supertype;
			first_mime.GetSupertype(&supertype);
			if (supertype.GetIcon(icon, B_MINI_ICON)!=B_OK) {
				first_mime.SetTo("application/octet-stream");
				first_mime.GetIcon(icon, B_MINI_ICON);
			}
		}
	}
	
	// und wenn gar nichts gefunden wird, auf "unknown type" setzen
	if (!type_found) {
		title = "Unknown clip";
		icon = get_resource_bitmap("unknown_clip");
		msg->PrintToStream();
	}
	
	IconMenuItem *item;
	item = new IconMenuItem(title.String(), new BMessage('Clip' + index), icon);
	item->SetTrigger(1);
	return item;
}
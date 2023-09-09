/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "MultiClipboard.h"

#include <Clipboard.h>
#include <Directory.h>
#include <Entry.h>
#include <File.h>
#include <FindDirectory.h>
#include <Message.h>
#include <Roster.h>
#include <String.h>

#include <string>
#include <string.h>

#if DEBUG
#include <iostream>
#endif

#include "Clip.h"
#include "constants.h"
#include "get_resource_bitmap.h"
#include "Prefs.h"

#define	SAVE_FILENAME	"clipup_clips"

MultiClipboard gMultiClipboard;

MultiClipboard::MultiClipboard()
:	fInit(be_clipboard!=NULL)
{

	if (!fInit) return;
	
	// FindPath
	find_directory(B_USER_SETTINGS_DIRECTORY, &fSavePath);
	fSavePath.SetTo( fSavePath.Path(), SAVE_FILENAME );
	
	// Clips einlesen
	BFile		file( fSavePath.Path(), B_READ_ONLY );

	BMessage fileMsg;
	if (fileMsg.Unflatten( &file )==B_OK && fileMsg.what == 'cup2') {

		for ( int i=0; ; ++i ) {

			Clip	*clip = new Clip();
			if (fileMsg.FindFlat("clip", i, clip)!=B_OK) break;
			
			if (clip->IsEmpty())
				delete clip;
			else {
				AddItem( clip );
			}

		}
		
	}

	int32 value;
	fLimit = (prefs.FindInt32("clip_limit", &value)==B_OK) ? value : DEFAULT_LIMIT;

	if (CountItems()) {
		((Clip *)ItemAt(0L))->WriteToClipboard();
		ApplyLimit();
	}
	else
		FetchSystemClipboard( false ); // hierin ist gleich ein ApplyLimit()
	
}

MultiClipboard::~MultiClipboard() {

	if (!fInit) return;

	if (CountItems()) {
		BMessage fileMsg('cup2');
	
		Clip *clip;
	
		for (int i=0; (clip = (Clip *)ItemAt(i)); ++i) {
			
#if DEBUG
			status_t error = fileMsg.AddFlat("clip", (BFlattenable *)clip );
			if (error!=B_OK) std::cerr << "Cannot add flat clip to msg: " << strerror(error) << std::endl;
#else
			fileMsg.AddFlat("clip", (BFlattenable *)clip );
#endif

		}
	
		BFile		file( fSavePath.Path(), B_WRITE_ONLY|B_CREATE_FILE );
		file.SetSize( 0 );
		fileMsg.Flatten( &file );
	}

	MakeEmpty();

}

void MultiClipboard::FetchSystemClipboard( bool getApplicationName ) {
	
	if (!be_clipboard || !be_clipboard->Lock()) return;
	
	BMessage *clip_msg = be_clipboard->Data();
	
	if (!clip_msg || clip_msg->IsEmpty()) return;
	
	Clip	*clip;
	
	if (getApplicationName) {
		app_info info;
		be_roster->GetActiveAppInfo(&info);
		clip = new Clip( clip_msg, info.signature);
	}
	else
		clip = new Clip( clip_msg );

	// Search, if clip already in list
	int32	foundClipPosition = -1;
	for (int i=0; i<CountItems(); ++i) {
		if ( *clip == *(Clip *)ItemAt(i) ) {
			foundClipPosition = i;
			break;
		}
	}
	
	if ( foundClipPosition!=-1 )
		MoveItem( foundClipPosition, 0 );
	else {
		AddItem( clip, 0 );
		ApplyLimit();
	}
	
	be_clipboard->Unlock();
	
}

void MultiClipboard::SetLimit( int32 limit ) {
	fLimit = limit;
	ApplyLimit();
}

void MultiClipboard::ApplyLimit()
{
	int32	count = 0;
	for (int i=1; i<CountItems(); ++i) {
		if (!((Clip *)ItemAt(i))->IsProtected()) {
			if (++count>fLimit) {
				delete (Clip *)RemoveItem( i );
				--i;
			}
		}
	}
}

void MultiClipboard::MakeEmpty() {

	Clip *clip;
	while ( (clip = (Clip *)RemoveItem((int32)0)) )
		delete clip;
	
}

void MultiClipboard::RemoveHistory() {
	
	Clip *clip;
	for (int i=1; i<CountItems(); ) {
		clip = (Clip *)ItemAt(i);
		if (clip->IsProtected()) {
			++i;
		}
		else {
			RemoveItem(clip);
			delete clip;
		}
	}

}

status_t MultiClipboard::ActivateClip( Clip *clip )
{
	ActivateClip( IndexOf( clip ) );
	return B_OK;
}

status_t MultiClipboard::ActivateClip( int32 pos )
{
	if (pos<0 || pos>CountItems() ) return B_ERROR;
	
	MoveItem(pos, 0);
	((Clip*)ItemAt(0))->WriteToClipboard();

	return B_OK;
}

const char *MultiClipboard::GetCurrentBubbleText() const
{
	if (!CountItems()) return "<no clip>";
	
	return ((Clip *)ItemAt( 0L ))->BubbleText();
	
}

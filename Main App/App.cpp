#include "App.h"
#include "DeskbarView.h"

#include <Bitmap.h>
#include <Entry.h>
#include <File.h>
#include <Message.h>
#include <Resources.h>
#include <Roster.h>

#if __INTEL__
#include <iostream.h>
#endif

int main()
{
	DeskbarView::AddToDeskbar();
	return 0;
}

BBitmap *get_resource_bitmap(const char *name) {

	entry_ref	ref;
	if (be_roster->FindApp(APP_SIGNATURE, &ref)!=B_OK) return NULL;

	BFile		file( &ref, B_READ_ONLY );
	BResources	resourcen( &file );

	size_t		groesse;
	BMessage	msg;
	char		*buf = (char *)resourcen.LoadResource('BBMP', name, &groesse);
	
	if (buf) {
		msg.Unflatten(buf);
		return new BBitmap( &msg );
	}

#if __INTEL__

	cerr << "ERROR: Resource not found: " << name << endl;
	
#endif

	return NULL;
}

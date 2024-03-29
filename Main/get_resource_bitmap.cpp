/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "get_resource_bitmap.h"

#include <Bitmap.h>
#include <Entry.h>
#include <File.h>
#include <Message.h>
#include <Resources.h>
#include <Roster.h>

#if DEBUG
#include <iostream>
#endif

#include "constants.h"

BBitmap *get_resource_bitmap(const char *name) {

	entry_ref	ref;
	if (be_roster->FindApp(APP_SIGNATURE, &ref)!=B_OK) return NULL;

	BFile		file( &ref, B_READ_ONLY );
	BResources	resources( &file );

	size_t		groesse;
	BMessage	msg;
	char		*buf = (char *)resources.LoadResource('BBMP', name, &groesse);
	
	if (buf) {
		msg.Unflatten(buf);
		return new BBitmap( &msg );
	}

#if DEBUG
	std::cerr << "ERROR: Resource not found: " << name << std::endl;
#endif

	return NULL;
}

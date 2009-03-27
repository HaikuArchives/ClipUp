#ifndef _PREFS_H
#define _PREFS_H

#include <Path.h>

#define	PREFS_FILENAME	"ClipUp_settings"

class Prefs : public BMessage {

public:
					Prefs();
					~Prefs();

private:

BPath				fSavePath;

};

extern Prefs prefs;
	
#endif
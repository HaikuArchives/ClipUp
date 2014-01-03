#ifndef _APP_H
#define _APP_H

#include <Application.h>

#define APP_SIGNATURE "application/x-vnd.pecora-clipup"

class BBitmap;

BBitmap *get_resource_bitmap(const char *name);

#endif
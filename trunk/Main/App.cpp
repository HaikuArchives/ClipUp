/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#if DEBUG
#include <iostream>
#endif

#include <Application.h>

#include "DeskbarView.h"
#include "constants.h"

int main()
{
	BApplication app(APP_SIGNATURE);
	DeskbarView::AddToDeskbar();
	return 0;
}

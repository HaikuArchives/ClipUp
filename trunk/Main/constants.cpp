/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "constants.h"

const rgb_color	normal_color = { 216, 216, 216, 255 };
const rgb_color	highlight_color = { 180, 185, 204, 255 };
const int kEffectSteps = 15;

#define NOGERMAN

#ifdef GERMAN

// General strings
const char *kTxtEmptyHistory		= "'Clip History' entleeren";
const char *kTxtLimitHistoryListTo	= "'Clip History' limitieren auf:";
const char *kTxtQuit				= "ClipUp beenden";

// arrowed view titles
const char *kTxtCurrentClip			= "Aktuelles Clipboard";
const char *kTxtClipHistory			= "Clip history";
const char *kTxtProtectedClips		= "Geschützte Clips";
const char *kTxtPreferences			= "Einstellungen";
const char *kTxtApplication			= "Programm";

// quit question
const char *kQuitQuestion			= "Wollen Sie wirklich ClipUp beenden?\n\n"
									  "Alle nicht-geschützten Clips werden gelöscht!";
const char *kQuitYes				= "Ja, tu's";
const char *kQuitNo					= "Nein!";

#else

// General strings
const char *kTxtEmptyHistory		= "Empty 'clip history'";
const char *kTxtLimitHistoryListTo	= "Limit history list to:";
const char *kTxtQuit				= "Quit ClipUp";

// arrowed view titles
const char *kTxtCurrentClip			= "Current clipboard";
const char *kTxtClipHistory			= "Clip history";
const char *kTxtProtectedClips		= "Protected clips";
const char *kTxtPreferences			= "Preferences";
const char *kTxtApplication			= "Application";

// quit question
const char *kTxtQuitQuestion		= "Do you really want to quit?\n\n"
									  "All non-protected clips will be deleted!";
const char *kTxtQuitYes				= "Yes, do it";
const char *kTxtQuitNo				= "No!";


#endif

// version information
const char *kTxtVersionStatus[]		= { "Development",
										"Alpha",
										"Beta",
										"Gamma",
										"Golden master",
										"Final" };

// about view
const char *kAppName				= "ClipUp 2";
const char *kCopyright				= "Copyright ©2002 by Werner Freytag";
const char *kSite					= "http://www.pecora.de/clipup";
const char *kSiteText				= "pecora.de";
const char *kEmail					= "mailto:werner@pecora.de";
const char *kEmailText				= "Werner Freytag";

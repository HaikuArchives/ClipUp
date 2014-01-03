/* This file contains general color definitions */

#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#include "GraphicsDefs.h"

// --- compiler directives ---
#define APP_SIGNATURE	"application/x-vnd.pecora-clipup2"
#define CLIP_LIMIT_BASE	'Clip'
#define DEFAULT_LIMIT	10

// --- constant variables ---
extern const rgb_color normal_color;
extern const rgb_color highlight_color;
extern const int kEffectSteps;

// General strings
extern const char *kTxtEmptyHistory;
extern const char *kTxtLimitHistoryListTo;
extern const char *kTxtQuit;

// arrowed view titles
extern const char *kTxtCurrentClip;
extern const char *kTxtClipHistory;
extern const char *kTxtProtectedClips;
extern const char *kTxtPreferences;
extern const char *kTxtApplication;

// version information
extern const char *kTxtVersionStatus[];

// quit question
extern const char *kTxtQuitQuestion;
extern const char *kTxtQuitYes;
extern const char *kTxtQuitNo;

// about view
extern const char *kAppName;
extern const char *kCopyright;
extern const char *kSite;
extern const char *kSiteText;
extern const char *kEmail;
extern const char *kEmailText;

#endif
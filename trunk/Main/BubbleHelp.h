#ifndef _BUBBLEHELPER_H
#define _BUBBLEHELPER_H

#include <OS.h>

class BubbleHelp {

friend class BubbleView;
	
public:
					BubbleHelp();
virtual 			~BubbleHelp();

void				SetHelp(BView *view, const char *text);

private:

bool				IsKeyDown();

thread_id			helperthread;
BList				*helplist;
BWindow				*textwin;
BTextView			*textview;
BView				*gfxview;
void				Helper();
void				DisplayHelp(const char *text, BPoint where);
void				OpenHelpWindow( BView *view, BPoint where );
char				*GetHelp(BView *view);
static long 		_helper(void *arg);
BView				*FindView(BPoint where);
		
void				HideBubble();
		
static long			runcount;
static long			fShowNow;
};

extern	BubbleHelp gBubbleHelp;

#endif

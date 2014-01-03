#ifndef _BUBBLEHELPER_H
#define _BUBBLEHELPER_H

#include <OS.h>

class BubbleHelp {
	friend class BubbleView;
	
	public:
					BubbleHelp();
		virtual 	~BubbleHelp();

		void		SetHelp(BView *view, char *text);
		void		EnableHelp(bool enable=true);

	private:
		BBitmap		*HG;
		BTextView	*textview;
		thread_id	helperthread;
		BList		*helplist;
		BWindow		*textwin;
		BView		*gfxview;
		void		DisplayHelp(char *text,BPoint where);
		void		Helper();
		char		*GetHelp(BView *view);
		static long _helper(void *arg);
		BView		*FindView(BPoint where);
		bool		enabled;
		
		void HideBubble();
		
		static long runcount;
};

extern	BubbleHelp gBubbleHelp;

#endif

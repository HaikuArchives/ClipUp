#include "BubbleHelp.h"

#include <Application.h>
#include <Bitmap.h>
#include <malloc.h>
#include <Region.h>
#include <Screen.h>
#include <String.h>
#include <string.h>
#include <TextView.h>
#include <Window.h>

#if DEBUG
#include <iostream.h>
#endif

// global initialisieren
BubbleHelp	gBubbleHelp;

long BubbleHelp::runcount = 0;

struct helppair
{
	BView	*view;
	char	*text;
};

class BubbleView : public BView {
	public:
				BubbleView(BRect frame, BubbleHelp *parent);
	private:
		BubbleHelp	*fParent;
};

BubbleView::BubbleView(BRect frame, BubbleHelp *parent)
	:	BView(frame, "", B_FOLLOW_ALL, B_WILL_DRAW)
{
	SetViewColor(216, 216, 216);
	fParent = parent;
};

BubbleHelp::BubbleHelp()
{
	// You only need one instance per application.
	if(atomic_add(&runcount,1) == 0)
	{
		helplist = new BList(30);
		helperthread = spawn_thread(_helper, "watcher", B_NORMAL_PRIORITY, this);
		if (helperthread >= 0)
			resume_thread(helperthread);
	}
	else
	{
		// Since you shouldn't be creating more than one instance
		// you may want to jump straight into the debugger here.
		debugger("only one BubbleHelp instance allowed/necessary");
	}
}

BubbleHelp::~BubbleHelp()
{

	if (helperthread >= 0) {
	
		// force helper thread into a known state
		bool locked = textwin->Lock();
		// Be rude...
		kill_thread(helperthread);
		// dispose of window
		if(locked)
		{
			textwin->PostMessage(B_QUIT_REQUESTED);
			textwin->Unlock();
		}
	}
	if (helplist)
	{
		helppair *pair;
		int i = helplist->CountItems()-1;
		while (i >= 0)
		{
			pair = (helppair*)helplist->RemoveItem(i);
			if(pair && pair->text)
				free(pair->text);
			delete pair;
			i--;
		}
		delete helplist;
	}
	atomic_add(&runcount,-1);
}

void BubbleHelp::SetHelp(BView *view, const char *text)
{
	
	if (this && view)
	{
		// delete previous text for this view, if any
		for(int i = 0;;i++)
		{
			helppair *pair;
			pair = (helppair*)helplist->ItemAt(i);
			if(!pair)
				break;
			if(pair->view == view)
			{
				helplist->RemoveItem(pair);
				free(pair->text);
				delete pair;
				break;
			}
		}

		// add new text, if any
		if (text) {
			helppair *pair = new helppair;
			pair->view = view;
			pair->text = strdup(text);
			helplist->AddItem(pair);
		}
	}
}


bool BubbleHelp::IsKeyDown()
{
	key_info keyInfo;
	get_key_info(&keyInfo);
		
	bool keydown = keyInfo.modifiers & ~(B_CAPS_LOCK|B_NUM_LOCK|B_SCROLL_LOCK);
	
	for (int i=0; (i<16) && (!keydown); ++i) {
		
		switch (i) {
			case 1: keydown |= keyInfo.key_states[i] & ~1; break;
			case 4:	keydown |= keyInfo.key_states[i] & ~32; break;
			case 7: keydown |= keyInfo.key_states[i] & ~16; break;
			default:
				keydown |= keyInfo.key_states[i];
		}
	}
	
	return keydown;
}

void BubbleHelp::OpenHelpWindow( BView *view, BPoint where ) {

	char *text = NULL;

	while (view && (text = GetHelp(view)) == NULL)
		view = view->Parent();

	if (!text) return;

	DisplayHelp(text,where);

	// wait until mouse moves out of view, or wait
	// for timeout
	ulong buttons;

	if (!textwin->Lock()) return;

	gfxview->GetMouse(&where,&buttons);
	gfxview->ConvertToScreen(&where);

	textwin->Unlock();
		
	long displaycounter = 0;
	BPoint where2;

	long displaytime = max_c(20,strlen(text));
	
	do {
		snooze(100000);

		if (!textwin->Lock()) return;

		gfxview->GetMouse(&where2,&buttons);
		gfxview->ConvertToScreen(&where2);

		textwin->Unlock();
		
	} while (!IsKeyDown() && !buttons && where2 == where && (displaycounter++<displaytime));

	textwin->Hide();

	do {

		snooze(100000);

		if (!textwin->Lock()) return;

		gfxview->GetMouse(&where2,&buttons);
		gfxview->ConvertToScreen(&where2);

		textwin->Unlock();

	} while (where2 == where);

}

char *BubbleHelp::GetHelp(BView *view)
{
	int i = 0;
	helppair *pair;
	
	// This could be sped up by sorting the list and
	// doing a binary search.
	// Right now this is left as an exercise for the
	// reader, or should I say "third party opportunity"?
	while((pair = (helppair*)helplist->ItemAt(i++)) != NULL)
	{
		if(pair->view == view)
			return pair->text;
	}
	return NULL;
}


long BubbleHelp::_helper(void *arg)
{
	((BubbleHelp*)arg)->Helper();
	return 0;
}

void BubbleHelp::Helper()
{
	// Wait until the BApplication becomes valid, in case
	// someone creates this as a global variable.
	while(!be_app_messenger.IsValid()) snooze(200000);
	
	textwin = new BWindow(BRect(0,0,10,10),"",B_NO_BORDER_WINDOW_LOOK, B_FLOATING_ALL_WINDOW_FEEL,
				B_NOT_MOVABLE|B_AVOID_FOCUS);

	textview = new BTextView(BRect(0,0,50,50),"",BRect(2,2,48,48),B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
	textview->SetWordWrap(false);

	gfxview = new BubbleView(textwin->Bounds(), this);
	gfxview->SetViewColor( 255, 160, 160 );

	textwin->AddChild(gfxview);
	
	textwin->Run();
	textwin->Lock();
	textwin->Activate(false);
	rename_thread(textwin->Thread(),"");
	textwin->Unlock();

	ulong delaycounter = 0;
	BPoint lastwhere;

	while(be_app_messenger.IsValid()) {

		if (textwin->Lock()) {
		
			BPoint where;
			ulong buttons;

			gfxview->GetMouse(&where,&buttons);
			gfxview->ConvertToScreen(&where);
			textwin->Unlock();

			if ( lastwhere != where || buttons) {
				delaycounter = 0;
			}
			else {
				// mouse didn't move
				if (delaycounter++>5) {
				
					delaycounter = 0;

					// mouse didn't move for a while
					OpenHelpWindow( FindView(where), where );
					
				}
			}
			lastwhere = where;
		}
		snooze(100000);
	}
	// (this thread normally gets killed by the destructor before arriving here)
}

BView *BubbleHelp::FindView(BPoint where)
{
	BView *winview = NULL;
	BWindow *win;
	long windex = 0;
	while((winview == NULL)&&((win = be_app->WindowAt(windex++)) != NULL))
	{
		if(win != textwin)
		{
			// lock with timeout, in case somebody has a non-running window around
			// in their app.
			if(win->LockWithTimeout(1E6) == B_OK)
			{
				BRect frame = win->Frame();
				if(frame.Contains(where))
				{
					BPoint winpoint;
					winpoint = where-frame.LeftTop();
					winview = win->FindView(winpoint);
					if(winview) {
						BRegion region;
						BPoint newpoint = where;
						winview->ConvertFromScreen(&newpoint);
						winview->GetClippingRegion(&region);
						if(!region.Contains(newpoint))
							winview = 0;
					}
				}
				win->Unlock();
			}
		}
	}
	return winview;
}

void BubbleHelp::DisplayHelp(const char *text, BPoint where) {

	textview->SetText(text);

	float width = 0;
	int numlines = textview->CountLines();
	int linewidth;
	for (int i = 0; i<numlines; i++)
		if ((linewidth = (int)textview->LineWidth(i))>width)
			width = linewidth;
	width += 10;

	font_height	fontheight;
	be_plain_font->GetHeight(&fontheight);

	float height = (fontheight.ascent + fontheight.descent + fontheight.leading) * numlines + 6;

	textwin->ResizeTo(width,height);
										 
	BScreen	screen(textwin);

	BPoint dest = where+BPoint(10,16);
	BRect screenframe = screen.Frame();
	if((dest.y+height)>(screenframe.bottom-3))
		dest.y = dest.y-(16+height+8);

	if((dest.x+width)>(screenframe.right))
		dest.x = dest.x-((dest.x+width)-screenframe.right);

	textwin->MoveTo(dest);
	textwin->SetWorkspaces(B_CURRENT_WORKSPACE);

	BRect	rect = BRect(textwin->Frame());
	rect.bottom++;
	
	BBitmap *bgbitmap = new BBitmap( rect.OffsetToCopy(0,0), screen.ColorSpace(), true );

	screen.ReadBitmap(bgbitmap, false, &rect);
	
	BView	*bgview = new BView( rect.OffsetToCopy(0,0), NULL, 0, 0);
	bgbitmap->Lock();
	bgbitmap->AddChild(bgview);

	BRect frame = BRect(0, 0, rect.Width()-2, rect.Height()-2);
		
	bgview->SetDrawingMode(B_OP_ALPHA);

	bgview->SetHighColor(0, 0, 0, 20);
	frame.OffsetBy(1,1);
	bgview->FillRoundRect( frame, 5, 5 );

	frame.OffsetBy(-1,-1);

	bgview->SetHighColor(255, 255, 255, 210);
	bgview->FillRoundRect(frame, 4, 3);

	bgview->SetHighColor(255, 255, 255, 220);
	bgview->StrokeRoundRect(frame.InsetByCopy(0.5, 0.5).OffsetToCopy(1.0, 1.0), 4, 3);

	bgview->SetHighColor(0, 0, 0, 150);
	bgview->StrokeRoundRect(frame, 4, 3);
	
	bgview->SetHighColor(0, 0, 0);

	float line_height = fontheight.ascent + fontheight.descent + fontheight.leading;

	BString Text = textview->Text();
	Text.ReplaceAll("\t", "   ");
	
	int start = 0, ende;
	for (int i = 0; i<numlines; i++) {

		ende = Text.FindFirst("\n", start); if (ende<0) ende = Text.Length();
		char* text = new char[ende - start + 1];
		Text.CopyInto(text, start, ende - start);
		text[ende - start] = 0;
		
		bgview->DrawString( text, BPoint(5, 2 + fontheight.ascent + i * line_height) );
		start = ende + 1;
	}

	bgbitmap->RemoveChild( bgview );
	bgbitmap->Unlock();

	textwin->Lock();

	gfxview->SetViewBitmap( bgbitmap );
	gfxview->Invalidate();
	delete bgbitmap;

	if (textwin->IsHidden()) {
		textwin->Show();
	}

	textwin->Unlock();
}

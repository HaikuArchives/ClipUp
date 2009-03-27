/*
 * Copyrights (c):
 *     2001 - 2008 , Werner Freytag.
 *     2009, Haiku
 * Distributed under the terms of the MIT License.
 *
 * Original Author:
 *              Werner Freytag <freytag@gmx.de>
 */

#include "PopUpWindow.h"

#include <Control.h>
#include <Screen.h>

#if DEBUG
#include <iostream>
#endif
#include <math.h>
#if DEBUG
#include <stl.h>
#endif

#include "ApplicationView.h"
#include "ArrowedView.h"
#include "constants.h"
#include "ClipListView.h"
#include "ClipView.h"
#include "DeskbarView.h"
#include "MainView.h"
#include "MultiClipboard.h"
#include "SettingsView.h"

// This is initialized in the DeskbarView because we need a vaild BApplication!
PopUpWindow *gPopUpWindow = NULL;

PopUpWindow::PopUpWindow()
:	BWindow( BRect(0, 0, 50, 50).OffsetToCopy( BPoint( 100, 50 ) ), "ClipUp",
		B_BORDERED_WINDOW_LOOK, B_FLOATING_ALL_WINDOW_FEEL,
		B_NOT_ANCHORED_ON_ACTIVATE|B_ASYNCHRONOUS_CONTROLS|B_FULL_UPDATE_ON_RESIZE),
	fActiveApp( 0 )
{
	
	SetSizeLimits( 0, 10000, 0, 10000 );
	ResizeTo( 50, 3);

	fMainView = new MainView();
	AddChild( fMainView );

	ArrowedView *view;
	BView *child;
	float width, height;
	
	view = new ClipListView( CLV_CURRENT_CLIP, BRect( 1, 1, Frame().Width() - 1, 10 ), "a_current", kTxtCurrentClip );
	fViews.push_back( view );

	view = new ClipListView( CLV_UNPROTECTED_CLIPS, BRect( 1, 1, Frame().Width() - 1, 10 ), "a_history", kTxtClipHistory );
	fViews.push_back( view );
	
	view = new ClipListView( CLV_PROTECTED_CLIPS, BRect( 1, 1, Frame().Width() - 1, 10 ), "a_protected", kTxtProtectedClips );
	fViews.push_back( view );
	
	// settings view
	view = new ArrowedView( BRect( 1, 1, Frame().Width() - 1, 10 ), "a_prefs", kTxtPreferences );

	child = new SettingsView( BRect( 0, 0, view->Frame().Width(), 144 ).OffsetByCopy(0, 14) );
	view->AddChild( child );
	child->GetPreferredSize(&width, &height);

	view->SetSizeLimit( width, height );
	fViews.push_back( view );
	
	// application view
	view = new ArrowedView( BRect( 1, 1, Frame().Width() - 1, 10 ), "a_app", kTxtApplication );

	child = new ApplicationView( BRect( 0, 0, view->Frame().Width(), 144 ).OffsetByCopy(0, 14) );
	view->AddChild( child );
	child->GetPreferredSize(&width, &height);
	
	view->SetSizeLimit( width, height );
	fViews.push_back( view );
	
}

void PopUpWindow::WindowActivated(bool state) {
	
	if (!state) Hide();
	
	// Kind of AcceptFirstClick for title tab
	if (state) {

		BView *titleView = FindView("_mainview");
		
		BPoint location;
		uint32 buttons;
		
		titleView->GetMouse( &location, &buttons, false );

		if (titleView->Bounds().Contains( location ) )
			titleView->MouseDown( location );
		
	}
}

void PopUpWindow::MessageReceived(BMessage *msg) {

	switch (msg->what) {

		// an ArrowedView requires a size change
		case 'Arro':
			ResizeToPreferredSize();
		break;
		
		// message from a clipview
		case 'clip':
		case 'prot': {
			
			ClipView *source = NULL;
			if ( msg->FindPointer("source", (void **)&source)==B_OK ) {
				
				if (msg->what=='clip') {
					gMultiClipboard.ActivateClip( source->SourceClip() );
					Hide();
					gDeskbarView->UpdateClip();
				}
				else {
					int32 index = gMultiClipboard.IndexOf(source->SourceClip());
					if (index>1) gMultiClipboard.MoveItem(index, 1);
					RemoveArrowedViews();
					AddArrowedViews( true );
				}
			}
		} break;
		
		case B_KEY_DOWN: {

			BView		*view = CurrentFocus();
			ClipView	*clipview = dynamic_cast<ClipView *>( view );
			
			switch (msg->FindInt32("raw_char")) {

				case B_ESCAPE:
					Hide();
				break;
				
				case B_DELETE:
					if (clipview) {
						Clip *clip = clipview->SourceClip();
						bool is_first_clip = gMultiClipboard.IndexOf(clip)==0;
						
						RemoveArrowedViews();
						gMultiClipboard.RemoveItem( clip );
						delete clip;
						AddArrowedViews( true );
						
						if (is_first_clip) {
							clip = (Clip *)gMultiClipboard.ItemAt(0);
							gMultiClipboard.ActivateClip( clip );
						}
					}
				break;
				
				case B_DOWN_ARROW: {
					
					if (!clipview) {
						if (fViews[0]->CountChildren()>1)
							fViews[0]->ChildAt(1)->MakeFocus();
					}
					else {
						BView *nextview = clipview->NextSibling();
						if (nextview) {
							nextview->MakeFocus();
						}
						else {
							ClipListView *clv = (ClipListView *)clipview->Parent();
							
							for (int i=0; i<3; ++i) {
								if ( fViews[i] == clv ) {
									do {
										if (++i>=3) i = 0;
									} while (fViews[i]->CountChildren()==1);
									
									fViews[i]->ChildAt(1)->MakeFocus();
									break;
								}
							}
							
						}
					}
				} break;

				case B_UP_ARROW: {
					
					if (!clipview) {
						for (int i=2; i>=0; --i) {
							if (fViews[i]->CountChildren()>1) {
								fViews[i]->ChildAt( fViews[i]->CountChildren()-1 )->MakeFocus();
								break;
							}
						}
								
					}
					else {
						ClipListView *clv = (ClipListView *)clipview->Parent();
						
						// find index
						int32 index;
						for (index=1; index<clv->CountChildren(); ++index)
							if (clv->ChildAt(index)==clipview) break;
							
						if (index>1) {
							BView *prevview = clipview->PreviousSibling();
							prevview->MakeFocus();
						}
						else {
							
							for (int i=0; i<3; ++i) {
								if ( fViews[i] == clv ) {
									do {
										if (--i<0) i = 2;
									} while (fViews[i]->CountChildren()==1);
									
									fViews[i]->ChildAt( fViews[i]->CountChildren()-1 )->MakeFocus();
									break;
								}
							}
							
						}
					}
				} break;
			}
		} break;
		
		case 'Quit':
			gDeskbarView->Looper()->PostMessage(B_QUIT_REQUESTED, gDeskbarView);
		break;
		
		case 'Empt': {
			RemoveArrowedViews();
			gMultiClipboard.RemoveHistory();
			AddArrowedViews( true );
		} break;
		
		case 'Limt': {

			int32 oldLimit = gMultiClipboard.Limit();

			int32 limit;
			if (msg->FindInt32("be:value", &limit)==B_OK)
				gMultiClipboard.SetLimit(limit);
			
			if (oldLimit>limit) {
				RemoveArrowedViews();
				AddArrowedViews( true );
			}
		} break;
		
		default:
#if DEBUG
			msg->PrintToStream();
#endif
			BWindow::MessageReceived( msg );
	}
}

void PopUpWindow::Show()
{
	
	if (!IsHidden()) return;
	
	fMouseMode = mouse_mode();
	set_mouse_mode(B_NORMAL_MOUSE);

	app_info info;
	be_roster->GetActiveAppInfo(&info);
	fActiveApp = info.team;

	AddArrowedViews( false );
	ResizeToPreferredSize( false );
	
	float	width = Frame().Width();
	float	height = Frame().Height();

	MoveTo( Frame().left + width / 2, Frame().top + height / 2 );
	ResizeTo( 0, 0 );

	SetWorkspaces(B_CURRENT_WORKSPACE);
	BWindow::Show();
	
	SizeEffect( width, height );
	
	// activate the deskbar - otherwise the current app will be deactivated when this window is closed!
	be_roster->GetAppInfo("application/x-vnd.Be-TSKB", &info);
	be_roster->ActivateApp(info.team);
}

void PopUpWindow::Hide()
{
	
	if (IsHidden()) return;
	
	be_roster->ActivateApp(fActiveApp);
		
	BRect frame( Frame() );
	
	SizeEffect( 0, 0 );
	BWindow::Hide();
	
	set_mouse_mode(fMouseMode);

	MoveTo(frame.LeftTop());
	ResizeTo(frame.Width(), frame.Height());

	RemoveArrowedViews();
}

void PopUpWindow::RemoveArrowedViews()
{
	for (vector<ArrowedView *>::iterator i = fViews.begin(); i != fViews.end(); ++i) {
		fMainView->RemoveChild( *i );

		float width, height;
		(*i)->GetPreferredSize( &width, &height );
	}

}

void PopUpWindow::AddArrowedViews( bool is_update )
{
	float next_top_position = 5;

	for (vector<ArrowedView *>::iterator i = fViews.begin(); i != fViews.end(); ++i) {

		fMainView->AddChild( *i );
		(*i)->SetTarget( this );
		
		if (!is_update) {
			float width, height;
			(*i)->GetPreferredSize( &width, &height );
			
			(*i)->MoveTo( 1, next_top_position );
			(*i)->ResizeTo( Frame().Width()-2, height );
	
			next_top_position += height;
		}
	}

	// Update the views
	if (is_update) ResizeToPreferredSize( true );

}

// sizechange without the views: used in hide() and show()
void PopUpWindow::SizeEffect( float new_width, float new_height )
{
	
	vector<ArrowedView *>::iterator vi;
	
	for ( vi = fViews.begin(); vi!=fViews.end(); ++vi )
		(*vi)->SetEnabled(false);
	
	float	old_width = Frame().Width();
	float	old_height = Frame().Height();
	
	BPoint	new_pos( Frame().LeftTop() - BPoint( (new_width - old_width)/2, (new_height - old_width)/2 ) );

	if (old_width==0 && old_height==0) {
	
		BPoint location;
		uint32 buttons;
		
		fMainView->GetMouse( &location, &buttons, false );
		fMainView->ConvertToScreen( &location );
		
		MoveTo( location);
		
	}
	
	BPoint	old_pos( Frame().LeftTop() );

	BRect	screen_frame = BScreen(this).Frame();
	
	BRect	new_frame = BRect( 0, 0, new_width -1, new_height - 1).OffsetToCopy( new_pos );
	
	new_pos.x = new_frame.left<0 ? 0 : 
					new_frame.right > screen_frame.right ? old_pos.x - new_width :
						new_pos.x;
	
	new_pos.y = new_frame.top<0 ? 0 : 
					new_frame.bottom>screen_frame.bottom ? old_pos.y - new_height :
						new_pos.y;

	float	delta_width = (new_width - old_width) / kEffectSteps;
	float	delta_height = (new_height - old_height) / kEffectSteps;
	float	delta_x = (new_pos.x - old_pos.x) / kEffectSteps;
	float	delta_y = (new_pos.y - old_pos.y) / kEffectSteps;

	for (int i=1; i<=kEffectSteps; ++i) {
	
		ResizeTo( old_width + delta_width * i, old_height + delta_height * i );
		MoveTo( old_pos.x + delta_x * i, old_pos.y + delta_y * i);

		snooze( 4000 );
	}
	
	if (new_width>old_width || new_height>old_height) {
		for ( vi = fViews.begin(); vi!=fViews.end(); ++vi ) {
			(*vi)->SizeChangeComplete();
			(*vi)->SetEnabled(true);
		}
	}
	
}

// sizechange with view resizing
void PopUpWindow::ResizeToPreferredSize( bool with_effect )
{
	vector<ArrowedView *>::iterator vi;
	
	if (with_effect) {
		for ( vi = fViews.begin(); vi!=fViews.end(); ++vi )
			(*vi)->SetEnabled(false);
	}
	
	// Find max. width
	float width, height, maxwidth(0);
	for ( vi = fViews.begin(); vi!=fViews.end(); ++vi )
	{
		(*vi)->GetPreferredSize( &width, &height );
		if (width>maxwidth)		maxwidth = width;
	}
	
	// Resize the views
	const int steps = with_effect ? kEffectSteps / 3 : 1;
	
	for (int i=1; i<=steps; ++i) {

		int deltaY = 0;
		for ( vi = fViews.begin(); vi!=fViews.end(); ++vi ) 
		{
			
			(*vi)->MoveBy( 0, deltaY );
			
			BRect	frame = (*vi)->Frame();
			float	w(0), h(0);
			
			(*vi)->GetPreferredSize( &w, &h );
			
			float delta_h = (h - frame.Height()) / (steps + 1 - i);
			int deltaH = (int)(delta_h + (delta_h > 0 ? .4999 : -.5));
			
			deltaY += deltaH;
			
			(*vi)->ResizeBy( 0, deltaH );

		}
		
		float delta_w = (maxwidth + 2 - Frame().Width()) / (steps + 1 - i);
		int deltaW = (int)(delta_w + (delta_w > 0 ? .5 : -.5) );

		ResizeBy( deltaW, deltaY );
		
		UpdateIfNeeded();

	}
	
	vi = &fViews.back();
	ResizeTo( maxwidth + 2, (*vi)->Frame().bottom - 1);
	
	if (with_effect) {
		for ( vi = fViews.begin(); vi!=fViews.end(); ++vi ) {
			(*vi)->SizeChangeComplete();
			(*vi)->SetEnabled(true);
		}
	}
	
}

void PopUpWindow::GetPreferredSize( float *width, float *height )
{
	vector<ArrowedView *>::iterator vi;
	
	*width = 0;

	// Find max. width
	float w, h;
	for ( vi = fViews.begin(); vi!=fViews.end(); ++vi )
	{
		(*vi)->GetPreferredSize( &w, &h );
		if (w>(*width))		(*width) = w;
	}
	
	*width += 2;
	
	vi = &fViews.back();
	*height = (*vi)->Frame().bottom - 1;
	
}

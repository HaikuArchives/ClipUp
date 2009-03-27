#include "ClipListView.h"

#include <ClassInfo.h>
#if DEBUG
#include <iostream>
#endif

#include "ClipView.h"
#include "MultiClipboard.h"

ClipListView::ClipListView(	clv_type type,
							BRect frame,
							const char *name,
							const char *label )
	:	ArrowedView( frame, name, label ),
		fType( type )
{
}

void ClipListView::AttachedToWindow()
{
	UpdateClips();
	
	ArrowedView::AttachedToWindow();
}

void ClipListView::SetEnabled( bool on ) {
	ClipView *view;
	for (int i=0; (view = (ClipView *)fClipViews.ItemAt( i )); ++i) {
		view->SetEnabled( on && Value() );
	}
}

void ClipListView::SizeChangeComplete()
{
	ClipView *view;
	for (int i=0; (view = (ClipView *)fClipViews.ItemAt( i )); ++i) {
		if (Value()) view->CreatePicture();
		view->SetEnabled( Value() );
	}
}

void ClipListView::AddClip( int i, int pos )
{
	BRect		rect(BRect (0, 0, Frame().Width(), 18 ).OffsetToCopy( 0, 15 + pos * 19 ));
	ClipView 	*clipview = new ClipView( rect, (Clip *)gMultiClipboard.ItemAt(i) );
	
	AddChild( clipview );
	fClipViews.AddItem( clipview );
}

void ClipListView::UpdateClips()
{

	for (int i=0; i<fClipViews.CountItems(); ++i) {
		ClipView *clipview = (ClipView *)fClipViews.ItemAt(i);
		RemoveChild( clipview );
		delete clipview;
	}

	fClipViews.MakeEmpty();
	
	int pos = 0;
	for (int i=0; i<gMultiClipboard.CountItems(); ++i) {
		
		bool addthis = false;
		if ( i==0 ) {
			if ( fType==CLV_CURRENT_CLIP ) addthis = true;
		}
		else {
			 if ( ( fType==CLV_UNPROTECTED_CLIPS && !((Clip *)gMultiClipboard.ItemAt(i))->IsProtected() )
			 	|| ( fType==CLV_PROTECTED_CLIPS && ((Clip *)gMultiClipboard.ItemAt(i))->IsProtected() ) )

			 	addthis = true;
		}
		
		if (addthis) {
			AddClip( i, pos );
			pos++;
		}
		
	}
	
	SetSizeLimit( 220, 14 + pos * 19 );
}
#include <Window.h>

#if DEBUG
#include <iostream>
#endif

#include "RealtimeSlider.h"

RealtimeSlider::RealtimeSlider(BRect frame, const char *name,
			const char *label, BMessage *message, int32 minValue,
			int32 maxValue, thumb_style thumbType,
			uint32 resizingMode, uint32 flags)

:	BSlider( frame, name, label, message, minValue, maxValue, thumbType,
		resizingMode, flags ),
	fMouseDown( false ),
	fDefaultValue( minValue )
{
}		

void RealtimeSlider::MouseMoved(BPoint pt, uint32 c, const BMessage *m) {
	BSlider::MouseMoved( pt, c, m );
	if (fMouseDown)
		Invoke();
}

void RealtimeSlider::MouseDown(BPoint pt) {
	
	SetMouseEventMask(B_POINTER_EVENTS, B_LOCK_WINDOW_FOCUS|B_SUSPEND_VIEW_FOCUS);

	BMessage	*message = Window()->CurrentMessage();
	int32		buttons = 0;
	message->FindInt32("buttons", &buttons);
	if (buttons & B_SECONDARY_MOUSE_BUTTON) {
		SetValue( fDefaultValue );
		Invoke();
	}
	else {
		fMouseDown = true;
		BSlider::MouseDown( pt );
		Invoke();
	}
}

void RealtimeSlider::MouseUp(BPoint pt) {
	fMouseDown = false;
	Invoke();
	BSlider::MouseUp( pt );
}

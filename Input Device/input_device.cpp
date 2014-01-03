#include "input_device.h"

#include <InterfaceDefs.h>
#include <stdlib.h>

thread_id	ClipUpInputDevice::fThread = B_ERROR;

BInputServerDevice* instantiate_input_device() {
	return (new ClipUpInputDevice());
}

ClipUpInputDevice::ClipUpInputDevice()
	:	BInputServerDevice()
{
	input_device_ref	clipupDevice			= { "ClipUp Input Device",
													B_KEYBOARD_DEVICE,
													NULL };
	input_device_ref	*clipupDeviceList[2]	= {&clipupDevice, NULL};

	RegisterDevices(clipupDeviceList);	
}

ClipUpInputDevice::~ClipUpInputDevice()
{
	// cleanup
}

status_t ClipUpInputDevice::InitCheck()
{
	// do any init code that could fail here
	// you will be unloaded if you return false

	return (BInputServerDevice::InitCheck());
}

status_t ClipUpInputDevice::SystemShuttingDown() {
	// do any cleanup (ie. saving a settings file) when the
	// system is about to shut down

	return (BInputServerDevice::SystemShuttingDown());
}


status_t ClipUpInputDevice::Start( const char *device, void	*cookie) {

	// start generating events
	// this is a hook function, it is called for you
	// (you should not call it yourself)

	fThread = spawn_thread(listener, device, B_LOW_PRIORITY, this);

	resume_thread(fThread);

	return B_NO_ERROR;
}

status_t ClipUpInputDevice::Stop( const char *device, void *cookie) {

	// stop generating events
	// this is a hook function, it is called for you
	// (you should not call it yourself)

	status_t err = B_OK;

	wait_for_thread(fThread, &err);
	fThread = B_ERROR;

	return B_NO_ERROR;
}

status_t ClipUpInputDevice::Control( const char	*device, void *cookie,
	uint32 code, BMessage *message) {

	return B_NO_ERROR;
}


int32 ClipUpInputDevice::listener( void *arg) {

	port_id port = create_port(20, "ClipUp output port");

	ClipUpInputDevice *clipupDevice = (ClipUpInputDevice *)arg;
	
	int32 code;
	while (read_port(port, &code, NULL, 0)==B_OK) {

		BMessage *event = new BMessage(B_KEY_DOWN);
		event->AddInt64("when", system_time() );
		event->AddInt32("raw_char", 118);
		event->AddInt32("modifiers", B_COMMAND_KEY);
		event->AddInt8("byte", 'v');
		event->AddInt8("byte", 0);
		event->AddInt8("byte", 0);
//		event->AddInt32("raw_char", 'v');
//		event->AddString("bytes", "v");
		
		clipupDevice->EnqueueMessage(event);

		snooze(100000);
	}

	delete_port( port );
	
	return (B_OK);
}

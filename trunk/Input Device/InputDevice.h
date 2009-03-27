#ifndef _CLIPUP_INPUT_DEVICE_H
#define _CLIPUP_INPUT_DEVICE_H

#include <InputServerDevice.h>
#include <List.h>
#include <OS.h>
#include <SupportDefs.h>


// export this for the input_server
extern "C" _EXPORT BInputServerDevice* instantiate_input_device();
 

class ClipUpInputDevice : public BInputServerDevice {
public:
							ClipUpInputDevice();
	virtual					~ClipUpInputDevice();

	virtual status_t		InitCheck();
	virtual status_t		SystemShuttingDown();

	virtual status_t		Start(const char *device, void *cookie);
	virtual	status_t		Stop(const char *device, void *cookie);

	virtual status_t		Control(const char	*device,
									void		*cookie,
									uint32		code, 
									BMessage	*message);

private:
	static int32			listener(void *arg);
	
	static thread_id		fThread;
};


#endif
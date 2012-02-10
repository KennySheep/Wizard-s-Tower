#ifndef BASE_FRAME_LISTENER_H
#define BASE_FRAME_LISTENER_H

#include "OgreFrameListener.h"
#include "CameraApp.h"

#include <OIS.h>

using namespace Ogre;
using namespace OIS;

class BaseFrameListener : public FrameListener {

public:
	// ctor/dtor
	BaseFrameListener(OgreApp *ap) { 
		
		keepRendering = true;
		
		tm = 0.0f;
		app = ap;
	}
	virtual ~BaseFrameListener() {}

	// We will provide some meat to this method override
	virtual bool frameStarted(const FrameEvent &evt);

	// This little puppy will process the input events
	virtual bool frameEnded(const FrameEvent &evt);

	// handle the keyboard input
	virtual void processKeyboardInput(const FrameEvent &evt);

	// handle the mouse input
	virtual void processMouseInput();

	// call this to terminate the application
	void terminateApp();

private:
	bool keepRendering;
	float tm;
	OgreApp *app;
};

#endif
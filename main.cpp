/****************************************************************************/
/* This app demonstrates how to create a first person camera by attaching   */
/* both the camera and the character to the same scene node.  The camera is */
/* positioned to be just behind the charater using the setPosition() on the */
/* camera.  When the scene node to which the camera and character are       */
/* attached is moved or rotated, both the camera and the character do the   */
/* same thing so that they maintain their relative positions and we have a  */
/* first person camera.  You can move and rotate the node holding the       */
/* camera and character using the following keys:                           */
/* pgup - forward                                                           */
/* pgdn - backward                                                          */
/* right arrow - right                                                      */
/* left arrow - left                                                        */
/* up arrow - up                                                            */
/* down arrow - down                                                        */
/* l - rotate left                                                          */
/* r - rotate right                                                         */
/****************************************************************************/

#include "CameraApp.h"

#if defined(WIN32)
#	include <windows.h>
#endif


#if defined (WIN32)
INT WINAPI WinMain (
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
#else
int main (int argc, char *argv[])
#endif
{
	CameraApp *app = 0;
	try {
		app = new CameraApp(NULL, true);
		app->constructObject();
		app->getRoot()->startRendering();
	}
	catch (Ogre::Exception &e) {
		//ErrorDialog *ed = PlatformManager::getSingleton().createErrorDialog();
		//ed->display(e.getFullDescription());
		Ogre::LogManager::getSingleton().logMessage(e.getFullDescription());
	}

	if(app) 
	{
		app->shutdown();
		delete app;
	}
	return 0;
}
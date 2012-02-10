#include "OgreApp.h"

OgreApp* OgreApp::theRunningApp = NULL;


int OgreApp::initializeRoot()
{
	root = new Ogre::Root();

	// try first to restore an existing config
	if (!root->restoreConfig()) {

		// if no existing config, or could not restore it, show the config dialog
		if (!root->showConfigDialog()) {

			// if the user pressed Cancel, clean up and exit
			delete root;
			return 0;
		}
	}
	return 1;
}

void OgreApp::createSceneManager()
{
	// get a pointer to the default base scene manager -- sufficient for our purposes
	sceneMgr = root->createSceneManager(Ogre::ST_GENERIC);
}

void OgreApp::setupResources(void)
{

    // Load resource paths from config file
	Ogre::ConfigFile cf;
    cf.load("resources.cfg");

    // Go through all sections & settings in the file
    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();

	Ogre::String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void OgreApp::createFrameListener()
{
	Ogre::LogManager::getSingletonPtr()->logMessage("+++ NULL frameListener in OgreApp");	
}

void OgreApp::createScene()
{
	Ogre::LogManager::getSingletonPtr()->logMessage("+++ NULL scene in OgreApp");	
}

/**
* Initialize the OIS input ststem.
*/
void OgreApp::initializeOIS(Ogre::RenderWindow *win)
{
	OIS::ParamList pl;														// name/value pair map to pass params
	size_t windowHnd = 0;													// to hold window handle
	std::ostringstream windowHndStr;										// string to hold numbers converted to strings
	win->getCustomAttribute("WINDOW", &windowHnd);							// get window handle from render win
	windowHndStr << windowHnd;												// convert handle to a string
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));	// add win handle to param list
	inputManager = OIS::InputManager::createInputSystem( pl );				// create OIS input system		

	// Now, let's create a mouse & keyboard object
	mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, false));
	keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, false));

	setMouseMetrics(win);													// tell mouse the window size
	mouse->setEventCallback(this);
    keyboard->setEventCallback(this);
}

void OgreApp::setMouseMetrics(Ogre::RenderWindow *win)
{
	unsigned int width, height, depth;
	int top, left;
	win->getMetrics(width, height, depth, left, top);
	const OIS::MouseState &ms = mouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

void OgreApp::shutdownOIS()
{
	// destroy it all!!
	if(inputManager)
	{
		inputManager->destroyInputObject(mouse);
		inputManager->destroyInputObject(keyboard);
		OIS::InputManager::destroyInputSystem(inputManager);
		inputManager = 0;
	}
}

void OgreApp::shutdown()
{
	if(oisEnabled) shutdownOIS();

	if(sceneMgr)
	{
		sceneMgr->getRootSceneNode()->detachAllObjects();
		sceneMgr->destroyAllEntities();
		sceneMgr->destroyAllCameras();
	}
	if(root) 
	{
		if(sceneMgr)root->destroySceneManager(sceneMgr);
		sceneMgr = NULL;
		if(frameListener) root->removeFrameListener(frameListener);
		delete root;
		root = NULL;
	}
	

}


void OgreApp::constructObject()
{
	if(! initializeRoot())
	{
		// user cancelled config, so leave
		return;
	}
	Ogre::LogManager::getSingletonPtr()->logMessage("+++ root configured");

	// initialize Root -- have it create a render window for us
	root->initialise(true);
	Ogre::LogManager::getSingletonPtr()->logMessage("+++ root initialized");
	// get a pointer to the auto-created window
	window = root->getAutoCreatedWindow();
	Ogre::LogManager::getSingletonPtr()->logMessage("+++ render window created");


	window->setFullscreen(false, 500, 500);
	if(oisEnabled)
	{
		initializeOIS(window);
		Ogre::LogManager::getSingletonPtr()->logMessage("+++ OIS initialized");
		mouse->setEventCallback(this);
		keyboard->setEventCallback(this);
		mouse->setBuffered(true);
		keyboard->setBuffered(true);
	}


	createSceneManager();
	//Ogre::LogManager::getSingletonPtr()->logMessage("+++ scene manager created");
	setupResources();
	//Ogre::LogManager::getSingletonPtr()->logMessage("+++ resources setup");
	createScene();
	//Ogre::LogManager::getSingletonPtr()->logMessage("+++ scene created");
	createFrameListener();
	//Ogre::LogManager::getSingletonPtr()->logMessage("+++ frameListener created");
	// register our frame listener
	if(frameListener) root->addFrameListener(frameListener);
	//Ogre::LogManager::getSingletonPtr()->logMessage("+++ frameListener registered");
	
}

OgreApp::OgreApp(Ogre::FrameListener *listener, bool useOIS)
{
	// assign parameters
	oisEnabled = useOIS;
	frameListener = listener;

	// member variable initialization
	mouse = 0;
	keyboard = 0;
	inputManager = 0;
	root = 0;
	window = 0;
	sceneMgr = 0;
	activeCamera = 0;
	theRunningApp = this;
}


void OgreApp::addFrameListener(Ogre::FrameListener *l)
{
	root->addFrameListener(l);
}

void OgreApp::windowClosed(Ogre::RenderWindow *rw)
{
	shutdown();
}

bool OgreApp::keyPressed(const OIS::KeyEvent &arg)
{
	
	return true;
}

bool OgreApp::keyReleased(const OIS::KeyEvent &arg)
{
	return true;
}

bool OgreApp::mouseMoved( const OIS::MouseEvent &arg )
{
	return true;
}

bool OgreApp::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return true;
}

bool OgreApp::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return true;
}

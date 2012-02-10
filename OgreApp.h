#ifndef OGRE_APP_H
#define OGRE_APP_H

#include <Ogre.h>
#include <OgreFrameListener.h>
#include <OIS.h>



#define OGRE_APP OgreApp::getInstance()


/**
* An application which combines Ogre nad Bullet into one.  This application holds 
* the root of the Ogre world as well as the dynamicsWorld and other key variables
* required by the Bullet physics engine.  You should derive the class for your
* application from this class and override the methods whose behaviour you want
* to change.  At a minumin, you will override createScene() to place some objects
* in the scene.  The key variables of this class are protected so they can be
* directly accessed by subclasses.  The second most important thing is to provide
* your own FrameListener, which can be provided to the constructor or by overriding
* the createFrameListener method.
* @author R. N. Robson
*/
class OgreApp:
	public Ogre::WindowEventListener,
	OIS::KeyListener, OIS::MouseListener
{
private:
	int initializeRoot();
	void shutdownOIS();
	void setMouseMetrics(Ogre::RenderWindow *win);
	void initializeOIS(Ogre::RenderWindow *win);
	static OgreApp *theRunningApp;
protected:
	Ogre::FrameListener *frameListener;
	OIS::Mouse *mouse;
	OIS::Keyboard *keyboard;
	OIS::InputManager *inputManager;
	Ogre::Root *root;
	Ogre::RenderWindow *window;
	Ogre::SceneManager *sceneMgr;
	bool oisEnabled;
	virtual void windowClosed(Ogre::RenderWindow *rw);
	Ogre::Camera *activeCamera;

	OgreApp(Ogre::FrameListener *listener, bool useOIS);
	
public:
	
	virtual void setInput(Ogre::Vector3 moveVect, float rot){}

	//the thing I made, step god dammit
	virtual void step(float timeSinceLastFrame){}

	virtual void showDebug(bool show){}

	//moves the head
	virtual void moveHead(float x, float y, float z){}

	//receives mouse position from ois, currently compleatly useless
	virtual void setMousePos(float x, float y){}

	//receives mouse button info from ois
	virtual void receiveMouseInput(bool left, bool right){}

	/**
	* Return a reference to the singleton application.
	*/
	/*static OgreApp& getInstance(Ogre::FrameListener *listener, bool useOIS)
	{
		static OgreApp theInstance(listener, useOIS);
		return theInstance;
	}*/

	~OgreApp()
	{
		shutdown();
	}

	/**
	* Return a reference to the running application.
	*/
	static OgreApp& getInstance()
	{
		return *theRunningApp;
	}


	/**
	* This creates an ST_GENERIC scene manager.  Override if you want a different scene manager.
	*/
	virtual void createSceneManager();

	/**
	* Reads the resource files from resources.cfg.
	* Don't touch unless you know what you are doing.
	*/
	virtual void setupResources(void);

	/**
	* Override this to create objects in your scene.
	*/
	virtual void createScene();



	/**
	* Override to create a frame listener if you did not pass one to the constructor.
	* The default version does nothing.
	*/
	virtual void createFrameListener();

	/**
	* This will be called as the app is shutting down and gives derived classes
	* a chance to clean up their stuff.  This is not designed for the termination 
	* of OGRE but for the leanup of other objects which were created by the application.
	*/
	virtual void shutdownApp()
	{
	}

	/**
	*  Clean up memory in the right order.
	* Don't touch unless you know what you are doing.
	*/
	virtual void shutdown();

	/**
	* This invokes the other methods in the correct order to start everything up.
	* Don't touch unless you know what you are doing.
	*/
	virtual void constructObject();

	/**
	* Add a new FrameListener onto the list of frame listeners.
	*/
	virtual void addFrameListener(Ogre::FrameListener *l);

	Ogre::Root* getRoot()
	{
		return root;
	}

	OIS::Keyboard* getKeyboard() const
	{
		return keyboard;
	}

	OIS::Mouse* getMouse() const
	{
		return mouse;
	}

	void setActiveCamera(Ogre::Camera *cam)
	{
		activeCamera = cam;
	}

	Ogre::Camera* getActiveCamera()
	{
		return activeCamera;
	}

	Ogre::SceneManager* getSceneManager() const
	{
		return sceneMgr;
	}

	/**
	* Method for OIS::KeyListener
	*/
	virtual bool keyPressed(const OIS::KeyEvent &arg);

	/**
	* Method for OIS::KeyListener
	*/
	virtual bool keyReleased(const OIS::KeyEvent &arg);

	virtual bool mouseMoved( const OIS::MouseEvent &arg );
    virtual bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
    virtual bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );


};


#endif
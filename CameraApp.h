#ifndef GUI_APP_H
#define GUI_APP_H

#include "OgreApp.h"
#include "DotSceneLoader.h"
#include "BtOgreGP.h"
#include "BtOgrePG.h"
#include <OgreTextAreaOverlayElement.h>
#include <OgreFontManager.h>
#include <Windows.h>
//#include <vector>

class CameraApp: public OgreApp
{
private:
	Ogre::SceneNode *camNode;
	Ogre::TerrainGroup* terrainGrp;
	btAxisSweep3 *mBroadphase;
	btDefaultCollisionConfiguration *mCollisionConfig;
	btCollisionDispatcher *mDispatcher;
	btSequentialImpulseConstraintSolver *mSolver;
	btCollisionShape *mBodyShape;
	btCollisionShape *mlArmShape;
	btCollisionShape *mrArmShape;
	btCollisionShape *mHeadShape;

	btCollisionShape *mPlayerShape;

	Ogre::Vector2 mousePos;


	btCollisionShape *mBoardShape;
	
	btRigidBody *mCharacterBody;
	btRigidBody *mlArmBody;
	btRigidBody *mrArmBody;
	btRigidBody *mHeadBody;

	btRigidBody *mPlayerBody;
	
	btRigidBody *mBoardBody;

	//things used for the GUI
	
	void updateCursor();

	void createGUI();

	void createBox(float posX, float posY, float width, float height, char* texture, char* text, char* name, bool selectable = false);

	void createTextArea();

	void buildMainMenu();

	void buildGameLobby();

	void receiveNetworkMessages();

	POINT getCursorPos();

public:
	virtual void step(float timeSinceLastFrame);

	virtual void setInput(Ogre::Vector3 moveVect, float rot);

	virtual void showDebug(bool show);

	//moves the head
	virtual void moveHead(float x, float y, float z);

	virtual void receiveMouseInput(bool left, bool right);

	//receives the mouse's position from the base fram listener
	virtual void setMousePos(float x, float y);

	CameraApp(Ogre::FrameListener *listener, bool useOIS)
		:OgreApp(listener, useOIS), camNode(NULL)
	{
	}

	virtual void createScene();

	virtual void createFrameListener();

	virtual void createSceneManager();

	virtual void shutdownApp();

	Ogre::SceneNode* getCamNode(){return camNode;}


};

#endif
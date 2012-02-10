#include "BaseFrameListener.h"

bool BaseFrameListener::frameStarted(const FrameEvent &evt) {
	tm += evt.timeSinceLastFrame;
	return true;
}

bool BaseFrameListener::frameEnded(const FrameEvent &evt) {

	// grab the keyboard & mouse state
	app->getKeyboard()->capture();
	app->getMouse()->capture();

	processKeyboardInput(evt);
	processMouseInput();

	if(! keepRendering)
	{
		//OGRE_APP.shutdown();
	}
	
	//CameraApp::step(evt.timeSinceLastFrame);
	//app->step(evt.timeSinceLastFrame);
	app->step(evt.timeSinceLastFrame);


	return keepRendering;

}

void BaseFrameListener::processKeyboardInput(const FrameEvent &evt)
{
	/*
	Ogre::Vector3 moveVect = Ogre::Vector3::ZERO;

	btVector3 headMove;
	headMove.setZero();

	float dist = 1.0;
	bool moving = false;
	bool rotating = false;
	float rotateDir = 1.0;
		

	if(app->getKeyboard()->isKeyDown(OIS::KC_F3)){
		app->showDebug(true);
	}else{
		app->showDebug(false);
	}

	if (app->getKeyboard()->isKeyDown(OIS::KC_ESCAPE))
	{
		keepRendering = false;
		return;
	}

	if (app->getKeyboard()->isKeyDown(OIS::KC_W))
	{
		moveVect.z = -dist;
		moving = true;
	}
	else if (app->getKeyboard()->isKeyDown(OIS::KC_S))
	{
		moveVect.z = dist;
		moving = true;
	}
	else if (app->getKeyboard()->isKeyDown(OIS::KC_RIGHT))
	{
		moveVect.x = -dist;
		moving = true;
	}
	else if (app->getKeyboard()->isKeyDown(OIS::KC_LEFT))
	{
		moveVect.x = dist;
		moving = true;
	}
	else if (app->getKeyboard()->isKeyDown(OIS::KC_DOWN))
	{
		moveVect.y = -dist;
		moving = true;
	}
	else if (app->getKeyboard()->isKeyDown(OIS::KC_UP))
	{
		moveVect.y = dist;
		moving = true;
	}
	else if (app->getKeyboard()->isKeyDown(OIS::KC_A))
	{
		rotateDir = 1.0;
		rotating = true;
	}
	else if (app->getKeyboard()->isKeyDown(OIS::KC_D))
	{
		rotateDir = -1.0;
		rotating = true;
	}

	if(app->getKeyboard()->isKeyDown(OIS::KC_I)){
		headMove.setX(1);
	}
	if(app->getKeyboard()->isKeyDown(OIS::KC_K)){
		headMove.setX(-1);
	}
	if(app->getKeyboard()->isKeyDown(OIS::KC_J)){
		headMove.setZ(1);
	}
	if(app->getKeyboard()->isKeyDown(OIS::KC_L)){
		headMove.setZ(-1);
	}

	app->moveHead(headMove.x(), headMove.y(), headMove.z());

	if(moving)
	{
		((CameraApp*)app)->getActiveCamera()->move(moveVect);
	}
	if(rotating)
	{
		((CameraApp*)app)->getActiveCamera()->yaw(Ogre::Degree(rotateDir * 0.4));
	}
	*/
	Ogre::Vector3 moveVect = Ogre::Vector3::ZERO;

	float dist = 1.0;
	bool moving = false;
	bool rotating = false;
	float rotateDir = 0.0;
		
	if (app->getKeyboard()->isKeyDown(OIS::KC_ESCAPE))
	{
		keepRendering = false;
		return;
	}

	if (app->getKeyboard()->isKeyDown(OIS::KC_A))
	{
		moveVect.z = -dist;
		moving = true;
	}
	if (app->getKeyboard()->isKeyDown(OIS::KC_D))
	{
		moveVect.z = dist;
		moving = true;
	}
	if (app->getKeyboard()->isKeyDown(OIS::KC_W))
	{
		moveVect.x = dist;
		moving = true;
	}
	if (app->getKeyboard()->isKeyDown(OIS::KC_S))
	{
		moveVect.x = -dist;
		moving = true;
	}
	if (app->getKeyboard()->isKeyDown(OIS::KC_DOWN))
	{
		moveVect.y = -dist;
		moving = true;
	}
	if (app->getKeyboard()->isKeyDown(OIS::KC_UP))
	{
		moveVect.y = dist;
		moving = true;
	}
	if (app->getKeyboard()->isKeyDown(OIS::KC_LEFT))
	{
		rotateDir = 1.0;
		rotating = true;
	}
	if (app->getKeyboard()->isKeyDown(OIS::KC_RIGHT))
	{
		rotateDir = -1.0;
		rotating = true;
	}

	if (app->getKeyboard()->isKeyDown(OIS::KC_X))
	{
		rotateDir = -1.0;
		rotating = true;
	}

	if(moving)
	{
		
		//((CameraApp*)app)->getActiveCamera()->move(moveVect);
	}
	if(rotating)
	{
		//((CameraApp*)app)->getActiveCamera()->yaw(Ogre::Degree(rotateDir * 0.4));
	}

	
	app->setInput(moveVect,rotateDir);

	btVector3 headMove;

	headMove.setZero();

	if(app->getKeyboard()->isKeyDown(OIS::KC_I)){
		headMove.setX(1);
	}
	if(app->getKeyboard()->isKeyDown(OIS::KC_K)){
		headMove.setX(-1);
	}
	if(app->getKeyboard()->isKeyDown(OIS::KC_J)){
		headMove.setZ(1);
	}
	if(app->getKeyboard()->isKeyDown(OIS::KC_L)){
		headMove.setZ(-1);
	}

	app->moveHead(headMove.x(), headMove.y(), headMove.z());
}

void BaseFrameListener::processMouseInput()
{
	bool left = false;
	bool right = false;
	if(app->getMouse()->getMouseState().buttonDown(OIS::MB_Left)){
		left = true;
	}
	if(app->getMouse()->getMouseState().buttonDown(OIS::MB_Right)){
		right = true;
	}
	  //mMouse->getMouseState().Y.abs

	((CameraApp*)app)->receiveMouseInput(left, right);
}

void BaseFrameListener::terminateApp()
{
	keepRendering = false;
}
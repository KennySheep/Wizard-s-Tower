/*
#include "ExampleApplication.h"

#define fCameraTranslationSpeed 500.0
#define fCameraRotationSpeed 1.0

#define ogreLevel 1

class Example1FrameListener : public Ogre::FrameListener
{
private:
	Ogre::SceneNode* _node;
	OIS::InputManager* _man;
	OIS::Keyboard* _key;
	OIS::Mouse* _mouse;
	Ogre::Camera* _Cam;
	Ogre::Vector3 SinbadTranslate;

	Ogre::Entity* _ent[ogreLevel];

	float _WalkingSpeed;
	//Ogre::AnimationState* _aniState[ogreLevel-1];
	//Ogre::AnimationState* _aniState2[ogreLevel-1];

	Ogre::AnimationState* _aniStateDance;

	Ogre::AnimationState* _aniRunTop;
	Ogre::AnimationState* _aniRunBottom;



public: 
	Example1FrameListener(Ogre::SceneNode* node, Ogre::Entity* ent[], RenderWindow* win, Ogre::Camera* cam)
	{
		_WalkingSpeed = 5;
		_node = node;

		SinbadTranslate=Ogre::Vector3::ZERO;

		for(int i = 0; i < ogreLevel; i++){
			_ent[i] = ent[i];
		}
		// retrieve a certain animation state "Dance" and enable it:
		
		//_aniState = _ent->getAnimationState("Dance");
		_aniRunTop = _ent[0]->getAnimationState("RunTop");
		_aniRunTop->setEnabled(true);
		_aniRunTop->setLoop(true);

		_aniRunBottom = _ent[0]->getAnimationState("RunBase");
		_aniRunBottom->setEnabled(true);
		_aniRunBottom->setLoop(true);

		//for(int i = 1; i < ogreLevel; i++){ 
			//_aniState[i-1] = _ent[i]->getAnimationState("RunBase");
			//_aniState2[i-1] = _ent[i]->getAnimationState("RunTop");
		
			//_aniState[i-1]->setEnabled(true);
			//_aniState[i-1]->setLoop(true);

			//_aniState2[i-1]->setEnabled(true);
			//_aniState2[i-1]->setLoop(true);
		//}

		_Cam = cam;
		// handle the window data
		size_t windowHnd = 0;
		std::stringstream windowHndStr;
		// Ask for the window handle
		win->getCustomAttribute("WINDOW", &windowHnd);
		// Convert to string
		windowHndStr << windowHnd;
		// Create a parameter list for OIS and add window handle
		OIS::ParamList pl;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
		// Create input system
		_man = OIS::InputManager::createInputSystem(pl);
		// Create keyboard to check for user input
		_key = static_cast<OIS::Keyboard*>(_man->createInputObject(OIS::OISKeyboard, false));
		// Create mouse to check for movement or clicking
		_mouse = static_cast<OIS::Mouse*>(_man->createInputObject(OIS::OISMouse, false));
	}

	bool frameStarted(const Ogre::FrameEvent &evt)
	{

		bool walked = false;

		if(_key->isKeyDown(OIS::KC_UP))
		{
			SinbadTranslate = Ogre::Vector3(0,0,-1);
			walked = true;
		}
		if(_key->isKeyDown(OIS::KC_DOWN))
		{
			SinbadTranslate = Ogre::Vector3(0,0,1);
			walked = true;
		}
		if(_key->isKeyDown(OIS::KC_LEFT))
		{
			SinbadTranslate = Ogre::Vector3(-1,0,0);
			walked = true;
		}
		if(_key->isKeyDown(OIS::KC_RIGHT))
		{
			SinbadTranslate = Ogre::Vector3(1,0,0);
			walked = true;
		}
		/////////////////////////////////////////////////////////////////
		if(walked)
		{
			_node->translate(SinbadTranslate * evt.timeSinceLastFrame * _WalkingSpeed);

			_node->resetOrientation();

			Ogre::Quaternion qOrientation(1, 0, 0, 0);
			Vector3 src = Vector3::UNIT_Z;
			qOrientation = src.getRotationTo(SinbadTranslate);
			_node->rotate(qOrientation);

			
			_aniRunBottom->setEnabled(true);
			_aniRunTop->setEnabled(true);
			if(_aniRunBottom->hasEnded())
			{
				_aniRunBottom->setTimePosition(0.0f);
			}
			if(_aniRunTop->hasEnded())
			{
				_aniRunTop->setTimePosition(0.0f);
			}
		}
		else
		{
			_aniRunBottom->setTimePosition(0.0f);
			_aniRunBottom->setEnabled(false);	
			_aniRunTop->setTimePosition(0.0f);
			_aniRunTop->setEnabled(false);
		}
		
		

			
		//////////////////////////////////////////////////////////////////

		//animation state variable needs to know how much time has elapsed since its last update
		//for(int i = 0; i < ogreLevel-1; i++){
		//	_aniState[i-1]->addTime(evt.timeSinceLastFrame);
		//	_aniState2[i-1]->addTime(evt.timeSinceLastFrame);
		//}
		_aniRunTop->addTime(evt.timeSinceLastFrame);
		_aniRunBottom->addTime(evt.timeSinceLastFrame);

		// check for key presses
		_key->capture();
		// escape game
		if(_key->isKeyDown(OIS::KC_ESCAPE)){
			// close window
			return false;
		}
		// check for mouse movement or clicks
		_mouse->capture();

		//Turn the mouse movement into an angle to rotate the camera
		// Process mouse relative movement into an angle
		float rotX = _mouse->getMouseState().X.rel * evt.timeSinceLastFrame * -1;
		float rotY = _mouse->getMouseState().Y.rel * evt.timeSinceLastFrame * -1;

		//Process keyboard input into camera translation:
		Ogre::Vector3 translate(0, 0, 0);
		if(_key->isKeyDown(OIS::KC_W)){
			translate += Ogre::Vector3(0, 0, -1);
		}
		if(_key->isKeyDown(OIS::KC_S)){
			translate += Ogre::Vector3(0, 0, 1);
		}
		if(_key->isKeyDown(OIS::KC_A)){
			translate += Ogre::Vector3(-1, 0, 0);
		}
		if(_key->isKeyDown(OIS::KC_D)){
			translate += Ogre::Vector3(1, 0, 0);
		}

		translate *= fCameraTranslationSpeed;
		rotX *= fCameraRotationSpeed;
		rotY *= fCameraRotationSpeed;

		// Translate camera using translate vector
		_Cam->moveRelative(translate * evt.timeSinceLastFrame);
	
		// Rotate using Euler angles and rotation speed
		//_Cam->yaw(Ogre::Radian(rotX * fCameraRotationSpeed));
		//_Cam->pitch(Ogre::Radian(rotY * fCameraRotationSpeed));

		// Find the local x and y axes
		Ogre::Vector3 localYAxis;
		Ogre::Vector3 localXAxis;

		// multiply the camera's quaternion by the up and right vectors
		// to get the x and y axes shifted by the camera's orientation
		localYAxis = _Cam->getOrientation() * Vector3::UNIT_Y; 
		localXAxis = _Cam->getOrientation() * Vector3::UNIT_X; 
	
		// Create our quaternions that represent yaw and pitch
		Ogre::Quaternion quatYaw(Ogre::Radian(rotX), Vector3::UNIT_Y);	Ogre::Quaternion quatPitch(Ogre::Radian(rotY), localXAxis);


		// Combine qaw and pitch by multiplying the two quaternions
		Ogre::Quaternion quat = quatYaw * quatPitch;

		// Rotate the camera by our quaternion
		_Cam->rotate(quat);




		return true;
	}

};

class Example1 : public ExampleApplication
{

	private:
	Ogre::Entity* _SinbadEnt;

	Ogre::SceneNode* _SinbadNode;

	Ogre::AnimationState* _aniState;

	Ogre::Entity* ent [ogreLevel];

	public:
	void createScene()
	{
		
		Ogre::SceneNode* node [1];

		node[0] = mSceneMgr->createSceneNode();
		
		_SinbadNode = node[0]->createChildSceneNode("Node 1");

		
		for(int i = 0; i<ogreLevel;i++){
			ent[i] = mSceneMgr->createEntity("Sinbad.mesh");	

			mSceneMgr->getRootSceneNode()->addChild(node[i]);
			_SinbadNode->attachObject(ent[i]);
		}
		
		
		Ogre::Plane plane(Vector3::UNIT_Y, -5);
		Ogre::MeshManager::getSingleton().createPlane("plane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane, 1500, 1500, 200, 200, true, 1, 5, 5, Vector3::UNIT_Z);
		Ogre::Entity* entPlane = mSceneMgr->createEntity("LightPlaneEntity", "plane");
		mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entPlane);
		entPlane->setMaterialName("Examples/BeachStones");

		
		// Add light to scene
		Ogre::Light* light = mSceneMgr->createLight("Light1");
		light->setType(Ogre::Light::LT_DIRECTIONAL);
		light->setDirection(Ogre::Vector3(1, -1, 0));



	}
	// override the exampleApp camera
	void createCamera()
	{
		mCamera = mSceneMgr->createCamera("MyCamera1");
		mCamera->setPosition(0, 100, 200);
		mCamera->lookAt(0,0,0);
		mCamera->setNearClipDistance(5);
	}



	// Add movement to the scene
	void createFrameListener(){
		//Ogre::FrameListener* FrameListener = new Example1FrameListener(_SinbadEnt,*ent, mWindow, mCamera);			//this line still complains

		Ogre::FrameListener* FrameListener = new Example1FrameListener(_SinbadNode, ent,mWindow,mCamera);

		mRoot->addFrameListener(FrameListener);
	}


};

int main (void)
{
	Example1 app;

	app.go();
	return 0;
}
*/

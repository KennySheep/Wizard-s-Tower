#include "CameraApp.h"
#include "BaseFrameListener.h"
#include "BtOgreGP.h"
#include "BtOgrePG.h"
#include "BtOgreExtras.h"
#include ".\ogrelab1\Ragdoll.h"

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "RakNetTypes.h"


#define MAX_PLAYERS 4

#define CONNECT "127.0.0.1"

//Ogre::LogManager::getSingleton().logMessage(Ogre::String());


#include "TCPserver.h"
#include "SendData.h"

#define BUF_LEN 512


#define TURNSPEED 100000.0
#define MOVESPEED 1000.0

GenTCPServerSocket* serverSocket;
Ogre::SceneNode *bodyNode;
Ogre::SceneNode *headNode;
Ogre::SceneNode *lArmNode;
Ogre::SceneNode *rArmNode;

Ogre::SceneNode *playerNode;

Ogre::Camera *camera; 

//the overlay on which all GUI and HUD related things are drawn
Overlay* overlay;

//overlay for the cursor
Overlay* cursorOverlay;

//tells the program if it is a client or a server
bool server;

//the raknet peer to deal with networking for me

RakNet::RakPeerInterface* peer;
RakNet::Packet *packet;


RagDoll *dollBig;

int iResult;
char recvbuf[BUF_LEN];
sendArray sArray;
inputArray cArray;
float playerRot;
float rotValue=0.0;


namespace Globals{
	btDynamicsWorld *phyWorld;
	BtOgre::DebugDrawer *dbdraw;
}


enum GameMessages
{
	ID_GAME_MESSAGE_1=ID_USER_PACKET_ENUM+1,

	SERVER_GAME_OBJECT_LOCATIONS=ID_USER_PACKET_ENUM+2,

	CLIENT_INPUT=ID_USER_PACKET_ENUM+3

};



void CameraApp::setInput(Ogre::Vector3 moveVect, float rot){

	if(server){
		getActiveCamera()->move(moveVect);
		getActiveCamera()->yaw(Ogre::Degree(rot * 0.4));
	}

	cArray.stuff.moveVec = moveVect;
	cArray.stuff.rotVal = rot;
}


void CameraApp::createScene()
{

	//serverSocket->AcceptConnection();

	// Bullet initialisation.
	mBroadphase = new btAxisSweep3(btVector3(-10000,-10000,-10000), btVector3(10000,10000,10000), 1024);
	mCollisionConfig = new btDefaultCollisionConfiguration();
	mDispatcher = new btCollisionDispatcher(mCollisionConfig);
	mSolver = new btSequentialImpulseConstraintSolver();

	Globals::phyWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphase, mSolver, mCollisionConfig);
	Globals::phyWorld->setGravity(btVector3(0,-9.8,0));



	DotSceneLoader loader;

	sceneMgr->setAmbientLight( Ogre::ColourValue( 0, 1, 1 ) );
	Ogre::Light *l = sceneMgr->createLight("MainLight");

	camera = sceneMgr->createCamera("MainCam");
	Ogre::SceneNode *camNode = sceneMgr->getRootSceneNode()->createChildSceneNode( "CameraNode" );	
	camNode->attachObject(camera);
	camNode->translate(0.0, 260.0, -300);
	Ogre::Viewport *vp = window->addViewport(camera);
	vp->setDimensions(0.0f, 0.0f, 1.0f, 1.0f);
	camera->setAspectRatio((float)vp->getActualWidth() / (float) vp->getActualHeight());
	camera->setFarClipDistance(1000.0f);
	camera->setNearClipDistance(5.0f);
	camera->lookAt(0, 0, 0);
	setActiveCamera(camera);
	{
		//creating the body, making it a bullet object

		bodyNode = sceneMgr->getRootSceneNode()->createChildSceneNode( "bodyNode" );
		Ogre::Entity* bodyEnt = sceneMgr->createEntity("ogrehead.mesh");
		bodyNode->attachObject(bodyEnt);
		bodyNode->scale(1.3,1.3,1.3);
		bodyNode->translate(0.0,12.0,10.0);
		//Create shape.
		BtOgre::StaticMeshToShapeConverter converter(bodyEnt);
		mBodyShape = converter.createSphere(); // Declare this btCollisionShape * pointer in SampleApp.h  
		//Calculate inertia tensor.
		btScalar mass = 10;
		btVector3 inertia;
		mBodyShape -> calculateLocalInertia(mass, inertia);
		//Create BtOgre MotionState (connects Ogre and Bullet).
		BtOgre::RigidBodyState *characterState = new BtOgre::RigidBodyState(bodyNode);
		//Create the Body.
		mCharacterBody = new btRigidBody(mass, characterState, mBodyShape, inertia);
		mCharacterBody ->setActivationState(DISABLE_DEACTIVATION);
		mCharacterBody -> getWorldTransform().setOrigin(mCharacterBody->getWorldTransform().getOrigin() + btVector3(-6, 0, -1));
		//add the body to the physics world
		Globals::phyWorld->addRigidBody(mCharacterBody );
	}

	{


	}
	//left arm
	{
		lArmNode = sceneMgr->getRootSceneNode()->createChildSceneNode( "lArmNode" );
		Ogre::Entity* lArmEnt = sceneMgr->createEntity("ogrehead.mesh");
		lArmNode->attachObject(lArmEnt);
		lArmNode->scale(0.1,0.1,0.1);
		lArmNode->translate(0.0,16.0,10.0);
		//Create shape.
		BtOgre::StaticMeshToShapeConverter converter(lArmEnt);
		mlArmShape = converter.createSphere(); // Declare this btCollisionShape * pointer in SampleApp.h  
		//Calculate inertia tensor.
		btScalar mass = 5;
		btVector3 inertia;
		mlArmShape -> calculateLocalInertia(mass, inertia);
		//Create BtOgre MotionState (connects Ogre and Bullet).
		BtOgre::RigidBodyState *lArmState = new BtOgre::RigidBodyState(lArmNode);
		//Create the Body.
		mlArmBody = new btRigidBody(mass, lArmState, mlArmShape, inertia);
		mlArmBody ->setActivationState(DISABLE_DEACTIVATION);

		//add the body to the physics world
		Globals::phyWorld->addRigidBody(mlArmBody );

	}
	//right arm
	{
		rArmNode = sceneMgr->getRootSceneNode()->createChildSceneNode( "rArmNode" );
		Ogre::Entity* rArmEnt = sceneMgr->createEntity("ogrehead.mesh");
		rArmNode->attachObject(rArmEnt);
		rArmNode->scale(0.1,0.1,0.1);
		rArmNode->translate(0.0,20.0,10.0);
		//Create shape.
		BtOgre::StaticMeshToShapeConverter converter(rArmEnt);
		mrArmShape = converter.createSphere(); // Declare this btCollisionShape * pointer in SampleApp.h  
		//Calculate inertia tensor.
		btScalar mass = 5;
		btVector3 inertia;
		mrArmShape -> calculateLocalInertia(mass, inertia);
		//Create BtOgre MotionState (connects Ogre and Bullet).
		BtOgre::RigidBodyState *rArmState = new BtOgre::RigidBodyState(rArmNode);
		//Create the Body.
		mrArmBody = new btRigidBody(mass, rArmState, mrArmShape, inertia);
		mrArmBody ->setActivationState(DISABLE_DEACTIVATION);

		//add the body to the physics world
		Globals::phyWorld->addRigidBody(mrArmBody );
	}



	//dollBig = new RagDoll(Globals::phyWorld,btVector3(30.0,70.0,30.0),btScalar(18.0f));






	{


		/*

		*/
		//creating the Player, making it a bullet object
		/*

		*/
		playerNode = sceneMgr->getRootSceneNode()->createChildSceneNode( "PlayerNode" );
		Ogre::Entity* PlayerEnt = sceneMgr->createEntity("Body.mesh");


		Ogre::Entity* lWing = sceneMgr->createEntity("lWing.mesh");
		//Ogre::Entity* rWing = sceneMgr->createEntity("Wing2.mesh");



		//PlayerEnt->attachObjectToBone("rWing",rWing,Quaternion::IDENTITY,Ogre::Vector3(0.0, 2, 0.0));
		PlayerEnt->attachObjectToBone("lWing",lWing,Quaternion::IDENTITY,Ogre::Vector3(0.0, 0.0, 0.0));


		//PlayerEnt->setMaterial(Mat);
		/*

		*/
		playerNode->attachObject(PlayerEnt);
		playerNode->scale(1.5,1.5,1.5);
		playerNode->translate(0.0,90.0,40.0);
		//Create shape.
		BtOgre::StaticMeshToShapeConverter converter(PlayerEnt);
		mPlayerShape = converter.createCapsule(); // Declare this btCollisionShape * pointer in SampleApp.h  
		//Calculate inertia tensor.
		btScalar mass = 5000;
		btVector3 inertia;
		inertia.setZero();
		mPlayerShape -> calculateLocalInertia(mass, inertia);		//commenting this out should lock the player to its axis
		mass = 5;
		//Create BtOgre MotionState (connects Ogre and Bullet).
		BtOgre::RigidBodyState *PlayerState = new BtOgre::RigidBodyState(playerNode);
		//Create the Body.
		mPlayerBody = new btRigidBody(mass, PlayerState, mPlayerShape, inertia);
		mPlayerBody ->setActivationState(DISABLE_DEACTIVATION);
		btVector3 angFac(0,1,0);
		mPlayerBody->setAngularFactor(angFac);
		mPlayerBody->setRestitution(0.0);
		//add the body to the physics world
		Globals::phyWorld->addRigidBody(mPlayerBody );
	}
	{


		//the head

		//Ogre::MaterialPtr  Mat = static_cast<Ogre::MaterialPtr> ( Ogre::MaterialManager::getSingleton().getByName("butterfly.material") );
		Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create("PlaneMat", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		Ogre::TextureUnitState* tuisTexture = mat->getTechnique(0)->getPass(0)->createTextureUnitState("White.jpg");

		Ogre::Entity* hat = sceneMgr->createEntity("Body.mesh");
		hat->setMaterial(mat);

		headNode = sceneMgr->getRootSceneNode()->createChildSceneNode( "headNode" );
		Ogre::Entity* headEnt = sceneMgr->createEntity("Head.mesh");

		headEnt->attachObjectToBone("hat",hat,Quaternion::IDENTITY,Ogre::Vector3(0.0, 2, 0.0));

		headNode->attachObject(headEnt);
		//headNode->scale(0.1,0.1,0.1);
		headNode->translate(0.0,93.0,40.0);
		//Create shape.
		BtOgre::StaticMeshToShapeConverter converter(headEnt);
		mHeadShape = converter.createSphere(); // Declare this btCollisionShape * pointer in SampleApp.h  
		//Calculate inertia tensor.
		btScalar mass = 5;
		btVector3 inertia;
		mHeadShape -> calculateLocalInertia(mass, inertia);
		//Create BtOgre MotionState (connects Ogre and Bullet).
		BtOgre::RigidBodyState *headState = new BtOgre::RigidBodyState(headNode);
		//Create the Body.
		mHeadBody = new btRigidBody(mass, headState, mHeadShape, inertia);
		mHeadBody ->setActivationState(DISABLE_DEACTIVATION);

		//add the body to the physics world
		Globals::phyWorld->addRigidBody(mHeadBody );


		//jointing head to body
		btGeneric6DofConstraint * joint6DOF;
		btTransform localA, localB;

		localA.setIdentity(); localB.setIdentity();

		float sizer = 0.15;

		//localA.setOrigin(btVector3(btScalar(0.), btScalar(0.30*sizer), btScalar(0.)));
		//localB.setOrigin(btVector3(btScalar(0.), btScalar(-0.14*sizer), btScalar(0.)));

		localA.setOrigin(btVector3(btScalar(0.), btScalar(-1.30), btScalar(0.)));
		localB.setOrigin(btVector3(btScalar(0.), btScalar(1.14), btScalar(0.)));

		joint6DOF = new btGeneric6DofConstraint(*mHeadBody, *mPlayerBody, localA, localB,true);

		joint6DOF->setAngularLowerLimit(btVector3(-SIMD_EPSILON,-SIMD_EPSILON,-SIMD_EPSILON));
		joint6DOF->setAngularUpperLimit(btVector3(SIMD_EPSILON,SIMD_EPSILON,SIMD_EPSILON));

		Globals::phyWorld->addConstraint(joint6DOF, true);

	}


	//Ogre::LogManager::getSingleton().logMessage(Ogre::String("I'm here"));

	loader.parseDotScene(Globals::phyWorld,Ogre::String("new.scene"), 
		Ogre::String("General"), 
		sceneMgr);

	//Ogre::LogManager::getSingleton().logMessage(Ogre::String("Now I'm here"));

	// THis call is necessary to ensure the destructor for the DotSceneLoader
	// does not delete the terrain group it created.  THis is a HACK!!
	terrainGrp = loader.takeTerrainGroup();

	// Get bullet debug lines to show using F3
	Globals::dbdraw = new BtOgre::DebugDrawer(sceneMgr -> getRootSceneNode(), Globals::phyWorld);
	Globals::phyWorld->setDebugDrawer(Globals::dbdraw);




	//sets up the GUI, must be called before any other GUI commands
	createGUI();
	//builds a box, posibly with text
	buildMainMenu();


	

}













void QuaternionToEuler(const btQuaternion &TQuat, btVector3 &TEuler)
{
	btScalar W = TQuat.getW();
	btScalar X = TQuat.getX();
	btScalar Y = TQuat.getY();
	btScalar Z = TQuat.getZ();
	float WSquared = W * W;
	float XSquared = X * X;
	float YSquared = Y * Y;
	float ZSquared = Z * Z;
	TEuler.setX(atan2f(2.0f * (Y * Z + X * W), -XSquared - YSquared + ZSquared + WSquared));
	TEuler.setY(asinf(-2.0f * (X * Z - Y * W)));
	TEuler.setZ(atan2f(2.0f * (X * Y + Z * W), XSquared - YSquared - ZSquared + WSquared));
	//TEuler *= RADTODEG;
};





void CameraApp::step(float timeSinceLastFrame){

	if(peer){
		iResult = 0;

		if(server){
			/*
			playerRot = 0.0;

			btQuaternion quat;
			quat.setEuler(0.0, 0.0, 0.0);
			btTransform transform;
			transform.setIdentity();
			transform.setRotation(quat);
			mPlayerBody->setWorldTransform(transform);
			*/

			sArray.stuff.headPos = headNode->getPosition();
			sArray.stuff.headOrt = headNode->getOrientation();

			sArray.stuff.bodyPos = bodyNode->getPosition();
			sArray.stuff.bodyOrt = bodyNode->getOrientation();

			sArray.stuff.lArmPos = lArmNode->getPosition();
			sArray.stuff.lArmOrt = lArmNode->getOrientation();

			sArray.stuff.rArmPos = rArmNode->getPosition();
			sArray.stuff.rArmOrt = rArmNode->getOrientation();

			sArray.stuff.playerPos = playerNode->getPosition();
			sArray.stuff.playerOrt = playerNode->getOrientation();

			receiveNetworkMessages();

			if(iResult > 0){

				if(cArray.stuff.rotVal!=0.0){

					if((cArray.stuff.rotVal > 0)==(mPlayerBody->getAngularVelocity().y() > 0)){
						mPlayerBody->applyTorque(btVector3(0, cArray.stuff.rotVal*TURNSPEED, 0));
					}else{
						mPlayerBody->setAngularVelocity(btVector3(0.0,0.0,0.0));
						mPlayerBody->applyTorque(btVector3(0, cArray.stuff.rotVal*TURNSPEED, 0));
					}

				}else{


					mPlayerBody->setAngularVelocity(btVector3(0.0,0.0,0.0));
				}




				Ogre::Vector3 temp;
				temp.x = cArray.stuff.camDirec.x;
				temp.y = cArray.stuff.camDirec.y;
				temp.z = cArray.stuff.camDirec.z;


				mPlayerBody->applyCentralForce(btVector3(cArray.stuff.moveVec.x*MOVESPEED*temp.x - cArray.stuff.moveVec.z*MOVESPEED*temp.z, 
					0.0 ,
					cArray.stuff.moveVec.x*MOVESPEED*temp.z + cArray.stuff.moveVec.z*MOVESPEED*temp.x));

			}

			//sArray.buffer.headPos = bodyNode->getPosition();  

			Globals::phyWorld->stepSimulation(timeSinceLastFrame, 10);


			
			RakNet::BitStream bsOut;
			bsOut.Write((RakNet::MessageID)SERVER_GAME_OBJECT_LOCATIONS);
			bsOut.Write(sArray);
			peer->Send(&bsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,RakNet::UNASSIGNED_SYSTEM_ADDRESS,true);
			
			


/*******************************************************************************************************************************/
						////////////////client/server code divide here///////////////////////
/*******************************************************************************************************************************/
		}else{						
			cArray.stuff.camDirec = camera->getDerivedDirection();

			{
				/*
				stringstream ss (stringstream::in | stringstream::out);
				ss << cArray.stuff.camDirec.x;
				ss << cArray.stuff.camDirec.y;
				ss << cArray.stuff.camDirec.z;
				Ogre::LogManager::getSingleton().logMessage("Camera derived direction:");
				Ogre::LogManager::getSingleton().logMessage(ss.str());
				*/
			}			


			/*
			RakNet::BitStream bsOut;
			bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
			bsOut.Write(cArray.buffer);
			peer->Send(&bsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,RakNet::UNASSIGNED_SYSTEM_ADDRESS,true);
			*/
			RakNet::BitStream bsOut;
			bsOut.Write((RakNet::MessageID)CLIENT_INPUT);
			bsOut.Write(cArray);
			peer->Send(&bsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,RakNet::UNASSIGNED_SYSTEM_ADDRESS,true);

			//cArray.stuff.rotVal = 0.0;

			char* str = new char[30];


			/******************************************************************************************/
			//Raknet code, hopefuly will send and receive messages
			/******************************************************************************************/

			receiveNetworkMessages();
			/***********************************************************************************************/


			if(iResult > 0){

				//if(packet && packet->length>0){
				headNode->setPosition(sArray.stuff.headPos.x,
					sArray.stuff.headPos.y,
					sArray.stuff.headPos.z);

				headNode->setOrientation(sArray.stuff.headOrt.w,
					sArray.stuff.headOrt.v.x,
					sArray.stuff.headOrt.v.y,
					sArray.stuff.headOrt.v.z);

				bodyNode->setPosition(sArray.stuff.bodyPos.x,
					sArray.stuff.bodyPos.y,
					sArray.stuff.bodyPos.z);

				bodyNode->setOrientation(sArray.stuff.bodyOrt.w,
					sArray.stuff.bodyOrt.v.x,
					sArray.stuff.bodyOrt.v.y,
					sArray.stuff.bodyOrt.v.z);

				lArmNode->setPosition(sArray.stuff.lArmPos.x,
					sArray.stuff.lArmPos.y,
					sArray.stuff.lArmPos.z);

				lArmNode->setOrientation(sArray.stuff.lArmOrt.w,
					sArray.stuff.lArmOrt.v.x,
					sArray.stuff.lArmOrt.v.y,
					sArray.stuff.lArmOrt.v.z);

				rArmNode->setPosition(sArray.stuff.rArmPos.x,
					sArray.stuff.rArmPos.y,
					sArray.stuff.rArmPos.z);

				rArmNode->setOrientation(sArray.stuff.rArmOrt.w,
					sArray.stuff.rArmOrt.v.x,
					sArray.stuff.rArmOrt.v.y,
					sArray.stuff.rArmOrt.v.z);

				playerNode->setPosition(sArray.stuff.playerPos.x,
					sArray.stuff.playerPos.y,
					sArray.stuff.playerPos.z);

				playerNode->setOrientation(sArray.stuff.playerOrt.w,
					sArray.stuff.playerOrt.v.x,
					sArray.stuff.playerOrt.v.y,
					sArray.stuff.playerOrt.v.z);
				//}


			}
			//peer->Send((char*)cArray.buffer, strlen(cArray.buffer)+1, HIGH_PRIORITY, RELIABLE, 0, RakNet::UNASSIGNED_RAKNET_GUID, true);

			//cArray.stuff.rotVal = 1;	
		}
	}

	updateCursor();
}

void CameraApp::receiveNetworkMessages(){
	for (packet=peer->Receive(); packet; peer->DeallocatePacket(packet), packet=peer->Receive())
	{
		switch (packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			Ogre::LogManager::getSingleton().logMessage("Another client has disconnected.\n");
			break;
		case ID_REMOTE_CONNECTION_LOST:
			Ogre::LogManager::getSingleton().logMessage("Another client has lost the connection.\n");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			Ogre::LogManager::getSingleton().logMessage("Another client has connected.\n");
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				Ogre::LogManager::getSingleton().logMessage("Our connection request has been accepted.\n");
			}
			break;
		case ID_NEW_INCOMING_CONNECTION:
			Ogre::LogManager::getSingleton().logMessage("A connection is incoming.\n");

			for(int  i = 0; i < 10; i++){
					RakNet::BitStream bsOut;
					bsOut.Write((RakNet::MessageID)SERVER_GAME_OBJECT_LOCATIONS);
					bsOut.Write(sArray);
					peer->Send(&bsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,RakNet::UNASSIGNED_SYSTEM_ADDRESS,true);
				}


			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			Ogre::LogManager::getSingleton().logMessage("The server is full.\n");
			break;
		case ID_DISCONNECTION_NOTIFICATION:

			Ogre::LogManager::getSingleton().logMessage("We have been disconnected.\n");

			break;
		case ID_CONNECTION_LOST:

			Ogre::LogManager::getSingleton().logMessage("Connection lost.\n");

			break;

		case CLIENT_INPUT:
			{
				iResult = 1;
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data,packet->length,false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(cArray);
			}
			break;

		case SERVER_GAME_OBJECT_LOCATIONS:
			{
				Ogre::LogManager::getSingleton().logMessage("Game object received");


				iResult = 1;
				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data,packet->length,false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(sArray);
			}
			break;


		case ID_GAME_MESSAGE_1:
			{

				iResult = 1;

				RakNet::RakString rs;
				RakNet::BitStream bsIn(packet->data,packet->length,false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
				bsIn.Read(sArray);
				/*
				bsIn.Read(rs);
				//Ogre::LogManager::getSingleton().logMessage("game message received");

				for(int i = 0; i < rs.GetLength(); i++){
				sArray.buffer[i] = rs.C_String()[i];
				}
				*/
			}
			break;

		default:
			Ogre::LogManager::getSingleton().logMessage("Unknown identifyer");
			break;
		}
	} 
}

void CameraApp::showDebug(bool show){
	Globals::phyWorld->debugDrawWorld();
	//Shows debug if F3 key down.
	Globals::dbdraw->setDebugMode(show);
	Globals::dbdraw->step();

}

void CameraApp::createFrameListener()
{
	frameListener = new BaseFrameListener(this);
}

void CameraApp::createSceneManager()
{
	// get a pointer to the default base scene manager -- sufficient for our purposes
	sceneMgr = root->createSceneManager(Ogre::ST_EXTERIOR_CLOSE );
}

void CameraApp::shutdownApp()
{
	int x = 1337;
}

void CameraApp::moveHead(float x, float y, float z){
	mPlayerBody->applyCentralImpulse(btVector3(x,y,z));


}

POINT  CameraApp::getCursorPos(){
	//gets the cursor position from windows
	POINT ptCursorPos;
	GetCursorPos(&ptCursorPos);

	//gets the screen handle
	HWND hwnd;
	window->getCustomAttribute("HWND", &hwnd);

	//uses the screen handle to set the cursor to ogre coordinents
	ScreenToClient( hwnd, &ptCursorPos );

	int width = window->getViewport(0)->getActualWidth();
	int height = window->getViewport(0)->getActualHeight();

	if(ptCursorPos.x > width)
		SetCursorPos(width,ptCursorPos.y);
	if(ptCursorPos.y > height)
		SetCursorPos(ptCursorPos.x,height);

	return ptCursorPos;
}

void CameraApp::setMousePos(float x, float y){

	//


}

//void CameraApp::resetGUI(){

//}

void CameraApp::updateCursor(){

	POINT ptCursorPos = getCursorPos();

	//moves the cursor to the mouse position
	if(cursorOverlay->getChild("cursor"))
		cursorOverlay->getChild("cursor")->setPosition(ptCursorPos.x,ptCursorPos.y);


	//getActualHeight
	//float height = window->getViewport(0)->getActualHeight();
	//float width = window->getViewport(0)->getActualWidth();


	//if(overlay->findElementAt(ptCursorPos.x/width,ptCursorPos.y/height)!=NULL){

	//	if(overlay->findElementAt(ptCursorPos.x/width,ptCursorPos.y/height)->getCaption()=="hit")
	//		overlay->findElementAt(ptCursorPos.x/width,ptCursorPos.y/height)->setCaption("boob");
	//	else
	//		overlay->findElementAt(ptCursorPos.x/width,ptCursorPos.y/height)->setCaption("hit");
	//}


}

void CameraApp::createGUI(){

	///////////////////////////////////////////////////////////
	//code for creating an overlay
	///////////////////////////////////////////////////////////
	OverlayManager& overlayManager = OverlayManager::getSingleton();
	// load fonts explicitly after resources are loaded (esp. SdkTray.zip) since they will not be loaded on start up from the resource manager
	ResourceManager::ResourceMapIterator iter = Ogre::FontManager::getSingleton().getResourceIterator();
	while (iter.hasMoreElements()) { iter.getNext()->load(); }

	// Create an overlay, and add the panel
	overlay = overlayManager.create("OverlayName");

	overlay->setZOrder(0);

	cursorOverlay = overlayManager.create("CursorOverlay");

	cursorOverlay->setZOrder(1);

	cursorOverlay->show();

	createBox(10, 10, 1, 1, "White.jpg", "Cursor", "cursor", true);
	// Show the overlay
	overlay->show();
	

}


void CameraApp::buildMainMenu(){
	overlay->clear();

	//OverlayManager& overlayManager = OverlayManager::getSingleton();
	//overlayManager.destroyAllOverlayElements();

	//createBox(10, 10, 250, 500, "BeachStones.jpg", "Title menu placeholder", "startMenu");

	createBox(100, 100, 50, 50, "BeachStones.jpg", "Start Game!", "startGame_box");

	createBox(100, 200, 50, 50, "BeachStones.jpg", "Join Game", "joinGame_box");

	createBox(100, 300, 50, 50, "BeachStones.jpg", "Exit Game", "exitGame_box");

}

void CameraApp::buildGameLobby(){
	overlay->clear();
	//OverlayManager& overlayManager = OverlayManager::getSingleton();
	//overlayManager.destroyAllOverlayElements();

	//createBox(10, 10, 250, 500, "BeachStones.jpg", "Title menu placeholder", "startMenu");

	createBox(100, 100, 50, 50, "BeachStones.jpg", "Choose A Weapon", "choseWeapon_box");

	createBox(100, 200, 50, 50, "BeachStones.jpg", "Choose A Spell", "choseSpell_box");

	createBox(100, 300, 50, 50, "BeachStones.jpg", "Go Back", "exitGame");


}

bool prevLeft;
bool prevRight;

void CameraApp::receiveMouseInput(bool left, bool right){

	if((left&&left!=prevLeft)||(right&&right!=prevRight)){

		POINT ptCursorPos = getCursorPos();

		//getActualHeight
		float height = window->getViewport(0)->getActualHeight();
		float width = window->getViewport(0)->getActualWidth();

		//sees if the cursor is over any of the boxes made
		if(overlay->findElementAt(ptCursorPos.x/width,ptCursorPos.y/height)!=NULL){

			if(overlay->findElementAt(ptCursorPos.x/width,ptCursorPos.y/height)->getCaption()=="Start Game!"){	//Join Game
				overlay->findElementAt(ptCursorPos.x/width,ptCursorPos.y/height)->setCaption("boob");

				server = true;
				//real server code
				peer = RakNet::RakPeerInterface::GetInstance();
				peer->Startup(MAX_PLAYERS, &RakNet::SocketDescriptor(8080,0), 1);
				peer->SetMaximumIncomingConnections(MAX_PLAYERS); 

				buildGameLobby();

			}else if(overlay->findElementAt(ptCursorPos.x/width,ptCursorPos.y/height)->getCaption()=="Join Game"){

				server = false;

				overlay->findElementAt(ptCursorPos.x/width,ptCursorPos.y/height)->setCaption("hit");
				//client conection code
				peer = RakNet::RakPeerInterface::GetInstance();
				peer->Startup(1,&RakNet::SocketDescriptor(), 1);
				peer->Connect(CONNECT, 8080, 0, 0);

				buildGameLobby();
			}else if(overlay->findElementAt(ptCursorPos.x/width,ptCursorPos.y/height)->getCaption()=="Go Back"){

				buildMainMenu();
			}
		}

	}

	prevLeft = left;
	prevRight = right;
}

void CameraApp::createBox(float posX, float posY, float width, float height, char* texture, char* text, char* name, bool selectable){
	OverlayManager& overlayManager = OverlayManager::getSingleton();

	// Create a panel
	OverlayContainer* panel = static_cast<OverlayContainer*>(
		overlayManager.createOverlayElement("Panel",name));
	panel->setMetricsMode(Ogre::GMM_PIXELS);
	panel->setPosition(posX, posY);
	panel->setDimensions(width, height);


	Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().create(name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::TextureUnitState* tuisTexture = mat->getTechnique(0)->getPass(0)->createTextureUnitState(texture);
	//panel->setMaterialName("MaterialName"); // Optional background material

	panel->setMaterialName(name);

	if(text!=""){
		// Create a text area
		TextAreaOverlayElement* textArea = static_cast<TextAreaOverlayElement*>(overlayManager.createOverlayElement("TextArea",text));

		textArea->setMetricsMode(Ogre::GMM_PIXELS);
		textArea->setPosition(0, 0);
		textArea->setDimensions(width, height);
		textArea->setCaption(text);
		textArea->setCharHeight(16);
		textArea->setFontName("BlueHighway");
		textArea->setColourBottom(ColourValue(1.0, 0.0, 0.0));
		textArea->setColourTop(ColourValue(1.0, 0.0, 0.0));
		// Add the text area to the panel
		panel->addChild(textArea);
	}

	if(selectable)
		cursorOverlay->add2D(panel);
	else
		overlay->add2D(panel);

	//panel->hide();


}




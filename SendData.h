#include "btBulletDynamicsCommon.h"
#include <Ogre.h>


#pragma once
struct sVector3{
	float x;
	float y;
	float z;

	sVector3 operator= (btVector3 const& b){
		sVector3 temp;
		
		x = b.getX();
		y = b.getY();
		z = b.getZ();

		return *this;
	}
	
	
	sVector3 operator= (Ogre::Vector3 const& b){
		sVector3 temp;

		x = b.x;
		y = b.y;
		z = b.z;

		return *this;
	}
	
};




#pragma once
struct Quart{
	float w;
	sVector3 v;

	Quart operator= (btQuaternion const& b){
		w = b.w();

		v.x = b.x();
		v.y = b.y();
		v.z = b.z();

		return *this;
	}

	Quart operator= (Ogre::Quaternion const& b){
		w = b.w;

		v.x = b.x;
		v.y = b.y;
		v.z = b.z;

		return *this;
	}
};
#pragma once
struct sendData{
	sVector3 headPos;
	Quart headOrt;
	
	sVector3 lArmPos;
	Quart lArmOrt;

	sVector3 rArmPos;
	Quart rArmOrt;

	sVector3 bodyPos;
	Quart bodyOrt;

	sVector3 playerPos;
	Quart playerOrt;
	
	sVector3 ragPos[7];
	Quart ragOrt[7];
	sVector3 ragHalfExtends[7];
		
};

#pragma once
struct imputData{
	sVector3 moveVec;

	sVector3 camDirec;

	float rotVal;	
};

#pragma once
union sendArray{
	sendData stuff;

	char buffer[sizeof(struct sendData)]; 
};

#pragma once
union inputArray{
	imputData stuff;

	char buffer[sizeof(struct imputData)]; 
};

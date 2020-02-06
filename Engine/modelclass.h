////////////////////////////////////////////////////////////////////////////////
// Filename: modelclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11_1.h>
#include <DirectXMath.h> 
#include <random>
using namespace DirectX;

#include <fstream>
using namespace std;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: ModelClass
////////////////////////////////////////////////////////////////////////////////

class ModelClass
{
private:
	struct VertexType
	{
		XMFLOAT3  position;
		XMFLOAT2  texture;
		XMFLOAT3  normal;
	};

	struct ModelType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*, char*, WCHAR*);
	//void TranslateAllVerts(XMFLOAT3);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();



private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	bool LoadModel(char*);
	void ReleaseModel();
		
private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	TextureClass* m_Texture;
	ModelType* m_model;
};

struct MovingModel
{
	ModelClass* model;
	XMFLOAT3 positionToMoveTo;
	XMFLOAT3 originalPosition;
	XMFLOAT3 currentPosition;
	float timeToTake;
	float timeTaken;
	bool startedMovement;
	void newPosition(float time)
	{
		if (checkTime())
		{
			timeTaken += time;
			float portionMoved = timeTaken / timeToTake;
			float newX = (positionToMoveTo.x * portionMoved) - originalPosition.x;
			float newY = (positionToMoveTo.y * portionMoved) - originalPosition.y;
			float newZ = (positionToMoveTo.z * portionMoved) - originalPosition.z;
			currentPosition = XMFLOAT3(newX, newY, newZ);
			//set the position to newX, newY, newZ
			//set the model to face positionToMoveTo.x, positionToMoveTo.y, positionToMoveTo.z
		}
		else
		{
			startedMovement = false;
			currentPosition = originalPosition;
		}
	}
	bool checkTime()
	{
		if (timeTaken < timeToTake)
			return true;
		else if (timeTaken > timeToTake)
			timeTaken = timeToTake;
		return false;
	}
};

struct CuboidZone
{
	float upperBound;
	float lowerBound;
	float frontBound;//closer to the screen (should be smaller than centre)
	float backBound;//further from the screen (should be bigger than centre)
	float leftBound;
	float rightBound;
	bool isInZone(XMFLOAT3 position)
	{//This method uses a left hand system.
		bool xCorrect = (position.x > leftBound && position.x < rightBound);
		bool yCorrect = (position.y > lowerBound && position.y < upperBound);
		bool zCorrect = (position.z > frontBound && position.z < backBound);
		return (xCorrect && yCorrect && zCorrect);
	}
};

struct ModelAndPosition
{
	ModelClass* model;
	XMFLOAT3* position;
	float rotation;//Stores in terms of degrees. This is on the y axis.
};

struct ModelAndPositionExtraPart
{
	ModelAndPosition main;
	ModelClass* extraModel;
};

struct ConstantlyMovingModel
{
	//float DEGREE_TO_RADIAN_CONVERSION_RATE = 0.0174532925f;
	float GOAL_APPROXIMATION = 0.5f;
	float ANGLE_APPROXIMATION = 0.05f;//In radians, how close is close enough to facing the correct direction.
	//float RADIAN_TO_DEGREE_CONVERSION_RATE = 1 / DEGREE_TO_RADIAN_CONVERSION_RATE;
	bool inZone;

	ModelClass* model;
	XMFLOAT3 currentPosition;
	XMFLOAT3* currentDestination;
	XMFLOAT3* currentRotation;//Stores in terms of radians.
	CuboidZone* flyZone;
	CuboidZone goalBounds;

	float speed;//units moved per second, bigger is faster.
	float turnRate;//time in seconds to turn 180 degrees, or PI radians, bigger is slower.

	//The next few methods are awful.
	void awfulZ(float moveBy)
	{
		if (currentDestination->z > currentPosition.z)
			currentPosition.z += moveBy;
		if (currentDestination->z < currentPosition.z)
			currentPosition.z -= moveBy;
	}
	void awfulX(float moveBy)
	{
		if (currentDestination->x > currentPosition.x)
			currentPosition.x += moveBy;
		if (currentDestination->x < currentPosition.x)
			currentPosition.x -= moveBy;
	}
	void awfulY(float moveBy)
	{
		if (currentDestination->y > currentPosition.y)
			currentPosition.y += moveBy;
		if (currentDestination->y < currentPosition.y)
			currentPosition.y -= moveBy;
	}

	void shutdown()
	{
		if (model)
		{
			model->Shutdown();
			delete model;
			model = 0;
		}
		if (flyZone)
		{
			model->Shutdown();
			delete model;
			model = 0;
		}
		if (currentDestination)
		{
			delete currentDestination;
			currentDestination = 0;
		}
		if (currentRotation)
		{
			delete currentRotation;
			currentRotation = 0;
		}
	}

	void softRotate(float turnPower, float needToFaceX, float needToFaceY)
	{

		bool currentUL = (currentRotation->y > XM_PIDIV2);
		bool currentUR = (currentRotation->y > 0 && currentRotation->y < XM_PIDIV2);
		bool currentLL = (currentRotation->y < -XM_PIDIV2);
		bool currentLR = (currentRotation->y < 0 && currentRotation->y > -XM_PIDIV2);
		bool needUL = (needToFaceY > XM_PIDIV2);
		bool needUR = (needToFaceY > 0 && needToFaceY < XM_PIDIV2);
		bool needLL = (needToFaceY < -XM_PIDIV2);
		bool needLR = (needToFaceY < 0 && needToFaceY > -XM_PIDIV2);

		bool bothLeft = ((currentLL || currentUL) && (needLL || needLR));
		bool bothRight = ((currentLR || currentUR) && (needLR || needUR));
		bool bothUpper = ((currentUL || currentUR) && (needUL || needUR));
		bool bothLower = ((currentLL || currentLR) && (needLL || needLR));
		bool sameLeft = ((currentLL && needLL) || (currentUL && needUL));
		bool firstFrame = (!(currentLL || currentUL || currentLR || currentUR));

		bool simpleRotation = (bothRight || bothUpper || bothLower || sameLeft || firstFrame);
		//figure out rotations for opposite corners, and do some other magic for both left.

		if (simpleRotation)
		{
			if (needToFaceY > currentRotation->y)
				currentRotation->y += turnPower;
			else if (needToFaceY < currentRotation->y)
				currentRotation->y -= turnPower;
		}

		else if (bothLeft)//If they're both in left quadrants, but not the same quadrant.
		{
			if (needToFaceY > currentRotation->y)
				currentRotation->y -= turnPower;//clockwise.
			else if (needToFaceY < currentRotation->y)
				currentRotation->y += turnPower;//anti-clockwise.
		}

		else
		{//Opposite quadrants
			if (currentLL || currentLR)//current lower
			{
				float oppositeAngle = currentRotation->y + XM_PI;
				if (needToFaceY < oppositeAngle)
					currentRotation->y -= turnPower;//Turn clockwise
				else //if (needToFaceY > oppositeAngle)
					currentRotation->y += turnPower;//Anti-clockwise
			}
			else if (currentUL || currentUR)
			{
				float oppositeAngle = -(XM_PI - currentRotation->y);
				if (needToFaceY < oppositeAngle)
					currentRotation->y += turnPower;//Anti-clockwise
				else //if (needToFaceY > oppositeAngle)
					currentRotation->y -= turnPower;//Turn clockwise
			}
			else
			{
				currentRotation->y += turnPower;
			}
		}
		//Fucking stumped - only case I can think of is it's trying to cross the axis. i.e. if Pi < needToFaceY or needToFaceY < - Pi

		if (currentRotation->x > XM_PI)
			currentRotation->x = -(XM_PI - (currentRotation->x - XM_PI));
		if (currentRotation->x < -XM_PI)
			//currentRotation->x = -(XM_PI + (XM_PI - currentRotation->x));
			currentRotation->x += XM_2PI;

		if (currentRotation->y < -XM_PI)
			//currentRotation->y = -(XM_PI + (XM_PI - currentRotation->y));
			//currentRotation->y = XM_PI - (currentRotation->y + XM_PI);
			currentRotation->y += XM_2PI;
		if (currentRotation->y > XM_PI)
			currentRotation->y = -(XM_PI - (currentRotation->y - XM_PI));

	}
	void findNewDestination()
	{
		//Select random coordinates that are inside the zone of acceptable coordinates.
		random_device rd1;
		default_random_engine generator1(rd1());
		uniform_real_distribution<float> distributionX(flyZone->leftBound, flyZone->rightBound);
		currentDestination->x = distributionX(generator1);
		random_device rd2;
		default_random_engine generator2(rd2());
		uniform_real_distribution<float> distributionY(flyZone->lowerBound, flyZone->upperBound);
		currentDestination->y = distributionY(generator2);
		random_device rd3;
		default_random_engine generator3(rd3());
		uniform_real_distribution<float> distributionZ(flyZone->frontBound, flyZone->backBound);
		currentDestination->z = distributionZ(generator3);
		//Set the goalBounds so that the model does not have to be on the exact point.
		goalBounds.upperBound = currentDestination->y + GOAL_APPROXIMATION;
		goalBounds.lowerBound = currentDestination->y - GOAL_APPROXIMATION;
		goalBounds.leftBound = currentDestination->x - GOAL_APPROXIMATION;
		goalBounds.rightBound = currentDestination->x + GOAL_APPROXIMATION;
		goalBounds.backBound = currentDestination->z + GOAL_APPROXIMATION;
		goalBounds.frontBound = currentDestination->z - GOAL_APPROXIMATION;
	}
	void moveOriginal(float time)
	{
		//Handle rotation.
		//work out where it needs to face
		//Only allow them to move 1 axis at a time.
		float diffX = currentDestination->x - currentPosition.x;
		float diffY = currentDestination->y - currentPosition.y;
		float diffZ = currentDestination->z - currentPosition.z;
		//float diffXZ = sqrt((diffX * diffX) + (diffZ * diffZ));//Pythagorous.
		float diffXZ = sqrt(pow(diffX, 2) + pow(diffZ, 2));//Pythagorous.
		//float diffYZ = sqrt((diffY * diffY) + (diffZ * diffZ));
		//atanf returns radians.
		//float hypotenuse = sqrt(pow(diffXZ, 2) + pow(diffY, 2));
		//float needToFaceX = atanf(diffY / diffXZ);//a = diffY, b = xz, c = hypotenuse
		//float needToFaceX = acosf(((diffXZ*diffXZ) + (hypotenuse * hypotenuse) - (diffY * diffY)) / (2 * (diffXZ * hypotenuse)));//a = diffY, b = xz, c = hypotenuse
		//float needToFaceY = acosf(((diffXZ*diffXZ) + (diffX * diffX) - (diffZ * diffZ)) / (2 * (diffXZ * diffX)));//a = diffZ, b = diffX, c = xz

		float needToFaceY = atanf(diffZ / diffX);
		if (diffZ < 0 && diffX < 0)
			needToFaceY -= XM_PI;
		else if (diffX < 0)
			needToFaceY += XM_PI;
		//if (needToFaceY < 0)
		//	needToFaceY = -needToFaceY;
		//if (needToFaceY < -XM_PIDIV2)
		//	needToFaceY = -needToFaceY;
		
		//float needToFaceX = 0;

		float needToFaceX = atan(diffY / diffXZ);

		//currentRotation->y = needToFaceY;
		//currentRotation->x = needToFaceX;
		
		//softRotate(time * XMConvertToRadians(1), needToFaceX, needToFaceY);
		float turnPower = (time / turnRate) * XM_PI;
		softRotate(turnPower, needToFaceX, needToFaceY);

		//currentRotation->y = needToFaceY;
		//currentRotation->x = needToFaceX;

		//currentRotation->y = 0;
		//currentRotation->x = 0;

		if (time > 4)//Didn't want to do this, had no choice
			time = 4;
		float moveBy = time * speed;

		float radians = currentRotation->y;
		float radiansX = currentRotation->x;

		//float radians = needToFaceY;
		//float radiansX = needToFaceX;

		float moveX = cosf(radians) * cosf(radiansX) * moveBy;
		float moveZ = sinf(radians) * cosf(radiansX) * moveBy;

		currentPosition.x += moveX;
		currentPosition.z += moveZ;
		
		currentPosition.y += sinf(radiansX) * moveBy;//* cosf(radians) 
		//Would like to use trig to limit this, so that they move at a constant speed,
		//currently they will essentially have a larger speed if they need to move on the y axis.

		//float radians = currentRotation->y;
		//float radiansX = currentRotation->x;
		// Update the position.
		//currentPosition.x += cosf(radians) * cosf(radiansX) * moveBy;
		//if (diffZ > 0)

		//awfulX(sqrt(pow((cosf(radians) * cosf(radiansX) * moveBy), 2)));
		//awfulZ(sqrt(pow((sinf(radians) * cosf(radiansX) * moveBy), 2)));
		//awfulY(sqrt(pow((sinf(radiansX) * cosf(radians) * moveBy), 2)));
		
		//Check if they have come close enough to reaching the destination.
		if (goalBounds.isInZone(currentPosition))
		{
			moveBy++;
			//currentPosition = currentDestination;
			//originalPosition = currentDestination;
			findNewDestination();
		}
	}
};

#endif
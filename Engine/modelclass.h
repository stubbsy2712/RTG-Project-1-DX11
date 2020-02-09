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
	float GOAL_APPROXIMATION = 0.5f;//Arbitrary units, how large the (box) radius around the goal.
	float ANGLE_APPROXIMATION = 0.05f;//In radians, how close is close enough to facing the correct direction.
	string turnDirection = "new";

	ModelClass* model;
	XMFLOAT3 currentPosition;
	XMFLOAT3* currentDestination;
	XMFLOAT3* currentRotation;//Stores in terms of radians.
	CuboidZone* flyZone;
	CuboidZone goalBounds;

	float speed;//units moved per second, bigger is faster.
	float turnRate;//time in seconds to turn 180 degrees, or PI radians, bigger is slower.

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

	string findTurnDirection(float* needToFaceY)
	{

		bool currentUL = (currentRotation->y > XM_PIDIV2);
		bool currentUR = (currentRotation->y > 0 && currentRotation->y < XM_PIDIV2);
		bool currentLL = (currentRotation->y < -XM_PIDIV2);
		bool currentLR = (currentRotation->y < 0 && currentRotation->y > -XM_PIDIV2);
		bool needUL = (*needToFaceY > XM_PIDIV2);
		bool needUR = (*needToFaceY > 0 && *needToFaceY < XM_PIDIV2);
		bool needLL = (*needToFaceY < -XM_PIDIV2);
		bool needLR = (*needToFaceY < 0 && *needToFaceY > -XM_PIDIV2);

		bool bothLeft = ((currentLL || currentUL) && (needLL || needLR));
		bool bothRight = ((currentLR || currentUR) && (needLR || needUR));
		bool bothUpper = ((currentUL || currentUR) && (needUL || needUR));
		bool bothLower = ((currentLL || currentLR) && (needLL || needLR));
		bool sameLeft = ((currentLL && needLL) || (currentUL && needUL));
		bool firstFrame = (!(currentLL || currentUL || currentLR || currentUR));

		bool simpleRotation = (bothRight || bothUpper || bothLower || sameLeft || firstFrame);
		
		//The simple way works
		if (simpleRotation)
		{
			if (*needToFaceY > currentRotation->y)
				return "anti-clockwise";
			else if (*needToFaceY < currentRotation->y)
				return "clockwise";
		}

		//If they're both in left quadrants, but not the same quadrant.
		else if (bothLeft)
		{
			if (*needToFaceY > currentRotation->y)
				return "clockwise";//hell breaks loose if this is reversed
			else
				return "anti-clockwise";//anti-clockwise. Doubles the issue reversed
		}

		//Figuring out the quickest rotation if the current and required rotation are in opposite quadrants.
		else
		{
			if (currentLL || currentLR)//current lower
			{
				float oppositeAngle = currentRotation->y + XM_PI;
				if (*needToFaceY < oppositeAngle)
					return "clockwise";
				else
					return "anti-clockwise";
			}
			else if (currentUL || currentUR)
			{
				float oppositeAngle = -(XM_PI - currentRotation->y);
				if (*needToFaceY < oppositeAngle)
					return "anti-clockwise";
				else
					return "clockwise";
			}
			else
			{
				return "clockwise";//In case something goes horribly wrong (shouldn't ever be called).
			}
		}
	}

	void softRotate(float* turnPower, float* needToFaceX, float* needToFaceY)
	{
		if ((currentRotation->y == *needToFaceY) && (currentRotation->x == *needToFaceX))
			return;

		//Handle y axis rotation.
		if (turnDirection == "new")
			turnDirection = findTurnDirection(needToFaceY);

		if (turnDirection == "clockwise")
			currentRotation->y -= *turnPower;
		else if (turnDirection == "anti-clockwise")
			currentRotation->y += *turnPower;

		if (currentRotation->y < -XM_PI)
			currentRotation->y += XM_2PI;
		if (currentRotation->y > XM_PI)
			currentRotation->y = -(XM_2PI - currentRotation->y);

		float angleOff = currentRotation->y - *needToFaceY;
		if (sqrt(pow(angleOff, 2)) < ANGLE_APPROXIMATION)
		{
			currentRotation->y = *needToFaceY;
			turnDirection = "forward";
		}

		//Turning on the x axis.
		if (*needToFaceX > currentRotation->x)
			currentRotation->x += *turnPower;
		else if (*needToFaceX < currentRotation->x)
			currentRotation->x -= *turnPower;

		if (currentRotation->x > XM_PI)
			currentRotation->x = -(XM_PI - (currentRotation->x - XM_PI));
		if (currentRotation->x < -XM_PI)
			currentRotation->x += XM_2PI;

		angleOff = currentRotation->x - *needToFaceX;
		if (sqrt(pow(angleOff, 2)) < ANGLE_APPROXIMATION)
			currentRotation->x = *needToFaceX;
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
	void move(float* time)
	{
		//work out where it needs to face
		float diffX = currentDestination->x - currentPosition.x;
		float diffY = currentDestination->y - currentPosition.y;
		float diffZ = currentDestination->z - currentPosition.z;
		float diffXZ = sqrt(pow(diffX, 2) + pow(diffZ, 2));

		//atanf returns radians.
		float needToFaceY = atanf(diffZ / diffX);
		if (diffZ < 0 && diffX < 0)
			needToFaceY -= XM_PI;
		else if (diffX < 0)
			needToFaceY += XM_PI;

		//The x axis rotation is much simpler because rotation on that axis is limited.
		float needToFaceX = atan(diffY / diffXZ);

		//Handle rotation.
		float turnPower = (*time / turnRate) * XM_PI;
		softRotate(&turnPower, &needToFaceX, &needToFaceY);
		
		//Didn't want to use a limiter, but had to otherwise the bats would move way too far in the first few frames.
		if (*time > 4)
			*time = 4;
		float moveBy = *time * speed;

		float radians = currentRotation->y;
		float radiansX = currentRotation->x;

		float moveX = cosf(radians) * cosf(radiansX) * moveBy;
		float moveZ = sinf(radians) * cosf(radiansX) * moveBy;

		currentPosition.x += moveX;
		currentPosition.z += moveZ;
		
		currentPosition.y += sinf(radiansX) * moveBy;
		//Would like to use trig to limit this, so that they move at a constant speed,
		//currently they will essentially have a larger speed if they need to move on the y axis.
		//Could patch, but would be a nightmare.
		
		//Check if they have come close enough to reaching the destination.
		if (goalBounds.isInZone(currentPosition))
		{
			findNewDestination();
			turnDirection = "new";
		}
	}
};

#endif
////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "inputclass.h"
#include "d3dclass.h"
#include "timerclass.h"
#include "shadermanagerclass.h"
#include "positionclass.h"
#include "cameraclass.h"
#include "lightclass.h"
#include "modelclass.h"
#include "bumpmodelclass.h"

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = true;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 100000.0f;
const float SCREEN_NEAR = 0.1f;


////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();


private:
	bool HandleMovementInput(float);
	bool Render();
	void setTreePositions();
	void setBatPositions();
	void pickSquirrelTree();
	void resetSquirrel(bool);
	void switchTime(bool);
	void BlackAndWhite(bool);
	void SetDayLight();
	void SetNightLight();

private:
	InputClass* m_Input;
	D3DClass* m_D3D;
	TimerClass* m_Timer;
	ShaderManagerClass* m_ShaderManager;
	PositionClass* m_Position;
	CameraClass* m_Camera;
	LightClass* m_BackgroundLight;
	ModelClass* m_skyBox;
	ModelClass* m_Terrain;
	ModelClass* m_BackupTerrain;
	ModelClass* m_frog;
	XMFLOAT4 Colours;

	DoubleBumpModel m_treeStump;
	ModelAndPositionExtraPart trees[30];//I've tried using pointers for these. It doesn't work. I spent more than
	ConstantlyMovingModel bats[4];//6 hours trying and couldn't find what was causing the error, and couldn't.
	//The error was essentially that I could make the array pointers have the correct value, but each entry
	//was for some reason uninitialized, even though I had definitely initialized them, it works fine like this.

	XMFLOAT3* m_squirrelTreePositions[10];

	ModelClass* m_squirrelModel;
	MovingModel* m_squirrel_moveModel;
	CuboidZone* m_squirrelZone;

	bool dayTime = false;
	int framesPassed = 0;
};

#endif
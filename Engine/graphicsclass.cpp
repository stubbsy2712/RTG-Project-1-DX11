////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
{
	m_Input = 0;
	m_D3D = 0;
	m_Timer = 0;
	m_ShaderManager = 0;
	m_BackgroundLight = 0;
	m_Position = 0;
	m_Camera = 0;
	m_squirrelModel = 0;
	m_skyBox = 0;
	m_frog = 0;
	m_Terrain = 0;
	m_BackupTerrain = 0;
	m_treeStump.partA = 0;
	m_treeStump.partB = 0;
	for each (XMFLOAT3* pos in m_squirrelTreePositions)
	{
		pos = 0;
	}
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	bool result;

	// Create the input object.  The input object will be used to handle reading the keyboard and mouse input from the user.
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	// Initialize the input object.
	result = m_Input->Initialize(hinstance, hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D.", L"Error", MB_OK);
		return false;
	}

	// Create the shader manager object.
	m_ShaderManager = new ShaderManagerClass;
	if(!m_ShaderManager)
	{
		return false;
	}

	// Initialize the shader manager object.
	result = m_ShaderManager->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the shader manager object.", L"Error", MB_OK);
		return false;
	}

	// Create the timer object.
	m_Timer = new TimerClass;
	if (!m_Timer)
	{
		return false;
	}

	// Initialize the timer object.
	result = m_Timer->Initialize();
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the timer object.", L"Error", MB_OK);
		return false;
	}

	// Create the position object.
	m_Position = new PositionClass;
	if (!m_Position)
	{
		return false;
	}

	// Set the initial position and rotation of the viewer.
	m_Position->SetPosition(0.0f, 0.5f, -10.0f);
	m_Position->SetRotation(0.0f, 0.0f, 0.0f);

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.

	// Create the light object.
	m_BackgroundLight = new LightClass;
	if(!m_BackgroundLight)
	{
		return false;
	}

	// Initialize the light object, starting at night.
	SetNightLight();
	//Create the  bump model object for models with normal maps and related vectors.
	
	m_treeStump.partA = new BumpModelClass;
	if (!m_treeStump.partA)
		return false;
	result = m_treeStump.partA->Initialize(m_D3D->GetDevice(), "../Engine/data/Stump_Lower.txt", L"../Engine/data/treeTrunk.dds",
		L"../Engine/data/Trunk_Normal.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Died initializing tree stump lower", L"Error", MB_OK);
		return false;
	}
	m_treeStump.partB = new BumpModelClass;
	if (!m_treeStump.partB)
		return false;
	result = m_treeStump.partB->Initialize(m_D3D->GetDevice(), "../Engine/data/Stump_Top.txt", L"../Engine/data/TreeStump06_Base.dds",
		L"../Engine/data/TreeStump06_Normal.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Died initializing tree stump top", L"Error", MB_OK);
		return false;
	}

	//Now the skybox
	m_skyBox = new ModelClass;
	if (!m_skyBox)
	{
		return false;
	}
	result = m_skyBox->Initialize(m_D3D->GetDevice(), "../Engine/data/SkyBox2.txt", L"../Engine/data/Night_Skybox.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Died initializing sky box", L"Error", MB_OK);
		return false;
	}

	//Terrain
	m_Terrain = new ModelClass;
	if (!m_Terrain)
	{
		return false;
	}
	result = m_Terrain->Initialize(m_D3D->GetDevice(), "../Engine/data/terrain.txt", L"../Engine/data/GrassTexture.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Died initializing terrain", L"Error", MB_OK);
		return false;
	}

	m_BackupTerrain = new ModelClass;
	if (!m_BackupTerrain)
	{
		return false;
	}
	result = m_BackupTerrain->Initialize(m_D3D->GetDevice(), "../Engine/data/terrain.txt", L"../Engine/data/GrassTexture.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Died initializing terrain", L"Error", MB_OK);
		return false;
	}

	//Onto the frog now.
	m_frog = new ModelClass;
	if (!m_frog)
	{
		return false;
	}
	result = m_frog->Initialize(m_D3D->GetDevice(), "../Engine/data/Frog.txt", L"../Engine/data/Night_SkyBox.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Died initializing frog", L"Error", MB_OK);
		return false;
	}

	//Make the tree model, which will be assigned to each tree object.
	ModelClass* treeTrunkModel = new ModelClass;
	result = treeTrunkModel->Initialize(m_D3D->GetDevice(), "../Engine/data/treeTrunk.txt", L"../Engine/data/LowPolyTrunk.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Died at treeTrunk Model", L"Error", MB_OK);
		return false;
	}

	ModelClass* treeBushModel = new ModelClass;
	result = treeBushModel->Initialize(m_D3D->GetDevice(), "../Engine/data/treeBush.txt", L"../Engine/data/LowPolyLeaves.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Died at treeBush Model", L"Error", MB_OK);
		return false;
	}

	for (int x = 0; x < 30; x++)//foreach is bad. Idk why. Foreach loops don't work.
	{
		XMFLOAT3* newPosition = new XMFLOAT3(10.0f, 1.0f, 10.0f);
		trees[x].main.model = treeTrunkModel;
		trees[x].extraModel = treeBushModel;
		trees[x].main.position = newPosition;
	}

	CuboidZone* batzone = new CuboidZone;
	batzone->lowerBound = 12.5f;
	batzone->upperBound = 17.5f;
	batzone->leftBound = -7;
	batzone->rightBound = 7;
	batzone->backBound = 7;
	batzone->frontBound = -7;

	//TODO: Set parameters of bat zone.
	ModelClass* batModel = new ModelClass;
	result = batModel->Initialize(m_D3D->GetDevice(), "../Engine/data/bat.txt", L"../Engine/data/bat.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Died on bats", L"Error", MB_OK);
		return false;
	}

	for (int x = 0; x < 4; x++)
	{
		//XMFLOAT3* newPosition = new XMFLOAT3(10.0f, 1.0f, 10.0f);
		bats[x].model = batModel;
		bats[x].currentRotation = new XMFLOAT3(0, 0, 0);
		bats[x].currentDestination = new XMFLOAT3;
		bats[x].speed = 1.0f;//units per second
		bats[x].turnRate = 10.0f;//seconds per 180 degrees (pi radians).
		bats[x].flyZone = batzone;
		bats[x].findNewDestination();
	}

	// Create the squirrel model object.
	m_squirrelModel = new ModelClass;
	if (!m_squirrelModel)
	{
		return false;
	}

	result = m_squirrelModel->Initialize(m_D3D->GetDevice(), "../Engine/data/squirrel.txt", L"../Engine/data/BrownFur.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the squirrel model object.", L"Error", MB_OK);
		return false;
	}//TODO: Fix the tree positions.

	m_squirrel_moveModel = new MovingModel;
	m_squirrel_moveModel->originalPosition = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_squirrel_moveModel->timeToTake = 2.0f;//this is the length of time it takes to complete it's journey.
	m_squirrel_moveModel->timeTaken = 0.0f;//hasn't started moving yet.
	m_squirrel_moveModel->model = m_squirrelModel;//idk if this is necessary
	m_squirrel_moveModel->currentPosition = m_squirrel_moveModel->originalPosition;

	//rng which tree, lets say there'll be 10
	m_squirrel_moveModel->positionToMoveTo = XMFLOAT3(12.0f, 3.0f, 8.0f);
	//alternatively could use position class to move the tings around.
	
	m_squirrelZone = new CuboidZone;

	m_squirrelZone->backBound = -12.5f;
	m_squirrelZone->frontBound = -7.5f;
	m_squirrelZone->upperBound = 3;
	m_squirrelZone->lowerBound = -1;
	m_squirrelZone->leftBound = -3;
	m_squirrelZone->rightBound = 3;

	setTreePositions();
	setBatPositions();

	return true;
}

//void GraphicsClass::playAudio()
//{
//	if (m_audio)
//}

void GraphicsClass::Shutdown()
{
	//TODO: Shutdown for everything.

	// Release the light object.
	if(m_BackgroundLight)
	{
		delete m_BackgroundLight;
		m_BackgroundLight = 0;
	}
	// Release the squirrel model.
	if (m_squirrelModel)
	{
		m_squirrelModel->Shutdown();
		delete m_squirrelModel;
		m_squirrelModel = 0;
	}
	// Release skybox.
	if (m_skyBox)
	{
		m_skyBox->Shutdown();
		delete m_skyBox;
		m_skyBox = 0;
	}
	// Release the frog.
	if (m_frog)
	{
		m_frog->Shutdown();
		delete m_frog;
		m_frog = 0;
	}
	// Release the terrain.
	if (m_Terrain)
	{
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = 0;
	}
	// Release the backing terrain.
	if (m_BackupTerrain)
	{
		m_BackupTerrain->Shutdown();
		delete m_BackupTerrain;
		m_BackupTerrain = 0;
	}
	// Release the tree stump.
	if (m_treeStump.partA)
	{
		m_treeStump.partA->Shutdown();
		delete m_treeStump.partA;
		m_treeStump.partA = 0;
	}

	if (m_treeStump.partB)
	{
		m_treeStump.partB->Shutdown();
		delete m_treeStump.partB;
		m_treeStump.partB = 0;
	}
	// Release the trees.
	for (int i = 0; i < 30; i++)
	{
		if (trees[i].main.model)
		{
			trees[i].main.model->Shutdown();
			delete trees[i].main.model;
			trees[i].main.model = 0;
		}
		if (trees[i].extraModel)
		{
			trees[i].extraModel->Shutdown();
			delete trees[i].extraModel;
			trees[i].extraModel = 0;
		}
	}
	// Release the bats.
	for (int i = 0; i < 4; i++)
	{
		if (bats[i].model)
		{
			bats[i].model->Shutdown();
			//delete bats[i].model;
			//bats[i].model = 0;
		}
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the position object.
	if (m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	// Release the shader manager object.
	if(m_ShaderManager)
	{
		m_ShaderManager->Shutdown();
		delete m_ShaderManager;
		m_ShaderManager = 0;
	}

	// Release the timer object.
	if (m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	// Release the input object.
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	return;
}

bool GraphicsClass::Frame()
{
	bool result;

	// Update the system stats.
	m_Timer->Frame();

	// Read the user input.
	result = m_Input->Frame();
	if (!result)
	{
		return false;
	}

	// Check if the user pressed escape and wants to exit the application.
	if (m_Input->IsEscapePressed() == true)
	{
		return false;
	}

	// Do the frame input processing.
	result = HandleMovementInput(m_Timer->GetTime());
	//result = HandleMovementInput(m_Timer->GetDelta());
	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::HandleMovementInput(float frameTime)
{
	bool keyDown;
	float posX, posY, posZ, rotX, rotY, rotZ;

	// Set the frame time for calculating the updated position.
	m_Position->SetFrameTime(frameTime);

	// Handle the input.
	keyDown = m_Input->IsLeftPressed();
	m_Position->TurnLeft(keyDown);

	keyDown = m_Input->IsRightPressed();
	m_Position->TurnRight(keyDown);

	keyDown = m_Input->IsUpPressed();
	m_Position->LookUpward(keyDown);

	keyDown = m_Input->IsDownPressed();
	m_Position->LookDownward(keyDown);

	keyDown = m_Input->IsWPressed();
	m_Position->MoveForward(keyDown);

	keyDown = m_Input->IsSPressed();
	m_Position->MoveBackward(keyDown);

	keyDown = m_Input->IsRPressed();
	m_Position->MoveUpward(keyDown);

	keyDown = m_Input->IsFPressed();
	m_Position->MoveDownward(keyDown);

	keyDown = m_Input->IsAPressed();
	m_Position->StrafeLeft(keyDown);

	keyDown = m_Input->IsDPressed();//:(
	m_Position->StrafeRight(keyDown);

	keyDown = m_Input->IsZPressed();
	resetSquirrel(keyDown);

	keyDown = m_Input->IsSpacePressed();
	switchTime(keyDown);

	keyDown = m_Input->IsQPressed();
	BlackAndWhite(keyDown);

	// HandleMouse Rotations
	m_Position->MouseRotate(m_Input->GetMouseXDelta(), m_Input->GetMouseYDelta());

	// Get the view point position/rotation.
	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);
	m_Input->ResetCursorPos();

	// Set the position of the camera.
	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

	return true;
}

void GraphicsClass::switchTime(bool keyDown)
{
	if (keyDown)
	{
		if (dayTime)
		{
			dayTime = false;
			m_skyBox->Initialize(m_D3D->GetDevice(), "../Engine/data/SkyBox2.txt", L"../Engine/data/Night_Skybox.dds");
			if (m_BackgroundLight->GetSpecularPower() != 0.0f)//Check that the black and white version isn't on.
				SetNightLight();
		}
		else
		{
			dayTime = true;
			m_skyBox->Initialize(m_D3D->GetDevice(), "../Engine/data/SkyBox2.txt", L"../Engine/data/DaylightBox.dds");
			if (m_BackgroundLight->GetSpecularPower() != 0.0f)//Check that the black and white version isn't on.
				SetDayLight();
		}
	}
}

void GraphicsClass::BlackAndWhite(bool keyDown)
{
	if (keyDown)
	{
		if (m_BackgroundLight->GetSpecularPower() == 0.0f)
		{
			if (dayTime)
				SetDayLight();
			else
				SetNightLight();
		}
		else
		{
			m_BackgroundLight->SetAmbientColor(0.0f, 0.0f, 0.0f, 0.0f);
			m_BackgroundLight->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
			m_BackgroundLight->SetDirection(0.0f, -1.0f, 0.0f);
			m_BackgroundLight->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
			m_BackgroundLight->SetSpecularPower(0.0f);
		}
	}
}

void GraphicsClass::SetDayLight()
{
	m_BackgroundLight->SetAmbientColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_BackgroundLight->SetDiffuseColor(0.95f, 0.95f, 1.0f, 1.0f);
	m_BackgroundLight->SetDirection(0.0f, -1.0f, 0.0f);
	m_BackgroundLight->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_BackgroundLight->SetSpecularPower(64.0f);
}

void GraphicsClass::SetNightLight()
{
	m_BackgroundLight->SetAmbientColor(0.4f, 0.25f, 0.5f, 1.0f);
	m_BackgroundLight->SetDiffuseColor(0.5f, 0.0f, 1.0f, 1.0f);
	m_BackgroundLight->SetDirection(0.0f, -1.0f, 0.0f);
	m_BackgroundLight->SetSpecularColor(0.5f, 0.25f, 1.0f, 1.0f);
	m_BackgroundLight->SetSpecularPower(64.0f);//Something weird happens if this is set to 0.
}

void GraphicsClass::resetSquirrel(bool keyDown)
{
	//No need to modify current position, as that is calculated using timeTaken and originalPosition.
	if (keyDown)
	{
		m_squirrel_moveModel->timeTaken = 0.0f;
		pickSquirrelTree();
	}
}

void GraphicsClass::setTreePositions()
{
	trees[0].main.position = new XMFLOAT3(1, 0, 5);
	trees[1].main.position = new XMFLOAT3(2, 0, -4);
	trees[2].main.position = new XMFLOAT3(3, 0, 8);
	trees[3].main.position = new XMFLOAT3(-4, 0, 4);
	trees[4].main.position = new XMFLOAT3(-5, 0, -8);
	trees[5].main.position = new XMFLOAT3(6.5f, 0, 1);
	trees[6].main.position = new XMFLOAT3(2, -0, -7.5f);
	trees[7].main.position = new XMFLOAT3(4.75f, 0, -3);
	trees[8].main.position = new XMFLOAT3(-8.5f, 0, -3);
	trees[9].main.position = new XMFLOAT3(-6, 0, 4.5f);
	trees[10].main.position = new XMFLOAT3(8, 0, 13);
	trees[11].main.position = new XMFLOAT3(-5, 0, 1);
	trees[12].main.position = new XMFLOAT3(3, 0, -4);
	trees[13].main.position = new XMFLOAT3(-3.5f, 0, 1);
	trees[14].main.position = new XMFLOAT3(0, 0, 5);
	trees[15].main.position = new XMFLOAT3(-2, 0, 1);
	trees[16].main.position = new XMFLOAT3(8, 0, 4);
	trees[17].main.position = new XMFLOAT3(-1.75f, 0, -5);
	trees[18].main.position = new XMFLOAT3(-1.5f, 0, 3);
	trees[19].main.position = new XMFLOAT3(0, 0, -12);
	trees[20].main.position = new XMFLOAT3(-4, 0, -5);
	trees[21].main.position = new XMFLOAT3(-10, 0, -2);
	trees[22].main.position = new XMFLOAT3(-7.1f, 0, -5);
	trees[23].main.position = new XMFLOAT3(4, 0, 2.5f);
	trees[24].main.position = new XMFLOAT3(2, 0, 4);
	trees[25].main.position = new XMFLOAT3(7, 0, -5);
	trees[26].main.position = new XMFLOAT3(-2.5f, 0, -5);
	trees[27].main.position = new XMFLOAT3(3, 0, 6);
	trees[28].main.position = new XMFLOAT3(-9, 0, 2);
	trees[29].main.position = new XMFLOAT3(-4, 0, -11);
	trees[0].main.rotation = 0;
	trees[1].main.rotation = 12;
	trees[2].main.rotation = 24;
	trees[3].main.rotation = 36;
	trees[4].main.rotation = 48;
	trees[5].main.rotation = 60;
	trees[6].main.rotation = 72;
	trees[7].main.rotation = 84;
	trees[8].main.rotation = 96;
	trees[9].main.rotation = 108;
	trees[10].main.rotation = 120;
	trees[11].main.rotation = 132;
	trees[12].main.rotation = 144;
	trees[13].main.rotation = 156;
	trees[14].main.rotation = 168;
	trees[15].main.rotation = 180;
	trees[16].main.rotation = 192;
	trees[17].main.rotation = 204;
	trees[18].main.rotation = 216;
	trees[19].main.rotation = 228;
	trees[20].main.rotation = 240;
	trees[21].main.rotation = 252;
	trees[22].main.rotation = 264;
	trees[23].main.rotation = 276;
	trees[24].main.rotation = 288;
	trees[25].main.rotation = 300;
	trees[26].main.rotation = 312;
	trees[27].main.rotation = 324;
	trees[28].main.rotation = 336;
	trees[29].main.rotation = 348;
}

void GraphicsClass::pickSquirrelTree()
{
	//RNG which tree.
	random_device rd;
	default_random_engine generator(rd());
	uniform_int_distribution<int> distribution(1, 10);
	int treeNumber = distribution(generator);
	m_squirrel_moveModel->positionToMoveTo = *trees[(3 * treeNumber) - 1].main.position;
}

void GraphicsClass::setBatPositions()
{
	bats[0].currentPosition = XMFLOAT3(-7, 12.5f, -7);
	bats[1].currentPosition = XMFLOAT3(-7, 12.5f, 7);
	bats[2].currentPosition = XMFLOAT3(7, 12.5f, -7);
	bats[3].currentPosition = XMFLOAT3(7, 12.5f, 7);
}

bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;
	bool result;

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	for (int x = 0; x < 4; x++)
	{
		bats[x].moveOriginal(m_Timer->GetTime() * 0.0005);
		m_D3D->GetWorldMatrix(worldMatrix);
		worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));//Scale first
		translateMatrix = XMMatrixTranslation(bats[x].currentPosition.x, bats[x].currentPosition.y, bats[x].currentPosition.z);
		worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationX(bats[x].currentRotation->x));
		worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationY(- (bats[x].currentRotation->y + XM_PIDIV2)));
		worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);
		bats[x].model->Render(m_D3D->GetDeviceContext());
		result = m_ShaderManager->RenderLightShader(m_D3D->GetDeviceContext(), bats[x].model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
			bats[x].model->GetTexture(), m_BackgroundLight->GetDirection(), m_BackgroundLight->GetAmbientColor(), m_BackgroundLight->GetDiffuseColor(),
			m_Camera->GetPosition(), m_BackgroundLight->GetSpecularColor(), m_BackgroundLight->GetSpecularPower());
		if (!result)
			return false;
	}

	//move the squirrel accordingly
	m_D3D->GetWorldMatrix(worldMatrix);
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(0.125f, 0.125f, 0.125f));//Scale first
	if (!m_squirrelZone->isInZone(m_Camera->GetPosition()))//Check they're out of the start before the squirrel starts moving.
		m_squirrel_moveModel->newPosition(0.0005f * m_Timer->GetTime());
	XMFLOAT3 newPos = m_squirrel_moveModel->currentPosition;
	translateMatrix = XMMatrixTranslation(newPos.x, newPos.y, newPos.z);
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);

	m_squirrelModel->Render(m_D3D->GetDeviceContext());
	result = m_ShaderManager->RenderLightShader(m_D3D->GetDeviceContext(), m_squirrelModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_squirrelModel->GetTexture(), m_BackgroundLight->GetDirection(), m_BackgroundLight->GetAmbientColor(), m_BackgroundLight->GetDiffuseColor(),
		m_Camera->GetPosition(), m_BackgroundLight->GetSpecularColor(), m_BackgroundLight->GetSpecularPower());

	if (!result)
		return false;

	//The tree stump, main part and top part.
	m_D3D->GetWorldMatrix(worldMatrix);
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(0.00125f, 0.00125f, 0.00125f));
	translateMatrix = XMMatrixTranslation(0.2f, 0.0f, 0.0f);
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);
	m_treeStump.partA->Render(m_D3D->GetDeviceContext());
	result = m_ShaderManager->RenderBumpMapShader(m_D3D->GetDeviceContext(), m_treeStump.partA->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
		m_treeStump.partA->GetColorTexture(), m_treeStump.partA->GetNormalMapTexture(), m_BackgroundLight->GetDirection(), m_BackgroundLight->GetDiffuseColor());
	if (!result)
		return false;
	m_treeStump.partB->Render(m_D3D->GetDeviceContext());
	result = m_ShaderManager->RenderBumpMapShader(m_D3D->GetDeviceContext(), m_treeStump.partB->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_treeStump.partB->GetColorTexture(), m_treeStump.partB->GetNormalMapTexture(), m_BackgroundLight->GetDirection(), m_BackgroundLight->GetDiffuseColor());
	if (!result)
		return false;

	//The skybox
	m_D3D->GetWorldMatrix(worldMatrix);
	translateMatrix = XMMatrixTranslation(m_Camera->GetPosition().x, m_Camera->GetPosition().y - 10, m_Camera->GetPosition().z);
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(1.0f, 2.0f, 1.0f));
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);
	m_skyBox->Render(m_D3D->GetDeviceContext());
	result = m_ShaderManager->RenderLightShader(m_D3D->GetDeviceContext(), m_skyBox->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_skyBox->GetTexture(), m_BackgroundLight->GetDirection(), m_BackgroundLight->GetAmbientColor(), m_BackgroundLight->GetDiffuseColor(),
		m_Camera->GetPosition(), m_BackgroundLight->GetSpecularColor(), m_BackgroundLight->GetSpecularPower());
	if (!result)
		return false;

	//Next the terrain.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_Terrain->Render(m_D3D->GetDeviceContext());
	translateMatrix = XMMatrixTranslation(0.0f, -0.5f, 0.0f);
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(0.35f, 0.005f, 0.35f));
	result = m_ShaderManager->RenderLightShader(m_D3D->GetDeviceContext(), m_Terrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_Terrain->GetTexture(), m_BackgroundLight->GetDirection(), m_BackgroundLight->GetAmbientColor(), m_BackgroundLight->GetDiffuseColor(),
		m_Camera->GetPosition(), m_BackgroundLight->GetSpecularColor(), m_BackgroundLight->GetSpecularPower());
	if (!result)
		return false;

	//FROG

	float rValue = cosf(0.05f * framesPassed);//1 loop = 2 pi (approx 6.2832)
	float gValue = sinf(0.05f * framesPassed);//6.2832 / 0.05 = 125.6 approx
	float bValue = cosf(0.05f * framesPassed + XM_PI);//approx 125.6 frames per loop.

	if (rValue < 0)
		rValue = 0;
	if (gValue < 0)
		gValue = 0;
	if (bValue < 0)
		bValue = 0;
	Colours = XMFLOAT4(rValue, gValue, bValue, 1.0f);

	m_D3D->GetWorldMatrix(worldMatrix);
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(0.01f, 0.01f, 0.01f));//Scale first
	translateMatrix = XMMatrixTranslation(-7.5f, 0.0f, 0.0f);
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);

	m_frog->Render(m_D3D->GetDeviceContext());
	result = m_ShaderManager->RenderLightShader(m_D3D->GetDeviceContext(), m_frog->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_frog->GetTexture(), m_BackgroundLight->GetDirection(), m_BackgroundLight->GetAmbientColor(), Colours,
		m_Camera->GetPosition(), Colours, m_BackgroundLight->GetSpecularPower());


	for each (ModelAndPositionExtraPart tree in trees)
	{

		m_D3D->GetWorldMatrix(worldMatrix);
		worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(0.001f, 0.001f, 0.001f));//Scale first
		worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixRotationY(tree.main.rotation));//Rotate
		translateMatrix = XMMatrixTranslation(tree.main.position->x, tree.main.position->y, tree.main.position->z);
		worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);

		tree.main.model->Render(m_D3D->GetDeviceContext());
		result = m_ShaderManager->RenderLightShader(m_D3D->GetDeviceContext(), tree.main.model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
			tree.main.model->GetTexture(), m_BackgroundLight->GetDirection(), m_BackgroundLight->GetAmbientColor(), m_BackgroundLight->GetDiffuseColor(),
			m_Camera->GetPosition(), m_BackgroundLight->GetSpecularColor(), m_BackgroundLight->GetSpecularPower());
		if (!result)
			return false;

		//Keep the modified world matrix for the second part of the tree.
		tree.extraModel->Render(m_D3D->GetDeviceContext());
		result = m_ShaderManager->RenderLightShader(m_D3D->GetDeviceContext(), tree.extraModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
			tree.extraModel->GetTexture(), m_BackgroundLight->GetDirection(), m_BackgroundLight->GetAmbientColor(), m_BackgroundLight->GetDiffuseColor(),
			m_Camera->GetPosition(), m_BackgroundLight->GetSpecularColor(), m_BackgroundLight->GetSpecularPower());
		if (!result)
			return false;
	}

	//The backup terrain.
	m_D3D->GetWorldMatrix(worldMatrix);
	m_BackupTerrain->Render(m_D3D->GetDeviceContext());
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixScaling(5.0f, 0.05f, 5.0f));
	translateMatrix = XMMatrixTranslation(0.0f, -5.0f, 0.0f);
	worldMatrix = XMMatrixMultiply(worldMatrix, translateMatrix);
	result = m_ShaderManager->RenderLightShader(m_D3D->GetDeviceContext(), m_BackupTerrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix,
		m_BackupTerrain->GetTexture(), m_BackgroundLight->GetDirection(), m_BackgroundLight->GetAmbientColor(), m_BackgroundLight->GetDiffuseColor(),
		m_Camera->GetPosition(), m_BackgroundLight->GetSpecularColor(), m_BackgroundLight->GetSpecularPower());
	if (!result)
		return false;

	// Present the rendered scene to the screen.
	m_D3D->EndScene();
	framesPassed++;

	return true;
}
////////////////////////////////////////////////////////////////////////////////
// Filename: CApp.cpp
////////////////////////////////////////////////////////////////////////////////
#include "CApp.h"


CApp::CApp()
{
	m_Input = 0;
	m_Direct3D = 0;
	m_Camera = 0;
	m_Terrain = 0;
	m_Timer = 0;
	m_Position = 0;
	m_Fps = 0;
	m_Cpu = 0;
	m_FontShader = 0;
	m_Text = 0;
	m_TerrainShader = 0;
	m_Light = 0;
	m_SkyDome = 0;
	m_SkydomeShader = 0;
	m_SkyPlane = 0;
	m_SkyPlaneShader = 0;
	m_Frustum = 0;
	m_Grass = 0;
	m_GrassShader = 0;
	m_QuadTree = 0;
}

CApp::CApp( const CApp& other )
{
}

CApp::~CApp()
{
}

bool CApp::Initialize( HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight )
{
	bool result;
	float cameraX, cameraY, cameraZ;
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	char videoCard[128];
	int videoMemory;

	
	// Create the input object.  The input object will be used to handle reading the keyboard and mouse input from the user.
	m_Input = new InputClass;
	if( !m_Input )
	{
		return false;
	}

	// Initialize the input object.
	result = m_Input->Initialize( hinstance, hwnd, screenWidth, screenHeight );
	if( !result )
	{
		MessageBox( hwnd, "Could not initialize the input object.", "Error", MB_OK );
		return false;
	}

	// Create the Direct3D object.
	m_Direct3D = new CDirect3DSystem;
	if( !m_Direct3D )
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_Direct3D->Initialize( screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR );
	if( !result )
	{
		MessageBox( hwnd, "Could not initialize DirectX 11.", "Error", MB_OK );
		return false;
	}

	m_Grass = new CGrass( NUMBER_OF_BLADES );
	if( !m_Grass )
	{
		return false;
	}

	result = m_Grass->Initialize( m_Direct3D->GetDevice() );
	if( !result )
	{
		MessageBox( hwnd, "Could not initialize Grass.", "Error", MB_OK );
		return false;
	}

	m_GrassShader = new CGrassShader;

	result = m_GrassShader->Initialize( m_Direct3D->GetDevice(), hwnd );
	if( !m_GrassShader )
	{
		MessageBox( hwnd, "Could not initialize grass shader.", "Error", MB_OK );
		return false;
	}

	// Create the camera object.
	m_Camera = new CCamera;
	if( !m_Camera )
	{
		return false;
	}

	// Initialize a base view matrix with the camera for 2D user interface rendering.
	m_Camera->SetPosition( 0.0f, 0.0f, -1.0f );
	m_Camera->Render();
	m_Camera->GetViewMatrix( viewMatrix );

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix( worldMatrix );
	m_Camera->GetViewMatrix( viewMatrix );
	m_Direct3D->GetProjectionMatrix( projectionMatrix );
	m_Direct3D->GetOrthoMatrix( orthoMatrix );

	// Set the initial position of the camera.
	//cameraX = 122.0f;
	//cameraY = 14.0f;
	//cameraZ = 125.0f;

	cameraX = 87;
	cameraY = 7;
	cameraZ = 151;

	m_Camera->SetPosition( cameraX, cameraY, cameraZ );

	// Create the terrain object.
	m_Terrain = new CTerrain;
	if( !m_Terrain )
	{
		return false;
	}

	// Initialize the terrain object.
	result = m_Terrain->Initialize( m_Direct3D->GetDevice(), "../../data/heightmap01.bmp", "../../data/flat.jpg", "../../data/colorm01.bmp" );
	if( !result )
	{
		MessageBox( hwnd, "Could not initialize the terrain object.", "Error", MB_OK );
		return false;
	}

	// Create the timer object.
	m_Timer = new CTimer;
	if( !m_Timer )
	{
		return false;
	}

	// Initialize the timer object.
	result = m_Timer->Initialize();
	if( !result )
	{
		MessageBox( hwnd, "Could not initialize the timer object.", "Error", MB_OK );
		return false;
	}

	// Create the position object.
	m_Position = new CPosition;
	if( !m_Position )
	{
		return false;
	}

	// Set the initial position of the viewer to the same as the initial camera position.
	m_Position->SetPosition( cameraX, cameraY, cameraZ );

	// Create the fps object.
	m_Fps = new CFPS;
	if( !m_Fps )
	{
		return false;
	}

	// Initialize the fps object.
	m_Fps->Initialize();

	// Create the cpu object.
	m_Cpu = new CCPU;
	if( !m_Cpu )
	{
		return false;
	}

	// Initialize the cpu object.
	m_Cpu->Initialize();

	// Create the font shader object.
	m_FontShader = new CFontShader;
	if( !m_FontShader )
	{
		return false;
	}

	// Initialize the font shader object.
	result = m_FontShader->Initialize( m_Direct3D->GetDevice(), hwnd );
	if(!result)
	{
		MessageBox( hwnd, "Could not initialize the font shader object.", "Error", MB_OK );
		return false;
	}

	// Create the text object.
	m_Text = new CText;
	if( !m_Text )
	{
		return false;
	}

	// Initialize the text object.
	result = m_Text->Initialize( m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, screenWidth, screenHeight, viewMatrix );
	if( !result )
	{
		MessageBox( hwnd, "Could not initialize the text object.", "Error", MB_OK );
		return false;
	}

	// Retrieve the video card information.
	m_Direct3D->GetVideoCardInfo( videoCard, videoMemory );

	// Set the video card information in the text object.
	result = m_Text->SetVideoCardInfo( videoCard, videoMemory, m_Direct3D->GetDeviceContext() );
	if( !result )
	{
		MessageBox( hwnd, "Could not set video card info in the text object.", "Error", MB_OK );
		return false;
	}

	// Create the terrain shader object.
	m_TerrainShader = new CTerrainShader;
	if( !m_TerrainShader )
	{
		return false;
	}

	// Initialize the terrain shader object.
	result = m_TerrainShader->Initialize( m_Direct3D->GetDevice(), hwnd );
	if(!result)
	{
		MessageBox( hwnd, "Could not initialize the terrain shader object.", "Error", MB_OK );
		return false;
	}

	// Create the light object.
	m_Light = new CLight;
	if( !m_Light )
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetAmbientColor( 0.02f, 0.02f, 0.75f, 1.0f );
	m_Light->SetDiffuseColor( .9f, .9f, 1.0f, 1.0f );
	m_Light->SetDirection( -0.5f, -1.0f, 0.0f );
	// Create the sky dome object.
	m_SkyDome = new CSkyDome;
	if( !m_SkyDome )
	{
		return false;
	}

	// Initialize the sky dome object.
	result = m_SkyDome->Initialize( m_Direct3D->GetDevice() );
	if( !result )
	{
		MessageBox( hwnd, "Could not initialize the sky dome object.", "Error", MB_OK );
		return false;
	}

	// Create the sky dome shader object.
	m_SkydomeShader = new CSkydomeShader;
	if( !m_SkydomeShader )
	{
		return false;
	}

	// Initialize the sky dome shader object.
	result = m_SkydomeShader->Initialize( m_Direct3D->GetDevice(), hwnd );
	if( !result )
	{
		MessageBox( hwnd, "Could not initialize the sky dome shader object.", "Error", MB_OK );
		return false;
	}

	// Create the sky plane object.
	m_SkyPlane = new CSkyplane;
	if( !m_SkyPlane )
	{
		return false;
	}

	// Initialize the sky plane object.
	result = m_SkyPlane->Initialize(m_Direct3D->GetDevice(), "../../data/cloud001.dds", "../../data/perturb001.dds");
	if( !result )
	{
		MessageBox( hwnd, "Could not initialize the sky plane object.", "Error", MB_OK );
		return false;
	}

	// Create the sky plane shader object.
	m_SkyPlaneShader = new CSkyplaneShader;
	if(!m_SkyPlaneShader)
	{
		return false;
	}

	// Initialize the sky plane shader object.
	result = m_SkyPlaneShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the sky plane shader object.", "Error", MB_OK);
		return false;
	}

	// Create the frustum object.
	m_Frustum = new CFrustum;
	if(!m_Frustum)
	{
		return false;
	}

	// Create the quad tree object.
	m_QuadTree = new CQuadTree;
	if(!m_QuadTree)
	{
		return false;
	}

	// Initialize the quad tree object.
	result = m_QuadTree->Initialize(m_Terrain, m_Direct3D->GetDevice());
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the quad tree object.", "Error", MB_OK);
		return false;
	}

	// Construct the frustum.
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);
	
	m_Position->SetRotation( -13, -48, 0 );

	/*m_QuadTreeShader = new CQuadTreeShader;
	if(!m_QuadTreeShader)
	{
		return false;
	}

	result = m_QuadTreeShader->Initialize( m_Direct3D->GetDevice(), hwnd );
	if(!result)
	{
		MessageBox(hwnd, "Could not initailize the quad tree shader object.", "Error", MB_OK);
		return false;
	}*/

	return true;
}


void CApp::Shutdown()
{
	// Release the sky plane shader object.
	if(m_SkyPlaneShader)
	{
		m_SkyPlaneShader->Shutdown();
		delete m_SkyPlaneShader;
		m_SkyPlaneShader = 0;
	}

	// Release the sky plane object.
	if(m_SkyPlane)
	{
		m_SkyPlane->Shutdown();
		delete m_SkyPlane;
		m_SkyPlane = 0;
	}

	// Release the sky dome shader object.
	if(m_SkydomeShader)
	{
		m_SkydomeShader->Shutdown();
		delete m_SkydomeShader;
		m_SkydomeShader = 0;
	}

	// Release the sky dome object.
	if(m_SkyDome)
	{
		m_SkyDome->Shutdown();
		delete m_SkyDome;
		m_SkyDome = 0;
	}

	// Release the light object.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the terrain shader object.
	if(m_TerrainShader)
	{
		m_TerrainShader->Shutdown();
		delete m_TerrainShader;
		m_TerrainShader = 0;
	}

	// Release the text object.
	if(m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	// Release the font shader object.
	if(m_FontShader)
	{
		m_FontShader->Shutdown();
		delete m_FontShader;
		m_FontShader = 0;
	}

	// Release the cpu object.
	if(m_Cpu)
	{
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	// Release the fps object.
	if(m_Fps)
	{
		delete m_Fps;
		m_Fps = 0;
	}

	// Release the position object.
	if(m_Position)
	{
		delete m_Position;
		m_Position = 0;
	}

	// Release the timer object.
	if(m_Timer)
	{
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the terrain object.
	if(m_Terrain)
	{
		m_Terrain->Shutdown();
		delete m_Terrain;
		m_Terrain = 0;
	}

	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the Direct3D object.
	if(m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	// Release the input object.
	if(m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	if(m_Grass)
	{
		//m_Grass->Shutdown();
		delete m_Grass;
		m_Grass  = 0;
	}

	if(m_GrassShader)
	{
		m_GrassShader->Shutdown();
		delete m_GrassShader;
		m_GrassShader = 0;
	}

	return;
}


bool CApp::Frame()
{
	bool result;


	// Read the user input.
	result = m_Input->Frame();
	if(!result)
	{
		return false;
	}
	
	// Check if the user pressed escape and wants to exit the application.
	if(m_Input->IsEscapePressed() == true)
	{
		return false;
	}

	// Update the system stats.
	m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	// Update the FPS value in the text object.
	result = m_Text->SetFps(m_Fps->GetFps(), m_Direct3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}
	
	// Update the CPU usage value in the text object.
	result = m_Text->SetCpu(m_Cpu->GetCpuPercentage(), m_Direct3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	// Do the frame input processing.
	result = HandleInput(m_Timer->GetTime());
	if(!result)
	{
		return false;
	}

	// Do the sky plane frame processing.
	m_SkyPlane->Frame();
	m_GrassShader->Frame();

	// Render the graphics.
	result = RenderGraphics();
	if(!result)
	{
		return false;
	}

	return result;
}


bool CApp::HandleInput(float frameTime)
{
	bool keyDown, result;
	float posX, posY, posZ, rotX, rotY, rotZ;
	int mouseX = 0, mouseY = 0;

	m_Input->GetMouseLocation(mouseX, mouseY);

	// Get the view point position/rotation.
	m_Position->GetPosition(posX, posY, posZ);
	m_Position->GetRotation(rotX, rotY, rotZ);

	// Set the frame time for calculating the updated position.
	m_Position->SetFrameTime(frameTime);

	// Handle the input.
	keyDown = m_Input->IsLeftPressed();
	m_Position->TurnLeft(keyDown, mouseX, mouseY, m_Input->IsLShiftPressed());

	keyDown = m_Input->IsRightPressed();
	m_Position->TurnRight(keyDown, mouseX, mouseY, m_Input->IsLShiftPressed());

	keyDown = m_Input->IsUpPressed();
	m_Position->MoveForward(keyDown, mouseX, mouseY, m_Input->IsLShiftPressed());

	keyDown = m_Input->IsDownPressed();
	m_Position->MoveBackward(keyDown, mouseX, mouseY, m_Input->IsLShiftPressed());

	keyDown = m_Input->IsOnePressed();
	if(keyDown)
		m_TerrainShader->ToggleWireFrame(m_Direct3D->GetDevice());

	keyDown = m_Input->IsZPressed();
	if(keyDown)
		m_updateCQuadTreeRender = !m_updateCQuadTreeRender;


	keyDown = m_Input->IsRPressed();
	m_Position->MoveUpward(keyDown);

	keyDown = m_Input->IsFPressed();
	m_Position->MoveDownward(keyDown);

	keyDown = m_Input->IsPgUpPressed();
	m_Position->LookUpward(keyDown);

	keyDown = m_Input->IsPgDownPressed();
	m_Position->LookDownward(keyDown);

	m_QuadTree->GetHeightAtPointXY(posX, posZ, m_QuadTree->GetRootNode());
	posY = m_QuadTree->GetHeight()+2;

	rotX += mouseY;
	rotY += mouseX;

	// Set the position of the camera.
	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);

	// Update the position values in the text object.
	result = m_Text->SetCameraPosition(posX, posY, posZ, m_Direct3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	// Update the rotation values in the text object.
	result = m_Text->SetCameraRotation(rotX, rotY, rotZ, m_Direct3D->GetDeviceContext());
	if(!result)
	{
		return false;
	}

	return true;
}


bool CApp::RenderGraphics()
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	D3DXVECTOR3 cameraPosition;
	bool result;


	// Clear the scene.
	m_Direct3D->BeginScene( 0.0f, 0.0f, 0.0f, 1.0f );

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	m_Direct3D->GetWorldMatrix( worldMatrix );
	m_Camera->GetViewMatrix( viewMatrix );
	m_Direct3D->GetProjectionMatrix( projectionMatrix );
	m_Direct3D->GetOrthoMatrix( orthoMatrix );

	// Get the position of the camera.
	cameraPosition = m_Camera->GetPosition();

	// Translate the sky dome to be centered around the camera position.
	D3DXMatrixTranslation( &worldMatrix, cameraPosition.x, cameraPosition.y, cameraPosition.z );

	// Turn off back face culling.
	m_Direct3D->TurnOffCulling();

	// Turn off the Z buffer.
	m_Direct3D->TurnZBufferOff();

	// Render the sky dome using the sky dome shader.
	m_SkyDome->Render(m_Direct3D->GetDeviceContext());
	m_SkydomeShader->Render(m_Direct3D->GetDeviceContext(), m_SkyDome->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
							m_SkyDome->GetApexColor(), m_SkyDome->GetCenterColor());

	// Turn back face culling back on.
	m_Direct3D->TurnOnCulling();

	// Enable additive blending so the clouds blend with the sky dome color.
	m_Direct3D->EnableSecondBlendState();

	// Render the sky plane using the sky plane shader.
	m_SkyPlane->Render(m_Direct3D->GetDeviceContext());
	m_SkyPlaneShader->Render(m_Direct3D->GetDeviceContext(), m_SkyPlane->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
							 m_SkyPlane->GetCloudTexture(), m_SkyPlane->GetPerturbTexture(), m_SkyPlane->GetTranslation(), m_SkyPlane->GetScale(), 
							 m_SkyPlane->GetBrightness());

	// Turn the Z buffer back on.
	m_Direct3D->TurnZBufferOn();

	// Reset the world matrix.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	

	// Construct the frustum.
	m_Frustum->ConstructFrustum(SCREEN_DEPTH, projectionMatrix, viewMatrix);
		
	D3DXVECTOR3 cameraDirection = m_Camera->GetRotation();
	D3DXVECTOR3 cameraLocation = D3DXVECTOR3( m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z );

	// Render the terrain using the quad tree and terrain shader.
	m_QuadTree->Render(m_Frustum, m_Direct3D->GetDeviceContext(), m_TerrainShader);
	m_QuadTree->SetIsRenderUpdate(m_updateCQuadTreeRender);
	m_QuadTree->RenderScene(m_Direct3D->GetDeviceContext(), 
								m_Direct3D,
								m_Terrain,
								m_TerrainShader,
								m_Grass, 
								m_GrassShader,
								worldMatrix, 
								viewMatrix, 
								projectionMatrix,
								cameraDirection, 
								cameraLocation,
								m_Cpu->GetCpuPercentage(),
								m_Light );


	
	//m_QuadTree->RenderTreeNodes( m_Direct3D->GetDeviceContext() );
	//m_QuadTreeShader->Render(m_Direct3D->GetDeviceContext(), m_QuadTree->GetQuadTreeIndexCount(), worldMatrix, viewMatrix, projectionMatrix);


	m_Direct3D->TurnOnCulling();

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();
		
	// Turn on the alpha blending before rendering the text.
	m_Direct3D->TurnOnAlphaBlending();

	// Render the text user interface elements.
	result = m_Text->Render(m_Direct3D->GetDeviceContext(), m_FontShader, worldMatrix, orthoMatrix);
	if(!result)
	{
		return false;
	}

	// Turn off alpha blending after rendering the text.
	m_Direct3D->TurnOffAlphaBlending();

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}
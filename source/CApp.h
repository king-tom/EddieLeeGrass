#pragma once

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 500.0f;
const float SCREEN_NEAR = 0.1f;
const int NUMBER_OF_BLADES = 1000;			// Per-patch


#include "CInput.h"
#include "CDirect3DSystem.h"
#include "CCamera.h"
#include "CTerrain.h"
#include "CTimer.h"
#include "CPosition.h"
#include "CFPS.h"
#include "CCPU.h"
#include "CFontShader.h"
#include "CText.h"
#include "CTerrainShader.h"
#include "CLight.h"
#include "CSkydome.h"
#include "CSkydomeShader.h"
#include "CSkyplane.h"
#include "CSkyplaneShader.h"
#include "CFrustum.h"
#include "CGrass.h"
#include "CGrassShader.h"
#include "CQuadTree.h"
#include "CQuadTreeShader.h"


class CApp
{
public:
	CApp();
	CApp( const CApp& );
	~CApp();

	bool Initialize( HINSTANCE, HWND, int, int );
	void Shutdown();
	bool Frame();

private:
	bool HandleInput( float );
	bool RenderGraphics();
	bool m_updateCQuadTreeRender;

private:
	CClass* m_Input;
	CDirect3DSystem* m_Direct3D;
	CCamera* m_Camera;
	CTerrain* m_Terrain;
	CTimer* m_Timer;
	CPosition* m_Position;
	CFPS* m_Fps;
	CCPU* m_Cpu;
	CFontShader* m_FontShader;
	CText* m_Text;
	CTerrainShader* m_TerrainShader;
	CLight* m_Light;
	CSkyDome* m_SkyDome;
	CSkydomeShader* m_SkydomeShader;
	CSkyplane *m_SkyPlane;
	CSkyplaneShader* m_SkyPlaneShader;
	CFrustum *m_Frustum;
	CGrass *m_Grass;
	CGrassShader *m_GrassShader;
	CQuadTree *m_QuadTree;
	CQuadTreeShader *m_QuadTreeShader;
};


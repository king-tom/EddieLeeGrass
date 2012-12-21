#pragma once

#include <windows.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <xnamath.h>
#include <stdio.h>
#include <vector>

#include "CTexture.h"
#include "CPerlinNoise.h"
#include "CFrustum.h"

#define TEXTURE_REPEAT 8
#define HEIGHT_MULTIPLIER .1
#define TERRAIN_CELL_WIDTH 8

class CTerrain
{
private:

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	    D3DXVECTOR3 normal;
		D3DXVECTOR4 color;
	};

	struct aabbData
	{
		std::vector<XMFLOAT3> vertPositions;			// positions: Six verts per quad
		std::vector<VertexType> vertData;				// positions, normals, etc
		std::vector<XMFLOAT3> aabbMinMax;				// 2 3-vectors per aabb min/max
	};

	struct HeightMapType 
	{ 
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
		float r, g, b;
	};

	struct VectorType 
	{ 
		float x, y, z;
	};

public:
	CTerrain();
	CTerrain(const CTerrain&);
	~CTerrain();

	bool Initialize(ID3D11Device*, char*, char *, char*);
	void Shutdown();
	//void Render(ID3D11DeviceContext*, XMMATRIX&);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

	float GetHeightAtPointXY(float x, float y);
	int GetVertexCount();
	void CopyVertexArray(void*);
	ID3D11Buffer *GetIndexBuffer(){return m_indexBuffer;}
	ID3D11Buffer *const*GetVertexBuffer(){return &m_vertexBuffer;}
	HeightMapType *GetHeightMap();

private:
	bool LoadHeightMap(char*);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void ShutdownHeightMap();

	void CalculateTextureCoordinates();
	bool LoadTexture(ID3D11Device*, char *);
	void ReleaseTexture();

	bool LoadColorMap(char*);

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	//void RenderBuffers(ID3D11DeviceContext*, XMMATRIX&);
	
	void CTerrain::cullAABB(std::vector<XMFLOAT4> &frustumPlanes);

private:
	std::vector<VertexType> culledTerrainData;

	std::vector<aabbData> terrainAABB;
	int m_terrainWidth, m_terrainHeight;
	int m_vertexCount, m_indexCount;
	VertexType * m_vertices;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	HeightMapType* m_heightMap;
	CTexture* m_Texture;
};
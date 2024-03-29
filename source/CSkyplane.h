#pragma once

#include <d3d11.h>
#include <d3dx10math.h>


#include "CTexture.h"


class CSkyplane
{
private:
	struct SkyPlaneType
	{
		float x, y, z;
		float tu, tv;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	CSkyplane();
	CSkyplane(const CSkyplane&);
	~CSkyplane();

	bool Initialize(ID3D11Device*, char *, char *);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	void Frame();

	int GetIndexCount();
	ID3D11ShaderResourceView* GetCloudTexture();
	ID3D11ShaderResourceView* GetPerturbTexture();
	
	float GetScale();
	float GetBrightness();
	float GetTranslation();

private:
	bool InitializeSkyPlane(int, float, float, float, int);
	void ShutdownSkyPlane();

	bool InitializeBuffers(ID3D11Device*, int);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTextures(ID3D11Device*, char *, char *);
	void ReleaseTextures();

private:
	SkyPlaneType* m_skyPlane;
	int m_vertexCount, m_indexCount;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	CTexture *m_CloudTexture, *m_PerturbTexture;
	float m_scale, m_brightness, m_translation;
};
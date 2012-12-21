#pragma once

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")


#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <D3DX11async.h>

#include "CDirect3DSystem.h"

class CDepthOfField
{
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR4 color;
	};

	CDepthOfField( CDirect3DSystem* );
	~CDepthOfField();

	HRESULT SetupFullScreenQuadVertexBuffer( ID3D11Device* ){HRESULT hr; return hr;}
	void Frame( ID3D11Device*   pd3dDevice, 
					DOUBLE          fTime, 
					FLOAT           fElapsedTime, 
					void*           pUserContext,
					D3DXMATRIX,
					D3DXMATRIX,
					D3DXMATRIX );
	void Render( ID3D11Device*   pd3dDevice, 
				DOUBLE          fTime, 
				FLOAT           fElapsedTime, 
				void*           pUserContext,
				D3DXMATRIX,
				D3DXMATRIX,
				D3DXMATRIX );

	bool Initialize( ID3D11Device* );
	bool InitializeBuffers( ID3D11Device* );
	bool InitializeShaders( ID3D11Device* device );

private:
	ID3D11Buffer* m_FullScreenVertexBuffer;
	ID3D11Texture2D* m_ColorTexture;
	ID3D11Texture2D* m_MSAAColorTexture;
	ID3D11RenderTargetView* m_ColorRTView;
	ID3D11ShaderResourceView* m_ColorSRView;
	ID3D11RenderTargetView* m_MSAAColorRTView;
	ID3D11ShaderResourceView* m_MSAAColorSRView;
	ID3D11Texture2D* m_DepthStencilTexture;
	ID3D11DepthStencilView* m_DepthStencilDSView;
	ID3D11ShaderResourceView* m_DepthStencilSRView;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_VertexLayout;

	CDirect3DSystem* m_d3d;
};
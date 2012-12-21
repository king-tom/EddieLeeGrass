#pragma once

#include <d3d11.h>
#include <d3d11Shader.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;


class CTerrainShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		D3DXVECTOR3 cameraDirection;
		float padding[2];
	};

public:
	CTerrainShader();
	CTerrainShader(const CTerrainShader&);
	~CTerrainShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	void ToggleWireFrame(ID3D11Device* device);
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, ID3D11ShaderResourceView*, D3DXVECTOR3);
	void RenderShader(ID3D11DeviceContext*, int);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, ID3D11ShaderResourceView*, D3DXVECTOR3 &);

private:
	bool InitializeShader(ID3D11Device*, HWND, char *, char *);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, char *);

	

private:
	//ID3D11RasterizerState* pRSwireFrame; 
	D3D11_RASTERIZER_DESC RSWireFrameDesc;
	ID3D11RasterizerState *m_rasterizerState;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;

	bool m_wireFrame;
};

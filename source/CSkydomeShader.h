#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;


class CSkydomeShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};

	struct GradientBufferType				// TODO figure out how to set multiple constant buffers without overwriting memory
	{
		D3DXVECTOR4 apexColor;
		D3DXVECTOR4 centerColor;
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float padding;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR3 lightDirection;
		float padding;
	};

public:
	CSkydomeShader();
	CSkydomeShader(const CSkydomeShader&);
	~CSkydomeShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR4, D3DXVECTOR4);

private:
	bool InitializeShader(ID3D11Device*, HWND, char *, char *);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, char *);

	bool SetShaderParameters(ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR4, D3DXVECTOR4);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11SamplerState* m_sampleState;
	ID3D11ShaderResourceView* m_skydomeTexture;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_gradientBuffer;
	ID3D11Buffer* m_lightBuffer;
};
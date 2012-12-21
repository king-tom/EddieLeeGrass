#pragma once

#include <d3d11.h>
#include <d3d11Shader.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;


class CQuadTreeShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
	};


public:
	CQuadTreeShader();
	CQuadTreeShader(const CQuadTreeShader&);
	~CQuadTreeShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	void ToggleWireFrame(ID3D11Device* device);
	bool Render(ID3D11DeviceContext*, int, 
									   D3DXMATRIX worldMatrix, 
									   D3DXMATRIX viewMatrix,
									   D3DXMATRIX projectionMatrix);
	void RenderShader(ID3D11DeviceContext*, int);

	bool SetShaderParameters(ID3D11DeviceContext*, 
									   D3DXMATRIX, 
									   D3DXMATRIX,
									   D3DXMATRIX );

private:
	bool InitializeShader(ID3D11Device*, HWND, char *, char *, char*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, char *);

	

private:

	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11GeometryShader* m_geometryShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;

	bool m_wireFrame;
};

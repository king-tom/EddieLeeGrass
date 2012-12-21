#pragma once

#include <d3d11.h>
#include <d3d11Shader.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>
using namespace std;


class CGrassShader
{
private:
	struct MatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		D3DXVECTOR4 HeightList;
		D3DXVECTOR3 CameraLocation;
		float padding;
		D3DXVECTOR3 TerrainOffset;
		float rand;
		float time;
		D3DXVECTOR3 padding2;
	};

	struct LightBufferType
	{
		D3DXVECTOR4 ambientColor;
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR4 lightDirection;
		D3DXVECTOR3 cameraDirection;
		float rand;
		D3DXVECTOR3 cameraLocation;
		float padding1;
	};


public:
	CGrassShader();
	CGrassShader( const CGrassShader& );
	~CGrassShader();

	bool Initialize( ID3D11Device*, HWND );
	void Shutdown();
	void ToggleWireFrame(ID3D11Device* device);
	bool Render( ID3D11DeviceContext*, int, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR3, unsigned long );
	void RenderShader( ID3D11DeviceContext*, int );
	void Frame();

	bool SetShaderParameters( ID3D11DeviceContext*, D3DXMATRIX, D3DXMATRIX, D3DXMATRIX, D3DXVECTOR4, D3DXVECTOR4, D3DXVECTOR3, D3DXVECTOR3, D3DXVECTOR4, D3DXVECTOR3, unsigned long );

private:
	bool InitializeShader( ID3D11Device*, HWND, char *, char * , char *);
	void ShutdownShader();
	void OutputShaderErrorMessage( ID3D10Blob*, HWND, char * );

private:
	//ID3D11RasterizerState* pRSwireFrame; 

	ID3D11ShaderResourceView* m_grassTexture;
	ID3D11ShaderResourceView* m_grassAlphaTexture;
	D3D11_RASTERIZER_DESC RSWireFrameDesc;
	ID3D11RasterizerState *m_rasterizerState;
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11GeometryShader *m_geometryShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11SamplerState* m_sampleStateAlpha;
	ID3D11BlendState *m_blendState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	double m_Time;

	bool m_wireFrame;
};
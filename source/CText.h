#pragma once

#include "CFont.h"
#include "CFontShader.h"


class CText
{
private:
	struct SentenceType
	{
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		int vertexCount, indexCount, maxLength;
		float red, green, blue;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
	    D3DXVECTOR2 texture;
	};

public:
	CText();
	CText(const CText&);
	~CText();

	bool Initialize(ID3D11Device*, ID3D11DeviceContext*, HWND, int, int, D3DXMATRIX);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, CFontShader*, D3DXMATRIX, D3DXMATRIX);

	bool SetVideoCardInfo(char*, int, ID3D11DeviceContext*);
	bool SetFps(int, ID3D11DeviceContext*);
	bool SetCpu(int, ID3D11DeviceContext*);
	bool SetCameraPosition(float, float, float, ID3D11DeviceContext*);
	bool SetCameraRotation(float, float, float, ID3D11DeviceContext*);

private:
	bool InitializeSentence(SentenceType**, int, ID3D11Device*);
	bool UpdateSentence(SentenceType*, char*, int, int, float, float, float, ID3D11DeviceContext*);
	void ReleaseSentence(SentenceType**);
	bool RenderSentence(SentenceType*, ID3D11DeviceContext*, CFontShader*, D3DXMATRIX, D3DXMATRIX);

private:
	int m_screenWidth, m_screenHeight;
	D3DXMATRIX m_baseViewMatrix;
	CFont* m_Font;
	SentenceType *m_sentence1, *m_sentence2, *m_sentence3, *m_sentence4, *m_sentence5;
	SentenceType *m_sentence6, *m_sentence7, *m_sentence8, *m_sentence9, *m_sentence10;
};
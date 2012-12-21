#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>
using namespace std;


#include "CTexture.h"


class CFont
{
private:
	struct FontType
	{
		float left, right;
		int size;
	};

	struct VertexType
	{
		D3DXVECTOR3 position;
	    D3DXVECTOR2 texture;
	};

public:
	CFont();
	CFont(const CFont&);
	~CFont();

	bool Initialize(ID3D11Device*, char*, char *);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

	void BuildVertexArray(void*, char*, float, float);

private:
	bool LoadFontData(char*);
	void ReleaseFontData();
	bool LoadTexture(ID3D11Device*, char *);
	void ReleaseTexture();

private:
	FontType* m_Font;
	CTexture* m_Texture;
};
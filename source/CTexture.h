#pragma once

#include <d3d11.h>
#include <d3dx11tex.h>


class CTexture
{
public:
	CTexture();
	CTexture(const CTexture&);
	~CTexture();

	bool Initialize(ID3D11Device*, char *);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11ShaderResourceView* m_texture;
};
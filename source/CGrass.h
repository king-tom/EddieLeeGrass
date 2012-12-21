#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include <D3D11.h>

#include "d3dx11effect.h"
#include <d3dx10math.h>

class CGrass
{

public:

	struct VertexType
	{
		D3DXVECTOR3 position;
	};

	CGrass::CGrass( const int );

	CGrass::CGrass( const CGrass& other );

	CGrass::CGrass( int quadSize, int numberOfBlades, int fieldSize ) 
		: m_quadSize	( quadSize ), 
		m_numberOfBlades( numberOfBlades ),
		m_fieldSize		( fieldSize ) { }

	CGrass::~CGrass(){}

	bool InitializeBuffers( ID3D11Device *device );
	bool Initialize( ID3D11Device* device );
	void CopyVertexArray( void* vertexList );
	void ReleaseBuffers();
	void Shutdown();
	void Render( ID3D11DeviceContext* deviceContext );
	void RenderBuffers( ID3D11DeviceContext* deviceContext );

	ID3D11Buffer *GetIndexBuffer() { return m_indexBuffer; }
	ID3D11Buffer *GetVertexBuffer(){ return m_vertexBuffer; }
	
	int GetIndexCount() { return m_indexCount; }
	int GetVertexCount(){ return m_vertexCount; }
	int GetStride()		{ return m_stride; }
	int GetOffset()		{ return m_offset; }

private:
	
	const int m_numberOfBlades;
	int m_quadSize;
	int m_fieldSize;
	int m_vertexCount;
	int m_indexCount;
	int m_stride;
	int m_offset;


	VertexType *m_vertices;
	ID3D11Buffer *m_vertexBuffer;
	ID3D11Buffer *m_indexBuffer;
};

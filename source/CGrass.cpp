////////////////////////////////////////////////////////////////////////////////
// Filename: CSkydome.cpp
////////////////////////////////////////////////////////////////////////////////
#include "CGrass.h"


CGrass::CGrass(const int numberOfBlades) : m_numberOfBlades( numberOfBlades )
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}


CGrass::CGrass(const CGrass& other) : m_numberOfBlades( 0 ) { }

bool CGrass::Initialize( ID3D11Device* device )
{
	bool result;
	/*
	// Calculate the normals for the terrain data.
	result = CalculateNormals();
	if(!result)
	{
		return false;
	}

	// Calculate the texture coordinates.
	CalculateTextureCoordinates();

	// Load the texture.
	result = LoadTexture(device, textureFilename);
	if(!result)
	{
	//	return false;
	}

	*/

	// Initialize the vertex and index buffer that hold the geometry for the terrain.
	result = InitializeBuffers( device );
	if(!result)
	{
		return false;
	}

	return true;

}

bool CGrass::InitializeBuffers( ID3D11Device *device )
{
	m_vertices = new VertexType[m_numberOfBlades];
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	unsigned long *indices = new unsigned long[m_numberOfBlades];
	HRESULT result;

	m_vertexCount = 0;

	srand( 37 );

	for( ; m_vertexCount < m_numberOfBlades - 1; )
	{
		D3DXVECTOR3 vec = D3DXVECTOR3( ( ( RAND_MAX - (float)rand() ) / RAND_MAX ) * 8 , 0.0,  ( ( RAND_MAX - (float)rand() ) / RAND_MAX ) * 8 );
		m_vertices[m_vertexCount].position = vec;
		indices[m_vertexCount] = m_vertexCount;
		m_vertexCount++;
	}

	//m_vertexCount++;
	//m_vertexCount /= 3;

		// Set up the description of the static vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_numberOfBlades;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
    result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		return false;
	}

	m_indexCount = m_numberOfBlades;

	// Set up the description of the static index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer( &indexBufferDesc, &indexData, &m_indexBuffer );
	if(FAILED(result))
	{
		return false;
	}

	//delete m_vertices;
	delete indices;

	return true;
}


void CGrass::CopyVertexArray( void* vertexList )
{
	memcpy( vertexList, m_vertices , sizeof(VertexType) * m_vertexCount );
	return;
}


void CGrass::Shutdown()
{
	// Release the vertex and index buffer that were used for rendering the grass.
	ReleaseBuffers();

	return;
}


void CGrass::Render(ID3D11DeviceContext* deviceContext)
{
	// Render the grass.
	RenderBuffers(deviceContext);

	return;
}


void CGrass::ReleaseBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	if(m_vertices)
	{
		m_vertexBuffer->Release();
		m_vertices = 0;
	}

	return;
}


void CGrass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
    stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case points.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	return;
}
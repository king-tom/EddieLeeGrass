#include "CQuadTree.h"


CQuadTree::CQuadTree()
{
	m_vertexList = 0;
	m_parentNode = 0;
	m_nodeVertices = 0;
	m_nodeIndices = 0;
	m_numberOfNodeVertices = -1;
	
	num = 0.0f;
}


CQuadTree::CQuadTree(const CQuadTree& other)
{
}


CQuadTree::~CQuadTree()
{
}


bool CQuadTree::Initialize(CTerrain* terrain, ID3D11Device* device)
{
	int vertexCount;
	float centerX, centerZ, width;

	
	pn = new CPerlinNoise(4.2f, .0037f, 0.7f, 4, 8);

	// Get the number of vertices in the terrain vertex array.
	vertexCount = terrain->GetVertexCount();

	// Store the total triangle count for the vertex list.
	m_triangleCount = terrain->GetVertexCount() / 3;

	// Create a vertex array to hold all of the terrain vertices.
	m_vertexList = new VertexType[vertexCount];
	if(!m_vertexList)
	{
		return false;
	}

	m_nodeVertices = new NodeVertexType[vertexCount];
	if(!m_nodeVertices)
	{
		return false;
	}

	m_nodeIndices = new unsigned long[vertexCount];

	// Copy the terrain vertices into the vertex list.
	terrain->CopyVertexArray((void*)m_vertexList);
	m_Terrain = terrain;
	// Calculate the center x,z and the width of the mesh.
	CalculateMeshDimensions(vertexCount, centerX, centerZ, width);

	// Create the parent node for the quad tree.
	m_parentNode = new NodeType;
	if(!m_parentNode)
	{
		return false;
	}

	// Recursively build the quad tree based on the vertex list data and mesh dimensions.
	CreateTreeNode(m_parentNode, centerX+4, centerZ+4, width+8, device);

	// Build tree node buffers
	//InitializeTreeNodeBuffers( device );

	// Release the vertex list since the quad tree now has the vertices in each node.
	if(m_vertexList)
	{
		delete [] m_vertexList;
		m_vertexList = 0;
	}

	return true;
}


void CQuadTree::Shutdown()
{
	// Recursively release the quad tree data.
	if(m_parentNode)
	{
		ReleaseNode(m_parentNode);
		delete m_parentNode;
		m_parentNode = 0;
	}

	return;
}


void CQuadTree::Render(CFrustum* frustum, ID3D11DeviceContext* deviceContext, CTerrainShader* shader)
{
	if(m_renderUpdate == false)
		return;
	// Reset the number of triangles that are drawn for this frame.
	m_drawCount = 0;

	// Render each node that is visible starting at the parent node and moving down the tree.
	RenderNode(m_parentNode, frustum, deviceContext, shader);

	return;
}


int CQuadTree::GetDrawCount()
{
	return m_drawCount;
}


void CQuadTree::CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth)
{
	int i;
	float maxWidth, maxDepth, minWidth, minDepth, width, depth, maxX, maxZ;


	// Initialize the center position of the mesh to zero.
	centerX = 0.0f;
	centerZ = 0.0f;

	// Sum all the vertices in the mesh.
	for(i=0; i<vertexCount; i++)
	{
		centerX += m_vertexList[i].position.x;
		centerZ += m_vertexList[i].position.z;
	}

	// And then divide it by the number of vertices to find the mid-point of the mesh.
	centerX = centerX / (float)vertexCount;
	centerZ = centerZ / (float)vertexCount;

	// Initialize the maximum and minimum size of the mesh.
	maxWidth = 0.0f;
	maxDepth = 0.0f;

	minWidth = fabsf(m_vertexList[0].position.x - centerX);
	minDepth = fabsf(m_vertexList[0].position.z - centerZ);

	// Go through all the vertices and find the maximum and minimum width and depth of the mesh.
	for(i=0; i<vertexCount; i++)
	{
		width = fabsf(m_vertexList[i].position.x - centerX);	
		depth = fabsf(m_vertexList[i].position.z - centerZ);	

		if(width > maxWidth) { maxWidth = width; }
		if(depth > maxDepth) { maxDepth = depth; }
		if(width < minWidth) { minWidth = width; }
		if(depth < minDepth) { minDepth = depth; }
	}

	// Find the absolute maximum value between the min and max depth and width.
	maxX = (float)max(fabs(minWidth), fabs(maxWidth));
	maxZ = (float)max(fabs(minDepth), fabs(maxDepth));
	
	// Calculate the maximum diameter of the mesh.
	meshWidth = max(maxX, maxZ) * 2.0f;

	return;
}


void CQuadTree::InitializeTreeNodeBuffers( ID3D11Device* device )
{

	bool result;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Set up the description of the vertex buffer.
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(NodeVertexType) * m_numberOfNodeVertices;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
    vertexData.pSysMem = m_nodeVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
    device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_nodeVerticesBuffer);

	// Set up the description of the index buffer.
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_numberOfNodeVertices;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
    indexData.pSysMem = m_nodeIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &m_nodeIndicesBuffer);

	// Release the vertex and index arrays now that the data is stored in the buffers in the node.
	delete [] m_nodeVertices;
	m_nodeVertices = 0;

	delete [] m_nodeIndices;
	m_nodeIndices = 0;

	return;
}


//  Render the quadtree points for debugging
void CQuadTree::RenderTreeNodes( ID3D11DeviceContext* deviceContext )
{
	unsigned int stride;
	unsigned int offset;


	// Set vertex buffer stride and offset.
    stride = sizeof(NodeVertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_nodeVerticesBuffer, &stride, &offset);

    // Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_nodeIndicesBuffer, DXGI_FORMAT_R32_UINT, 0);

    // Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	return;
}

void CQuadTree::CollectNodeData( NodeType* node, int positionX, int positionZ, float width, ID3D11Device* device )
{
	int numTriangles, i, count, vertexCount, index, vertexIndex;
	float offsetX, offsetZ;
	VertexType* vertices;
	unsigned long* indices;
	bool result;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	

			node->nodes[0] = NULL;
			node->nodes[1] = NULL;
			node->nodes[2] = NULL;
			node->nodes[3] = NULL;
			
			// Case 3: If this node is not empty and the triangle count for it is less than the max then 
			// this node is at the bottom of the tree so create the list of triangles to store in it.
			//node->triangleCount = 2;//numTriangles;

			// Calculate the number of vertices.
			vertexCount = 2*3*4;//numTriangles * 3;

			// Create the vertex array.
			vertices = new VertexType[vertexCount];

			// Create the index array.
			indices = new unsigned long[vertexCount];

			// Initialize the index for this new vertex and index array.
			index = 0;

			// Vert 1 Lower Left Quadrant
			vertices[index].position = D3DXVECTOR3( positionX - width / 2.0f, 
				pn->GetHeight( positionX - width / 2.0f, positionZ ), 
				positionZ );
			vertices[index].texture = D3DXVECTOR2( 1, 1 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			node->heightList._11 = vertices[index].position.y;
			indices[index] = index;
			index++;
			vertices[index].position = D3DXVECTOR3( positionX, 
				pn->GetHeight( positionX, positionZ ), 
				positionZ );
			vertices[index].texture = D3DXVECTOR2( 1, 0 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			node->heightList._12 = vertices[index].position.y;
			indices[index] = index;
			index++;
			vertices[index].position = D3DXVECTOR3( positionX - width / 2.0f, 
				pn->GetHeight( positionX - width / 2.0f, positionZ - width / 2.0f ), 
				positionZ - width / 2.0f );
			vertices[index].texture = D3DXVECTOR2( 0, 1 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			node->heightList._13 = vertices[index].position.y;
			indices[index] = index;
			index++;

			//Vert 2 LL Quadrant
			vertices[index].position = D3DXVECTOR3( positionX - width / 2.0f, 
				pn->GetHeight(positionX - width / 2.0f, positionZ - width / 2.0f ), 
				positionZ - width / 2.0f );
			vertices[index].texture = D3DXVECTOR2( 0, 1 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			indices[index] = index;
			index++;
			vertices[index].position = D3DXVECTOR3( positionX, 
				pn->GetHeight( positionX, positionZ ), 
				positionZ );
			vertices[index].texture = D3DXVECTOR2( 1, 0 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			indices[index] = index;
			index++;
			vertices[index].position = D3DXVECTOR3( positionX, 
				pn->GetHeight(positionX, positionZ - width / 2.0f ), 
				positionZ - width / 2.0f );
			vertices[index].texture = D3DXVECTOR2( 0, 0 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			node->heightList._14 = vertices[index].position.y;
			indices[index] = index;
			index++;



			//Vert UL Quadrant
			vertices[index].position = D3DXVECTOR3( positionX, 
				pn->GetHeight( positionX, positionZ ), 
				positionZ );
			vertices[index].texture = D3DXVECTOR2( 1, 1 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			node->heightList._21 = vertices[index].position.y;
			indices[index] = index;
			index++;
			vertices[index].position = D3DXVECTOR3( positionX + width / 2.0f, 
				pn->GetHeight( positionX + width / 2.0f, positionZ ), 
				positionZ );
			vertices[index].texture = D3DXVECTOR2( 1, 0 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			node->heightList._22 = vertices[index].position.y;
			indices[index] = index;
			index++;
			vertices[index].position = D3DXVECTOR3( positionX, 
				pn->GetHeight( positionX, positionZ - width / 2.0f ), 
				positionZ - width / 2.0f );
			vertices[index].texture = D3DXVECTOR2( 0, 1 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			node->heightList._23 = vertices[index].position.y;
			indices[index] = index;
			index++;

			//Vert UL Quadrant
			vertices[index].position = D3DXVECTOR3( positionX, 
				pn->GetHeight( positionX, positionZ - width / 2.0f ), 
				positionZ - width / 2.0f );
			vertices[index].texture = D3DXVECTOR2( 0, 1 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			indices[index] = index;
			index++;
			vertices[index].position = D3DXVECTOR3( positionX + width / 2.0f, 
				pn->GetHeight(positionX + width / 2.0f, positionZ ), 
				positionZ);
			vertices[index].texture = D3DXVECTOR2( 1, 0 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			indices[index] = index;
			index++;
			vertices[index].position = D3DXVECTOR3( positionX + width / 2.0f, 
				pn->GetHeight(positionX + width / 2.0f, positionZ - width / 2.0f ), 
				positionZ - width / 2.0f );
			vertices[index].texture = D3DXVECTOR2( 0, 0 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			node->heightList._24 = vertices[index].position.y;
			indices[index] = index;
			index++;



			//Vert UR Quadrant
			vertices[index].position = D3DXVECTOR3( positionX, 
				pn->GetHeight(positionX, positionZ + width / 2.0f ), 
				positionZ + width / 2.0f );
			vertices[index].texture = D3DXVECTOR2( 1, 1 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			node->heightList._31 = vertices[index].position.y;
			indices[index] = index;
			index++;
			vertices[index].position = D3DXVECTOR3( positionX + width / 2.0f, 
				pn->GetHeight(positionX + width / 2.0f, positionZ + width / 2.0f ), 
				positionZ + width / 2.0f );
			vertices[index].texture = D3DXVECTOR2( 1, 0 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			node->heightList._32 = vertices[index].position.y;
			indices[index] = index;
			index++;
			vertices[index].position = D3DXVECTOR3( positionX, 
				pn->GetHeight(positionX, positionZ ), 
				positionZ );
			vertices[index].texture = D3DXVECTOR2( 0, 1 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			node->heightList._33 = vertices[index].position.y;
			indices[index] = index;
			index++;

			//Vert UR Quadrant
			vertices[index].position = D3DXVECTOR3( positionX, 
				pn->GetHeight(positionX, positionZ ), 
				positionZ );
			vertices[index].texture = D3DXVECTOR2( 0, 1 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			indices[index] = index;
			index++;
			vertices[index].position = D3DXVECTOR3( positionX + width / 2.0f, 
				pn->GetHeight(positionX + width / 2.0f, positionZ + width / 2.0f ), 
				positionZ + width / 2.0f );
			vertices[index].texture = D3DXVECTOR2( 1, 0 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			indices[index] = index;
			index++;
			vertices[index].position = D3DXVECTOR3( positionX + width / 2.0f, 
				pn->GetHeight(positionX + width / 2.0f, positionZ ), 
				positionZ );
			vertices[index].texture = D3DXVECTOR2( 0, 0 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			node->heightList._34 = vertices[index].position.y;
			indices[index] = index;
			index++;
						
			

			//Vert LR Quadrant
			vertices[index].position = D3DXVECTOR3( positionX - width / 2.0f, 
				pn->GetHeight(positionX - width / 2.0f, positionZ + width / 2.0f ), 
				positionZ + width / 2.0f );
			vertices[index].texture = D3DXVECTOR2( 1, 1 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			node->heightList._41 = vertices[index].position.y;
			indices[index] = index;
			index++;
			vertices[index].position = D3DXVECTOR3( positionX, 
				pn->GetHeight(positionX, positionZ + width / 2.0f ), 
				positionZ + width / 2.0f );
			vertices[index].texture = D3DXVECTOR2( 1, 0 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			node->heightList._42 = vertices[index].position.y;
			indices[index] = index;
			index++;
			vertices[index].position = D3DXVECTOR3( positionX - width / 2.0f , 
				pn->GetHeight(positionX - width / 2.0f , positionZ ), 
				positionZ );
			vertices[index].texture = D3DXVECTOR2( 0, 1 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			node->heightList._43 = vertices[index].position.y;
			indices[index] = index;
			index++;

			//Vert 6 LR Quadrant
			vertices[index].position = D3DXVECTOR3( positionX - width / 2.0f, 
				pn->GetHeight(positionX - width / 2.0f, positionZ), 
				positionZ );
			vertices[index].texture = D3DXVECTOR2( 0, 1 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			indices[index] = index;
			index++;
			vertices[index].position = D3DXVECTOR3( positionX, 
				pn->GetHeight(positionX, positionZ + width / 2.0f ), 
				positionZ + width / 2.0f );
			vertices[index].texture = D3DXVECTOR2( 1, 0 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			indices[index] = index;
			index++;
			vertices[index].position = D3DXVECTOR3( positionX, 
				pn->GetHeight(positionX, positionZ ), 
				positionZ );
			vertices[index].texture = D3DXVECTOR2( 0, 0 );
			vertices[index].normal = D3DXVECTOR3(0,0,0);
			vertices[index].color = D3DXVECTOR4(0,0,0,0);
			node->heightList._44 = vertices[index].position.y;
			indices[index] = index;
			index++;

			// Set up the description of the vertex buffer.
			vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = 0;
			vertexBufferDesc.MiscFlags = 0;
			vertexBufferDesc.StructureByteStride = 0;

			// Give the subresource structure a pointer to the vertex data.
			vertexData.pSysMem = vertices;
			vertexData.SysMemPitch = 0;
			vertexData.SysMemSlicePitch = 0;

			// Now finally create the vertex buffer.
			device->CreateBuffer(&vertexBufferDesc, &vertexData, &node->vertexBuffer);

			// Set up the description of the index buffer.
			indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBufferDesc.ByteWidth = sizeof(unsigned long) * vertexCount;
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.MiscFlags = 0;
			indexBufferDesc.StructureByteStride = 0;

			// Give the subresource structure a pointer to the index data.
			indexData.pSysMem = indices;
			indexData.SysMemPitch = 0;
			indexData.SysMemSlicePitch = 0;

			// Create the index buffer.
			device->CreateBuffer(&indexBufferDesc, &indexData, &node->indexBuffer);

			delete [] vertices;
			vertices = 0;

			delete [] indices;
			indices = 0;

			//m_CQuadTreeScene.insert( m_CQuadTreeScene.begin(), node);
			
}

void CQuadTree::CreateTreeNode(NodeType* node, float positionX, float positionZ, float width, ID3D11Device* device)
{
	int numTriangles, i, count, vertexCount, index, vertexIndex;
	float offsetX, offsetZ;
	VertexType* vertices;
	unsigned long* indices;
	bool result;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData, indexData;
	
	// Store the node position and size.
	node->positionX = positionX;
	node->positionZ = positionZ;
	node->width = width;

	// Initialize the vertex and index buffer to null.
	node->vertexBuffer = 0;
	node->indexBuffer = 0;

	// Initialize the children nodes of this node to null.
	node->nodes[0] = 0;
	node->nodes[1] = 0;
	node->nodes[2] = 0;
	node->nodes[3] = 0;

	// Upper-Right quadrant
	if( width <= 16.0f )
	{
		CollectNodeData( node, positionX, positionZ, width, device );
		return;
	}
	
	CreateTreeNode( ( node->nodes[0] = new NodeType ), positionX - ( width / 4 ), positionZ - ( width / 4 ), width / 2.0f, device );
	CreateTreeNode( ( node->nodes[1] = new NodeType ), positionX - ( width / 4 ), positionZ + ( width / 4 ), width / 2.0f, device );
	CreateTreeNode( ( node->nodes[2] = new NodeType ), positionX + ( width / 4 ), positionZ - ( width / 4 ), width / 2.0f, device );
	CreateTreeNode( ( node->nodes[3] = new NodeType ), positionX + ( width / 4 ), positionZ + ( width / 4 ), width / 2.0f, device );

	return;
}


int CQuadTree::CountTriangles(float positionX, float positionZ, float width)
{
	int count, i;
	bool result;


	// Initialize the count to zero.
	count = 0;

	// Go through all the triangles in the entire mesh and check which ones should be inside this node.
	for(i=0; i<m_triangleCount; i++)
	{
		// If the triangle is inside the node then increment the count by one.
		result = IsTriangleContained(i, positionX, positionZ, width);
		if(result == true)
		{
			count++;
		}
	}

	return count;
}


bool CQuadTree::IsTriangleContained(int index, float positionX, float positionZ, float width)
{
	float radius;
	int vertexIndex;
	float x1, z1, x2, z2, x3, z3;
	float minimumX, maximumX, minimumZ, maximumZ;


	// Calculate the radius of this node.
	radius = width / 2.0f;

	// Get the index into the vertex list.
	vertexIndex = index * 3;

	// Get the three vertices of this triangle from the vertex list.
	x1 = m_vertexList[vertexIndex].position.x;
	z1 = m_vertexList[vertexIndex].position.z;
	vertexIndex++;
	
	x2 = m_vertexList[vertexIndex].position.x;
	z2 = m_vertexList[vertexIndex].position.z;
	vertexIndex++;

	x3 = m_vertexList[vertexIndex].position.x;
	z3 = m_vertexList[vertexIndex].position.z;

	// Check to see if the minimum of the x coordinates of the triangle is inside the node.
	minimumX = min(x1, min(x2, x3));
	if(minimumX > (positionX + radius))
	{
		return false;
	}

	// Check to see if the maximum of the x coordinates of the triangle is inside the node.
	maximumX = max(x1, max(x2, x3));
	if(maximumX < (positionX - radius))
	{
		return false;
	}

	// Check to see if the minimum of the z coordinates of the triangle is inside the node.
	minimumZ = min(z1, min(z2, z3));
	if(minimumZ > (positionZ + radius))
	{
		return false;
	}

	// Check to see if the maximum of the z coordinates of the triangle is inside the node.
	maximumZ = max(z1, max(z2, z3));
	if(maximumZ < (positionZ - radius))
	{
		return false;
	}

	return true;
}


void CQuadTree::ReleaseNode(NodeType* node)
{
	int i;


	// Recursively go down the tree and release the bottom nodes first.
	for(i=0; i<4; i++)
	{
		if(node->nodes[i] != 0)
		{
			ReleaseNode(node->nodes[i]);
		}
	}

	// Release the vertex buffer for this node.
	if(node->vertexBuffer)
	{
		node->vertexBuffer->Release();
		node->vertexBuffer = 0;
	}

	// Release the index buffer for this node.
	if(node->indexBuffer)
	{
		node->indexBuffer->Release();
		node->indexBuffer = 0;
	}

	// Release the four child nodes.
	for(i=0; i<4; i++)
	{
		if(node->nodes[i])
		{
			delete node->nodes[i];
			node->nodes[i] = 0;
		}
	}

	return;
}


void CQuadTree::RenderNode(NodeType* node, 
						   CFrustum* frustum, 
						   ID3D11DeviceContext* deviceContext, 
						   CTerrainShader* shader)
{

	bool result;
	int count, i, indexCount;
	unsigned int stride, offset;

	if( node == NULL ) return;

	// Check to see if the node can be viewed, height doesn't matter in a quad tree.
	result = frustum->CheckCube(node->positionX, 0.0f, node->positionZ, (node->width / 2.0f));

	// If it can't be seen then none of its children can either so don't continue down the tree, this is where the speed is gained.
	if(!result)
	{
		return;
	}

	// If it can be seen then check all four child nodes to see if they can also be seen.
	count = 0;
	for(i=0; i<4; i++)
	{
		if(node->nodes[i] != 0)
		{
			count++;
			RenderNode(node->nodes[i], frustum, deviceContext, shader);
		}
	}

	// If there were any children nodes then there is no need to continue as parent nodes won't contain any triangles to render.
	if(count != 0)
	{
		return;
	}

	// Otherwise if this node can be seen and has triangles in it then render these triangles.
	if(m_renderUpdate)
		m_CQuadTreeScene.push_back(node);

	
	m_drawCount += node->triangleCount;

	return;
}

void CQuadTree::RenderScene(){}

void CQuadTree::RenderScene( ID3D11DeviceContext *deviceContext, 
							CDirect3DSystem* d3d, 
							CTerrain *terrain,
							CTerrainShader *terrainShader,
							CGrass *grass,
							CGrassShader *grassShader, 
							D3DXMATRIX worldMatrix,
							D3DXMATRIX viewMatrix,
							D3DXMATRIX projectionMatrix, 
							D3DXVECTOR3 cameraDirection,
							D3DXVECTOR3 cameraLocation,
							unsigned long time,
							CLight *light )
{
	// Set vertex buffer stride and offset.
    unsigned int stride = sizeof(VertexType); 
	unsigned int offset = 0;
	int indexCount = 0;
	
	// Send cam angles from the sun, x and y, to the gpu
	float degreesFromSun = ( ( ( unsigned int )abs( cameraDirection.x + 17 ) ) % 360 );
	
	//  X-Component
	if( degreesFromSun < 20 && degreesFromSun > 0 )
	{
		cameraDirection = D3DXVECTOR3( 1 - ( 20 - degreesFromSun ) / 20, 
			cameraDirection.y, 
			cameraDirection.z );

		if( cameraDirection.x <  0.1f )
			cameraDirection = D3DXVECTOR3( 0.1f, cameraDirection.y, cameraDirection.z );
	}
	else if( degreesFromSun > 340 && degreesFromSun < 360 )
	{
		cameraDirection = D3DXVECTOR3( 1 - ( degreesFromSun - 340 ) / 20,
			cameraDirection.y,
			cameraDirection.z );

		if( cameraDirection.x <  0.1f )
			cameraDirection = D3DXVECTOR3( 0.1f, cameraDirection.y, cameraDirection.z );
	}
	else if( degreesFromSun == 0 || degreesFromSun == 360 )
	{
		cameraDirection = D3DXVECTOR3( 0.1f, cameraDirection.y, cameraDirection.z);
	}
	else
	{
		cameraDirection = D3DXVECTOR3( 1.0f, cameraDirection.y, cameraDirection.z );
	}

	// Y-component
	degreesFromSun = ( ( ( unsigned int )abs( cameraDirection.y + 46 ) ) % 360 );

	if( degreesFromSun < 40 && degreesFromSun > 0)
	{
		cameraDirection = D3DXVECTOR3( cameraDirection.x, 
			1 - ( 40 - degreesFromSun ) / 40, 
			1 - ( 40 - degreesFromSun ) / 40);

		if( cameraDirection.y <  0.1f )
			cameraDirection = D3DXVECTOR3( cameraDirection.x, 0.1f, 0.1f );
	}
	else if( degreesFromSun > 320 && degreesFromSun < 360 )
	{

		cameraDirection = D3DXVECTOR3( cameraDirection.x, 
			1 - ( degreesFromSun - 320 ) / 40, 
			1 - ( degreesFromSun - 320 ) / 40 );

		if( cameraDirection.y <  0.1f )
			cameraDirection = D3DXVECTOR3( cameraDirection.x, 0.1f, 0.1f );
	}
	else if( degreesFromSun == 0 || degreesFromSun == 360)
	{
		cameraDirection = D3DXVECTOR3( cameraDirection.x, 0.1f, 0.1f );
	}
	else
	{
		cameraDirection = D3DXVECTOR3( 1, 1, 1 );
	}

	cameraDirection *= .6;


	//  Render grass patches
	for(std::vector<NodeType *>::iterator i = m_CQuadTreeScene.begin(); i != m_CQuadTreeScene.end(); i++)
	{
		// Set the terrain shader parameters that it will use for rendering.
		bool result = terrainShader->SetShaderParameters(d3d->GetDeviceContext(),
			worldMatrix, 
			viewMatrix, 
			projectionMatrix, 
			light->GetAmbientColor(), 
			light->GetDiffuseColor(), 
			light->GetDirection(), 
			terrain->GetTexture(), 
			cameraDirection,
			D3DXVECTOR2( cameraLocation.x, cameraLocation.z ) );


		// Turn off blending.
		d3d->TurnOffAlphaBlending();
		// Set the vertex buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetVertexBuffers(0, 1, &((*i)->vertexBuffer), &stride, &offset);			//TODO: Gary: add vertexBuffer, indexBuffer, stride, offset, etc to struct CQuadTreeRenderStruct{vertexBuffer, indexBuffer,...}

		// Set the index buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetIndexBuffer(&(*(*i)->indexBuffer), DXGI_FORMAT_R32_UINT, 0);

		// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Determine the number of indices in this node.
		indexCount = (*i)->triangleCount * 3;

		// Call the terrain shader to render the polygons in this node.
		terrainShader->RenderShader(deviceContext, indexCount);

		d3d->TurnOffCulling();
		d3d->TurnOnAlphaBlending();

		//  Render four quads of the quadtree sub-division
		grass->Render( deviceContext );			
		grassShader->Render( deviceContext, 
			grass->GetIndexCount(), 
			worldMatrix, 
			viewMatrix, 
			projectionMatrix, 
			D3DXVECTOR4(1,1,1,1),
			D3DXVECTOR4(1,1,1,1),
			cameraLocation,
			cameraDirection,
			D3DXVECTOR4( (*i)->heightList._11, (*i)->heightList._12, (*i)->heightList._13, (*i)->heightList._14 ),
			D3DXVECTOR3( (*i)->positionX-8, 0.0f, (*i)->positionZ-8),
			time );


		grass->Render( deviceContext );			
		grassShader->Render( deviceContext, 
			grass->GetIndexCount(), 
			worldMatrix, 
			viewMatrix, 
			projectionMatrix, 
			D3DXVECTOR4(1,1,1,1),
			D3DXVECTOR4(1,1,1,1),
			cameraLocation,
			cameraDirection,
			D3DXVECTOR4( (*i)->heightList._21, (*i)->heightList._22, (*i)->heightList._23, (*i)->heightList._24 ),
			D3DXVECTOR3( (*i)->positionX, 0.0f, (*i)->positionZ-8),
			time );

		grass->Render( deviceContext );			
		grassShader->Render( deviceContext, 
			grass->GetIndexCount(), 
			worldMatrix, 
			viewMatrix, 
			projectionMatrix, 
			D3DXVECTOR4(1,1,1,1),
			D3DXVECTOR4(1,1,1,1),
			cameraLocation,
			cameraDirection,
			D3DXVECTOR4( (*i)->heightList._31, (*i)->heightList._32, (*i)->heightList._33, (*i)->heightList._34 ),
			D3DXVECTOR3( (*i)->positionX, 0.0f, (*i)->positionZ),
			time );

		grass->Render( deviceContext );			
		grassShader->Render( deviceContext, 
			grass->GetIndexCount(), 
			worldMatrix, 
			viewMatrix, 
			projectionMatrix, 
			D3DXVECTOR4(1,1,1,1),
			D3DXVECTOR4(1,1,1,1),
			cameraLocation,
			cameraDirection,
			D3DXVECTOR4( (*i)->heightList._41, (*i)->heightList._42, (*i)->heightList._43, (*i)->heightList._44 ),
			D3DXVECTOR3( (*i)->positionX-8, 0.0f, (*i)->positionZ),
			time );
	}

	d3d->TurnOnCulling();
	d3d->TurnOffAlphaBlending();

	if(m_renderUpdate)
		m_CQuadTreeScene.clear();

	return;
}


bool CQuadTree::SetIsRenderUpdate(bool renderUpdate)
{
	return (m_renderUpdate = renderUpdate);
}


int CQuadTree::GetQuadTreeIndexCount()
{
	return m_numberOfNodeVertices;
}

CQuadTree::NodeType *CQuadTree::GetRootNode()
{
	return m_parentNode;
}

int CQuadTree::GetHeightAtPointXY(float posX, float posZ, NodeType* node)
{
	if( posX <= 0 || posZ <= 0 || posX >= 256 || posZ >= 256 || node == NULL)
		return 50;

	if( node->width == 16 && abs( posX - node->positionX ) <= 8 && abs( posZ - node->positionZ ) <= 8 )
	{
		D3DXVECTOR3 *rayQueryTriangle;
		rayQueryTriangle = new D3DXVECTOR3[3];

		if( posX <= node->positionX && posZ <= node->positionZ )
		{
			if( posX - node->positionX < posZ - node->positionZ )
			{	 
				rayQueryTriangle[0] = D3DXVECTOR3( 0, 
					node->heightList._11 , 
					8 );
				rayQueryTriangle[1] = D3DXVECTOR3( 8, 
					node->heightList._12 , 
					8 );
				rayQueryTriangle[2] = D3DXVECTOR3( 0, 
					node->heightList._13 , 
					0 );
				
			}
			else
			{
				rayQueryTriangle[0] = D3DXVECTOR3( 0, 
					node->heightList._13 , 
					0 );
				rayQueryTriangle[1] = D3DXVECTOR3( 8, 
					node->heightList._12 , 
					8 );
				rayQueryTriangle[2] = D3DXVECTOR3( 8, 
					node->heightList._14 , 
					0 );
			}
					D3DXVECTOR3 pos = D3DXVECTOR3(posX-(node->positionX-8), 0, posZ-(node->positionZ-8));
					D3DXVECTOR3 directionVector = D3DXVECTOR3( 0, -1, 0 );
					D3DXVECTOR3 edge1, edge2;
					D3DXVECTOR3 normal;

					edge1 = rayQueryTriangle[1] - rayQueryTriangle[0];
					edge2 = rayQueryTriangle[2] - rayQueryTriangle[0];

					D3DXVec3Cross(&normal, &edge1, &edge2);
					D3DXVec3Normalize(&normal, &normal);
					
					float D = ((-normal.x * rayQueryTriangle[0].x) + (-normal.y * rayQueryTriangle[0].y) + (-normal.z * rayQueryTriangle[0].z));

					float denominator = ((normal.x * directionVector.x) + (normal.y * directionVector.y) + (normal.z * directionVector.z));

					float  numerator = -1.0f * (((normal.x * pos.x) + (normal.y * pos.y) + (normal.z * pos.z)) + D);
					float t = numerator / denominator;

					D3DXVECTOR3 Q = pos + (directionVector * t);

					return num = Q.y;
		}
		else if( ( posX >= node->positionX && posZ <= node->positionZ ) )
		{
			if( posX - node->positionX < posZ - node->positionZ )
			{ 
				rayQueryTriangle[0] = D3DXVECTOR3( 0, 
					node->heightList._21 , 
					8 );
				rayQueryTriangle[1] = D3DXVECTOR3( 8, 
					node->heightList._22 , 
					8 );
				rayQueryTriangle[2] = D3DXVECTOR3( 0, 
					node->heightList._23 , 
					0 );
			}
			else
			{
				rayQueryTriangle[0] = D3DXVECTOR3( 0, 
					node->heightList._23 , 
					 0 );
				rayQueryTriangle[1] = D3DXVECTOR3( 8, 
					node->heightList._22 , 
					8 );
				rayQueryTriangle[2] = D3DXVECTOR3( 8, 
					node->heightList._24 , 
					 0 );
			}
					D3DXVECTOR3 pos = D3DXVECTOR3(posX-node->positionX, 0, posZ-(node->positionZ-8));
					D3DXVECTOR3 directionVector = D3DXVECTOR3( 0, -1, 0 );
					D3DXVECTOR3 edge1, edge2;
					D3DXVECTOR3 normal;

					edge1 = rayQueryTriangle[1] - rayQueryTriangle[0];
					edge2 = rayQueryTriangle[2] - rayQueryTriangle[0];

					D3DXVec3Cross(&normal, &edge1, &edge2);
					D3DXVec3Normalize(&normal, &normal);

					float D = ((-normal.x * rayQueryTriangle[0].x) + (-normal.y * rayQueryTriangle[0].y) + (-normal.z * rayQueryTriangle[0].z));

					float denominator = ((normal.x * directionVector.x) + (normal.y * directionVector.y) + (normal.z * directionVector.z));

					float  numerator = -1.0f * (((normal.x * pos.x) + (normal.y * pos.y) + (normal.z * pos.z)) + D);
					float t = numerator / denominator;

					D3DXVECTOR3 Q = pos + (directionVector * t);

					return num = Q.y;
		}
		else if( ( posX >= node->positionX && posZ >= node->positionZ ) )
		{
			if( abs( ( posX - node->positionX ) ) < abs( ( posZ - node->positionZ ) ) )
			{
					rayQueryTriangle[0] = D3DXVECTOR3( 0, 
						node->heightList._31 , 
						8 );
					rayQueryTriangle[1] = D3DXVECTOR3( 8, 
						node->heightList._32 , 
						8 );
					rayQueryTriangle[2] = D3DXVECTOR3( 0, 
						node->heightList._33 , 
						0 );
			}
			else
			{
					rayQueryTriangle[0] = D3DXVECTOR3( 0, 
						node->heightList._33 , 
						0 );
					rayQueryTriangle[1] = D3DXVECTOR3( 8, 
						node->heightList._32 , 
						8 );
					rayQueryTriangle[2] = D3DXVECTOR3( 8, 
						node->heightList._34 , 
						0 );
			}
					D3DXVECTOR3 pos = D3DXVECTOR3(posX-node->positionX, 0, posZ-node->positionZ);
					D3DXVECTOR3 directionVector = D3DXVECTOR3( 0, -1, 0 );
					D3DXVECTOR3 edge1, edge2;
					D3DXVECTOR3 normal;

					edge1 = rayQueryTriangle[1] - rayQueryTriangle[0];
					edge2 = rayQueryTriangle[2] - rayQueryTriangle[0];

					D3DXVec3Cross(&normal, &edge1, &edge2);
					D3DXVec3Normalize(&normal, &normal);

					float D = ((-normal.x * rayQueryTriangle[0].x) + (-normal.y * rayQueryTriangle[0].y) + (-normal.z * rayQueryTriangle[0].z));

					float denominator = ((normal.x * directionVector.x) + (normal.y * directionVector.y) + (normal.z * directionVector.z));

					float  numerator = -1.0f * (((normal.x * pos.x) + (normal.y * pos.y) + (normal.z * pos.z)) + D);
					float t = numerator / denominator;

					D3DXVECTOR3 Q = pos + (directionVector * t);

					return num = Q.y;
		}
		else if( ( posX <= node->positionX && posZ >= node->positionZ ) )
		{
			if( posX - node->positionX < posZ - node->positionZ  )
			{
					rayQueryTriangle[0] = D3DXVECTOR3( 0, 
						node->heightList._41 , 
						8 ),
					rayQueryTriangle[1] = D3DXVECTOR3( 8, 
						node->heightList._42 , 
						8 ),
					rayQueryTriangle[2] = D3DXVECTOR3( 0, 
						node->heightList._43 , 
						0 );
			}
			else
			{
					rayQueryTriangle[0] = D3DXVECTOR3( 0, 
						node->heightList._43 , 
						0 );
					rayQueryTriangle[1] = D3DXVECTOR3(8, 
						node->heightList._42 , 
						8 );
					rayQueryTriangle[2] = D3DXVECTOR3( 8, 
						node->heightList._43 , 
						0 );
			}
					D3DXVECTOR3 pos = D3DXVECTOR3(posX-(node->positionX-8), 0, posZ-node->positionZ);
					D3DXVECTOR3 directionVector = D3DXVECTOR3( 0, -1, 0 );
					D3DXVECTOR3 edge1, edge2;
					D3DXVECTOR3 normal;

					edge1 = rayQueryTriangle[1] - rayQueryTriangle[0];
					edge2 = rayQueryTriangle[2] - rayQueryTriangle[0];

					D3DXVec3Cross(&normal, &edge1, &edge2);
					D3DXVec3Normalize(&normal, &normal);

					float D = ((-normal.x * rayQueryTriangle[0].x) + (-normal.y * rayQueryTriangle[0].y) + (-normal.z * rayQueryTriangle[0].z));

					float denominator = ((normal.x * directionVector.x) + (normal.y * directionVector.y) + (normal.z * directionVector.z));

					float  numerator = -1.0f * (((normal.x * pos.x) + (normal.y * pos.y) + (normal.z * pos.z)) + D);
					float t = numerator / denominator;

					D3DXVECTOR3 Q = pos + (directionVector * t);

					return num = Q.y;
		}
			return 6;
	}
	
	if( posX <= node->positionX && posZ <= node->positionZ )
		GetHeightAtPointXY( posX, posZ, node->nodes[0] );
	if( posX >= node->positionX && posZ <= node->positionZ )
		GetHeightAtPointXY( posX, posZ, node->nodes[2] );
	if( posX >= node->positionX && posZ >= node->positionZ )
		GetHeightAtPointXY( posX, posZ, node->nodes[3] );
	if( posX <= node->positionX && posZ >= node->positionZ )
		GetHeightAtPointXY( posX, posZ, node->nodes[1] );
		

	return 6;
}

#pragma once

const int MAX_TRIANGLES = 64;//20000;//512;//2048;//10000


#include "CTerrain.h"
#include "CFrustum.h"
#include "CTerrain.h"
#include "CTerrainShader.h"
#include "CGrass.h"
#include "CGrassShader.h"
#include "CLight.h"
#include "CDirect3DSystem.h"

#include <vector>

class CQuadTree
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
		D3DXVECTOR4 color;
	};

	struct NodeVertexType
	{
		D3DXVECTOR3 position;
	};

	struct NodeType
	{
        float positionX, positionZ, positionY; 
		float width;
		static const int triangleCount = 8;
		D3DXMATRIX heightList;
		ID3D11Buffer *vertexBuffer, *indexBuffer;
        NodeType* nodes[4];
	};

	struct NodeType *first;

public:
	CQuadTree();
	CQuadTree(const CQuadTree&);
	~CQuadTree();
		
	bool Initialize				( CTerrain*, ID3D11Device* );
	void Shutdown				( void );
	void Render					( CFrustum*, ID3D11DeviceContext*, CTerrainShader* );
	void RenderScene			( ID3D11DeviceContext* , 
									CDirect3DSystem*, 
									CTerrain *,
									CTerrainShader*, 
									CGrass *, 
									CGrassShader *, 
									D3DXMATRIX, 
									D3DXMATRIX, 
									D3DXMATRIX, 
									D3DXVECTOR3,
									D3DXVECTOR3,
									unsigned long,
									CLight *);

	bool SetIsRenderUpdate		( bool );
	int GetQuadTreeIndexCount	( void );
	int GetDrawCount			( void );
	void RenderTreeNodes		( ID3D11DeviceContext* );
	int GetHeightAtPointXY		( float, float, NodeType* );
	NodeType *GetRootNode		( void );
	float GetHeight				(){return num;}

private:
	void CalculateMeshDimensions	( int, float&, float&, float& );
	void CreateTreeNode				( NodeType*, float, float, float, ID3D11Device* );
	int CountTriangles				( float, float, float );
	bool IsTriangleContained		( int, float, float, float );
	void ReleaseNode				( NodeType* );
	void RenderNode					( NodeType*, CFrustum*, ID3D11DeviceContext*, CTerrainShader*);
	void InitializeTreeNodeBuffers	( ID3D11Device* );
	void CollectNodeData			( NodeType *, int, int, float, ID3D11Device* );
	void RenderScene				( void );

private:
	bool m_renderUpdate;
	std::vector<NodeType *> m_CQuadTreeScene;
	NodeType *root;
	
	float num;

	int m_triangleCount, m_drawCount;
	int m_numberOfNodeVertices;

	unsigned long *m_nodeIndices;

	CPerlinNoise *pn;
	ID3D11Buffer * m_nodeIndicesBuffer;
	ID3D11Buffer *m_nodeVerticesBuffer;
	NodeVertexType *m_nodeVertices;
	VertexType* m_vertexList;
	NodeType* m_parentNode;

	CTerrain *m_Terrain;
};
#pragma once
#include "Dx11_Terrain.h"


class Dx11_QuadTree
{
	const unsigned int		m_cMAX_TRIANGLES_PER_NODE = 100;

public:
	struct stNode
	{
		float fCenterX = 0.0f, fCenterZ = 0.0f, fWidth = 0.0;
		int iVertexCount = 0;//(using as Index Count)
		ID3D11Buffer	*m_pVB = nullptr, *m_pIB = nullptr;
		stNode			*m_pChild[4] = { nullptr , nullptr , nullptr , nullptr };
	};	

protected:
	Dx11_Terrain::stVertex  *m_pVertexList = nullptr;
	stNode					*m_pRootNode = nullptr;
	unsigned int			m_uiVertexCount;

	bool AddQuadNode(float _fCenterX, float _fCenterZ, float _fWidth);
	void ReleaseQuadNode(stNode *_pNode);
	void RenderQuadNode(ID3D11DeviceContext* _pDeviceContext, stNode *_pNode);

	void CalculateMeshDimension(unsigned int _uiVertexCount, float &_fCenterX, float &_fCenterZ, float &_fWidth);
	void CreateTreeNode(ID3D11Device* _pDevice, stNode **_pNode, float _fCenterX, float _fCenterZ, float _fWidth);

	bool IsTriangleContained(unsigned int _uiVertexIndex, float _fCenterX, float _fCenterZ, float _fWidth);
	unsigned int CountTriangle(float _fCenterX, float _fCenterZ, float _fWidth);


public:
	Dx11_QuadTree();
	~Dx11_QuadTree();
	bool BuildQuadTree(ID3D11Device* _pDevice, Dx11_Terrain* _pTerrain);
	void Release();
	void Render(ID3D11DeviceContext* _pDeviceContext);
};


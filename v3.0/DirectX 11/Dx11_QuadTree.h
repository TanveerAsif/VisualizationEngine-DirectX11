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

	struct stWVPBuffer
	{
		D3DXMATRIX worldMat;
		D3DXMATRIX viewMat;
		D3DXMATRIX projMat;
	};

	struct stQuadNodeColor
	{
		D3DXVECTOR4 vColor;
	};

protected:
	D3DXMATRIX			m_worldMat, m_viewMat, m_projMat;
	ID3D11VertexShader  *m_pVS = nullptr;	
	ID3D11PixelShader   *m_pPS = nullptr;
	ID3D11InputLayout	*m_pInputLayout = nullptr;
	ID3D11Buffer		*m_pWVPBuffer = nullptr, *m_pQuadColor = nullptr;

	Dx11_Terrain::stVertex  *m_pVertexList = nullptr;
	stNode					*m_pRootNode = nullptr;
	unsigned int			m_uiVertexCount;
	unsigned int			m_uiQuadColorCounter;

	bool AddQuadNode(float _fCenterX, float _fCenterZ, float _fWidth);
	bool IntializeShader(ID3D11Device * _pDevice);
	void ReleaseQuadNode(stNode *_pNode);
	void RenderQuadNode(ID3D11DeviceContext* _pDeviceContext, stNode *_pNode);

	void CalculateMeshDimension(unsigned int _uiVertexCount, float &_fCenterX, float &_fCenterZ, float &_fWidth);
	void CreateTreeNode(ID3D11Device* _pDevice, stNode **_pNode, float _fCenterX, float _fCenterZ, float _fWidth);

	bool IsTriangleContained(unsigned int _uiVertexIndex, float _fCenterX, float _fCenterZ, float _fWidth);
	unsigned int CountTriangle(float _fCenterX, float _fCenterZ, float _fWidth);
	D3DXVECTOR4& GetColor(unsigned int _index);

public:
	Dx11_QuadTree();
	~Dx11_QuadTree();
	bool BuildQuadTree(ID3D11Device* _pDevice, Dx11_Terrain* _pTerrain);
	void Release();
	void Render(ID3D11DeviceContext* _pDeviceContext, float _fTick, D3DXMATRIX _worldMat, D3DXMATRIX _viewMat, D3DXMATRIX _projMat);	
};


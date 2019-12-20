#include "Dx11_QuadTree.h"
#include <fstream>


bool Dx11_QuadTree::AddQuadNode(float _fCenterX, float _fCenterZ, float _fWidth)
{
	return false;
}

void Dx11_QuadTree::CalculateMeshDimension(unsigned int _uiVertexCount, float & _fCenterX, float & _fCenterZ, float & _fWidth)
{	
	for (unsigned int  i = 0; i < _uiVertexCount; i++)
	{
		_fCenterX += m_pVertexList[i].vPos.x;
		_fCenterZ += m_pVertexList[i].vPos.z;
	}
	_fCenterX = _fCenterX / _uiVertexCount;
	_fCenterZ = _fCenterZ / _uiVertexCount;

	float maxWidth = 0.0f, maxDepth = 0.0f;
	float minWidth = _fCenterX - m_pVertexList[0].vPos.x;
	float minDepth = _fCenterZ - m_pVertexList[0].vPos.z;

	for (unsigned int i = 0; i < _uiVertexCount; i++)
	{
		float w = _fCenterX - m_pVertexList[i].vPos.x;
		float d = _fCenterZ - m_pVertexList[i].vPos.z;

		if (minWidth < w) minWidth = w;
		if (minDepth < d) minDepth = d;
		if (w > maxWidth) maxWidth = w;
		if (d > maxDepth) maxDepth = d;
	}
		
	float maxX = max(fabs(minWidth), fabs(maxWidth));
	float maxZ = max(fabs(minDepth), fabs(maxDepth));

	//Max Diameter of Mesh
	_fWidth = max(maxX, maxZ) * 2.0;

	return;
}

void Dx11_QuadTree::CreateTreeNode(ID3D11Device * _pDevice, stNode *_pNode, float _fCenterX, float _fCenterZ, float _fWidth)
{
	_pNode = new stNode();
	_pNode->iVertexCount = 0;
	_pNode->fCenterX = _fCenterX;
	_pNode->fCenterZ = _fCenterZ;
	_pNode->fWidth = _fWidth;
	for (int iChild = 0; iChild < 4; iChild++)
		_pNode->m_pChild[iChild] = nullptr;

	//Count Triangles in Width By CenterX and CenterZ
	unsigned int uiTriangles = CountTriangle(_fCenterX, _fCenterZ, _fWidth);
	
	if (uiTriangles < m_cMAX_TRIANGLES_PER_NODE)
	{
		_pNode->iVertexCount = uiTriangles * 3;
		//Create VB and IB
		//_pNode->m_pVB, _pNode->m_pIB
		unsigned int index = 0;
		Dx11_Terrain::stVertex *pVertices = new Dx11_Terrain::stVertex[_pNode->iVertexCount];
		unsigned int           *pIndices = new unsigned int[_pNode->iVertexCount];
		for (unsigned int i = 0; i < _pNode->iVertexCount; i += 3)
		{
			if (IsTriangleContained(i, _fCenterX, _fCenterZ, _fWidth))
			{
				pVertices[index] = m_pVertexList[i];
				pIndices[index] = index;
				index++;

				pVertices[index] = m_pVertexList[i + 1];
				pIndices[index] = index;
				index++;

				pVertices[index] = m_pVertexList[i + 2];
				pIndices[index] = index;
				index++;
			}
			 
		}

		D3D11_BUFFER_DESC vertexBuffDesc;
		ZeroMemory(&vertexBuffDesc, sizeof(vertexBuffDesc));
		vertexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBuffDesc.ByteWidth = sizeof(Dx11_Terrain::stVertex) * _pNode->iVertexCount;
		vertexBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA vertexBuffRes;
		ZeroMemory(&vertexBuffRes, sizeof(vertexBuffRes));
		vertexBuffRes.pSysMem = pVertices;
		HRESULT hr = _pDevice->CreateBuffer(&vertexBuffDesc, &vertexBuffRes, &_pNode->m_pVB);
		if (hr != S_OK)
			return;

		D3D11_BUFFER_DESC indexBuffDesc;
		ZeroMemory(&indexBuffDesc, sizeof(indexBuffDesc));
		indexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBuffDesc.ByteWidth = sizeof(unsigned int) * _pNode->iVertexCount;
		indexBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA indexBuffRes;
		ZeroMemory(&indexBuffRes, sizeof(indexBuffRes));
		indexBuffRes.pSysMem = pIndices;
		hr = _pDevice->CreateBuffer(&indexBuffDesc, &indexBuffRes, &_pNode->m_pIB);
		if (hr != S_OK)
			return;

		delete pVertices;
		delete pIndices;
	}
	else
	{
		//Create Four Child
		for (int iChildIndex = 0; iChildIndex < 4; iChildIndex++)
		{
			float fOffsetX = ((iChildIndex % 2 == 0) ? -1 : 1) *_fWidth / 2;
			float fOffsetZ = ((iChildIndex % 4 < 2) ? -1 : 1 ) *_fWidth / 2;

			CreateTreeNode(_pDevice, _pNode->m_pChild[iChildIndex], _fCenterX + fOffsetX, _fCenterZ + fOffsetZ, _fWidth / 2.0);
		}
		int ok = 1;
	}
}

bool Dx11_QuadTree::IsTriangleContained(unsigned int _uiVertexIndex, float _fCenterX, float _fCenterZ, float _fWidth)
{
	float x1 = m_pVertexList[_uiVertexIndex].vPos.x;
	float x2 = m_pVertexList[_uiVertexIndex + 1].vPos.x;
	float x3 = m_pVertexList[_uiVertexIndex + 2].vPos.x;

	float z1 = m_pVertexList[_uiVertexIndex].vPos.z;
	float z2 = m_pVertexList[_uiVertexIndex + 1].vPos.z;
	float z3 = m_pVertexList[_uiVertexIndex + 2].vPos.z;


	float minX = min(x1, min(x2, x3));
	float minZ = min(z1, min(z2, z3));
	float maxX = max(x1, max(x2, x3));
	float maxZ = max(z1, max(z2, z3));


	float fRadius = (_fWidth / 2);
	if ((minX >= _fCenterX - fRadius) && (minZ >= _fCenterZ - fRadius) && (maxZ <= _fCenterZ + fRadius) && (maxZ <= _fCenterZ + fRadius))
		return true;

	return false;
}

unsigned int Dx11_QuadTree::CountTriangle(float _fCenterX, float _fCenterZ, float _fWidth)
{
	unsigned int uiCount = 0;
	for (unsigned int i = 0; i < m_uiVertexCount; i += 3) //Three Vertices Check To Form Circle and lies in x, z and width
	{
		if (IsTriangleContained(i, _fCenterX, _fCenterZ, _fWidth))
		{
			uiCount++;
		}
	}
	return uiCount;
}

Dx11_QuadTree::Dx11_QuadTree()	
	:m_uiVertexCount(0)
{

}


Dx11_QuadTree::~Dx11_QuadTree()
{
}

bool Dx11_QuadTree::BuildQuadTree(ID3D11Device	*_pDevice, Dx11_Terrain	*_pTerrain)
{
	//Terrain Failed To Generate Vertices
	if (!_pTerrain->Init(_pDevice))
		return false;

	m_uiVertexCount = _pTerrain->GetVertexCount();
	m_pVertexList = new Dx11_Terrain::stVertex[m_uiVertexCount];
	_pTerrain->CopyVertices(m_pVertexList);
	

	float centerX = 0.0f, centerZ = 0.0f, width = 0.0f;
	CalculateMeshDimension(m_uiVertexCount, centerX, centerZ, width);

	//m_pRootNode = new stNode();
	CreateTreeNode(_pDevice, m_pRootNode, centerX, centerZ, width);

	return true;
}

void Dx11_QuadTree::Release()
{
	if (m_pRootNode)
	{
		//delete every child0.
	}
}

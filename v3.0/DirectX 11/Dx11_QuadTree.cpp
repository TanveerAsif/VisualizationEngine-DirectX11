#include "Dx11_QuadTree.h"
#include <fstream>

//bool errorLogger(ID3D10Blob *_pErrorBuffer)
//{
//	FILE *fp = NULL;
//	char *pCompileError = (char *)(_pErrorBuffer->GetBufferPointer());
//	unsigned long bufferSize = _pErrorBuffer->GetBufferSize();
//
//	fopen_s(&fp, "../../Data/ShaderCompileError.txt", "w");
//	if (fp)
//	{
//		for (int i = 0; i < bufferSize; i++)
//		{
//			fwrite(&pCompileError[i], sizeof(pCompileError[i]), 1, fp);
//		}
//		fclose(fp);
//	}
//	return false;
//}

bool Dx11_QuadTree::AddQuadNode(float _fCenterX, float _fCenterZ, float _fWidth)
{
	return false;
}

bool Dx11_QuadTree::IntializeShader(ID3D11Device * _pDevice)
{
	ID3D10Blob *pErrorBuffer, *pVSBuffer, *pHSBuffer, *pDSBuffer, *pPSBuffer;
	//Compile Vertex Shader
	HRESULT hr = D3DX11CompileFromFile(L"../../Data/QuadTree.hlsl", nullptr, nullptr, "VS", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr,
		&pVSBuffer, &pErrorBuffer, nullptr);
	if (hr != S_OK)
	{
		//errorLogger(pErrorBuffer);
		return false;
	}

	//Compile Pixel Shader
	hr = D3DX11CompileFromFile(L"../../Data/QuadTree.hlsl", nullptr, nullptr, "PS", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr,
		&pPSBuffer, &pErrorBuffer, nullptr);
	if (hr != S_OK)
	{
		//errorLogger(pErrorBuffer);
		return false;
	}

	hr = _pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), nullptr, &m_pVS);
	if (hr != S_OK)
		return false;

	hr = _pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), nullptr, &m_pPS);
	if (hr != S_OK)
		return false;

	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(inputLayout);
	hr = _pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pInputLayout);
	if (hr != S_OK)
		return false;

	pVSBuffer->Release();
	pPSBuffer->Release();

	pVSBuffer = nullptr;
	pHSBuffer = nullptr;

	D3D11_BUFFER_DESC buffDesc;
	ZeroMemory(&buffDesc, sizeof(buffDesc));
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.ByteWidth = sizeof(stWVPBuffer);
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = _pDevice->CreateBuffer(&buffDesc, nullptr, &m_pWVPBuffer);
	if (hr != S_OK)
		return false;
	
	//QuadColor
	ZeroMemory(&buffDesc, sizeof(buffDesc));
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.ByteWidth = sizeof(stQuadNodeColor);
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = _pDevice->CreateBuffer(&buffDesc, nullptr, &m_pQuadColor);
	if (hr != S_OK)
		return false;

	return true;
}

void Dx11_QuadTree::ReleaseQuadNode(stNode * _pNode)
{
	for (int i = 0; i < 4; i++)
	{
		if (_pNode->m_pChild[i] != nullptr)
			ReleaseQuadNode(_pNode->m_pChild[i]);
	}

	if (_pNode->m_pVB)
	{
		_pNode->m_pVB->Release();
		_pNode->m_pVB = nullptr;
	}

	if (_pNode->m_pIB)
	{
		_pNode->m_pIB->Release();
		_pNode->m_pIB = nullptr;
	}

	for (int i = 0; i < 4; i++)
	{
		if (_pNode->m_pChild[i])
		{
			delete _pNode->m_pChild[i];
			_pNode->m_pChild[i] = nullptr;
		}
	}
}

void Dx11_QuadTree::RenderQuadNode(ID3D11DeviceContext* _pDeviceContext, stNode* _pNode)
{
	unsigned int unChildCount = 0; //Child Count of Node _pNode
	for (int i = 0; i < 4; i++)
	{
		if (_pNode->m_pChild[i])
		{
			unChildCount++;
			RenderQuadNode(_pDeviceContext, _pNode->m_pChild[i]);
		}
	}

	if (unChildCount > 0)
		return; 

	//Render Node
	//SetVertex Buffer
	UINT stride = sizeof(Dx11_Terrain::stVertex);
	UINT offset = 0;
	_pDeviceContext->IASetVertexBuffers(0, 1, &_pNode->m_pVB, &stride, &offset);
	_pDeviceContext->IASetIndexBuffer(_pNode->m_pIB, DXGI_FORMAT_R32_UINT, 0);
	_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = _pDeviceContext->Map(m_pWVPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return;
	stWVPBuffer *pData = (stWVPBuffer *)mappedResource.pData;
	pData->worldMat = m_worldMat;
	pData->viewMat = m_viewMat;
	pData->projMat = m_projMat;
	_pDeviceContext->Unmap(m_pWVPBuffer, 0);
	_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pWVPBuffer);

	
	m_uiQuadColorCounter++;
	if (m_uiQuadColorCounter > 3)
		m_uiQuadColorCounter = 0;

	D3DXVECTOR4 newQuadColor = GetColor(m_uiQuadColorCounter);

	D3D11_MAPPED_SUBRESOURCE mappedQuadColorRes;
	hr = _pDeviceContext->Map(m_pQuadColor, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedQuadColorRes);
	if (hr != S_OK)
		return;
	stQuadNodeColor *pQuadColor = (stQuadNodeColor *)mappedQuadColorRes.pData;
	pQuadColor->vColor = newQuadColor;
	_pDeviceContext->Unmap(m_pQuadColor, 0);
	_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pQuadColor);

	_pDeviceContext->VSSetShader(m_pVS, nullptr, 0);
	_pDeviceContext->PSSetShader(m_pPS, nullptr, 0);

	//See declaration of iVertexCount
	_pDeviceContext->DrawIndexed(_pNode->iVertexCount, 0, 0);
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
		
	float maxX = (float)max(fabs(minWidth), fabs(maxWidth));
	float maxZ = (float)max(fabs(minDepth), fabs(maxDepth));

	//Max Diameter of Mesh
	_fWidth = max(maxX, maxZ) * 2.0;

	return;
}

void Dx11_QuadTree::CreateTreeNode(ID3D11Device * _pDevice, stNode **_pNode, float _fCenterX, float _fCenterZ, float _fWidth)
{
	*_pNode = new stNode();
	(*_pNode)->iVertexCount = 0;
	(*_pNode)->fCenterX = _fCenterX;
	(*_pNode)->fCenterZ = _fCenterZ;
	(*_pNode)->fWidth = _fWidth;
	for (int iChild = 0; iChild < 4; iChild++)
		(*_pNode)->m_pChild[iChild] = nullptr;

	//Count Triangles in Width By CenterX and CenterZ
	unsigned int uiTriangles = CountTriangle(_fCenterX, _fCenterZ, _fWidth);
	if (uiTriangles == 0)
		return;
	
	if (uiTriangles < m_cMAX_TRIANGLES_PER_NODE)
	{
		(*_pNode)->iVertexCount = uiTriangles * 3;
		//Create VB and IB
		//_pNode->m_pVB, _pNode->m_pIB
		unsigned int index = 0;
		Dx11_Terrain::stVertex *pVertices = new Dx11_Terrain::stVertex[(*_pNode)->iVertexCount];
		unsigned int           *pIndices = new unsigned int[(*_pNode)->iVertexCount];
		for (unsigned int i = 0; i < /*(*_pNode)->iVertexCount*/m_uiVertexCount; i += 3)
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
		vertexBuffDesc.ByteWidth = sizeof(Dx11_Terrain::stVertex) * (*_pNode)->iVertexCount;
		vertexBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA vertexBuffRes;
		ZeroMemory(&vertexBuffRes, sizeof(vertexBuffRes));
		vertexBuffRes.pSysMem = pVertices;
		HRESULT hr = _pDevice->CreateBuffer(&vertexBuffDesc, &vertexBuffRes, &(*_pNode)->m_pVB);
		if (hr != S_OK)
			return;

		D3D11_BUFFER_DESC indexBuffDesc;
		ZeroMemory(&indexBuffDesc, sizeof(indexBuffDesc));
		indexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBuffDesc.ByteWidth = sizeof(unsigned int) * (*_pNode)->iVertexCount;
		indexBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA indexBuffRes;
		ZeroMemory(&indexBuffRes, sizeof(indexBuffRes));
		indexBuffRes.pSysMem = pIndices;
		hr = _pDevice->CreateBuffer(&indexBuffDesc, &indexBuffRes, &(*_pNode)->m_pIB);
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
			float fOffsetX = ((iChildIndex % 2 == 0) ? -1 : 1) *_fWidth / 4;
			float fOffsetZ = ((iChildIndex % 4 < 2) ? -1 : 1 ) *_fWidth / 4;

			unsigned int iTriangles = CountTriangle((_fCenterX + fOffsetX), (_fCenterZ + fOffsetZ), (_fWidth / 2.0));
			if (iTriangles > 0)
				CreateTreeNode(_pDevice, &(*_pNode)->m_pChild[iChildIndex], _fCenterX + fOffsetX, _fCenterZ + fOffsetZ, _fWidth / 2.0);
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
	if ((minX >= _fCenterX - fRadius) && (minZ >= _fCenterZ - fRadius) && (maxX <= _fCenterX + fRadius) && (maxZ <= _fCenterZ + fRadius))
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

D3DXVECTOR4 & Dx11_QuadTree::GetColor(unsigned int _index)
{
	D3DXVECTOR4 *pColor;
	switch (_index)
	{
	case 1 :
		pColor = new D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
		break;

	case 2:
		pColor = new D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
		break;

	case 3:
		pColor = new D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
		break;

	default:
		pColor = new D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
		break;
	}
	return *pColor;
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
		
	//Init Shader
	if (!IntializeShader(_pDevice))
		return false;

	//Build Tree
	m_uiVertexCount = _pTerrain->GetVertexCount();
	m_pVertexList = new Dx11_Terrain::stVertex[m_uiVertexCount];
	_pTerrain->CopyVertices(m_pVertexList);
	

	float centerX = 0.0f, centerZ = 0.0f, width = 0.0f;
	CalculateMeshDimension(m_uiVertexCount, centerX, centerZ, width);
	CreateTreeNode(_pDevice, &m_pRootNode, centerX, centerZ, width);

	//Vertex List Not Required Once QuadTree Build
	if (m_pVertexList)
	{
		delete[] m_pVertexList;
		m_pVertexList = nullptr;
	}

	return true;
}

void Dx11_QuadTree::Release()
{
	if (m_pRootNode)
	{
		//delete every child0.
		ReleaseQuadNode(m_pRootNode);
	}
}

void Dx11_QuadTree::Render(ID3D11DeviceContext * _pDeviceContext, float _fTick, D3DXMATRIX _worldMat, D3DXMATRIX _viewMat, D3DXMATRIX _projMat)
{
	
	_pDeviceContext->IASetInputLayout(m_pInputLayout);
	
	//Set world, view and proj matrix
	m_worldMat = _worldMat;
	m_viewMat = _viewMat;
	m_projMat = _projMat;
	D3DXMatrixTranspose(&m_worldMat, &m_worldMat);
	D3DXMatrixTranspose(&m_viewMat, &m_viewMat);
	D3DXMatrixTranspose(&m_projMat, &m_projMat);

	m_uiQuadColorCounter = 0;
	RenderQuadNode(_pDeviceContext, m_pRootNode);
}

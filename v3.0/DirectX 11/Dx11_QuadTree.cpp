#include "Dx11_QuadTree.h"
#include <fstream>
#include <math.h>

bool errorLogger(ID3D10Blob *_pErrorBuffer)
{
	FILE *fp = NULL;
	char *pCompileError = (char *)(_pErrorBuffer->GetBufferPointer());
	unsigned long bufferSize = _pErrorBuffer->GetBufferSize();

	fopen_s(&fp, "../../Data/ShaderCompileError.txt", "w");
	if (fp)
	{
		for (int i = 0; i < bufferSize; i++)
		{
			fwrite(&pCompileError[i], sizeof(pCompileError[i]), 1, fp);
		}
		fclose(fp);
	}
	return false;
}

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

bool Dx11_QuadTree::InitializaTessalationShader(ID3D11Device * _pDevice)
{
	ID3D10Blob *pErrorBuffer, *pVSBuffer, *pHSBuffer, *pDSBuffer, *pPSBuffer;
	//Compile Vertex Shader
	HRESULT hr = D3DX11CompileFromFile(L"../../Data/TessellationShader.hlsl", nullptr, nullptr, "MyVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr,
		&pVSBuffer, &pErrorBuffer, nullptr);
	if (hr != S_OK)
	{
		errorLogger(pErrorBuffer);
		return false;
	}


	//Compile Hull Shader
	hr = D3DX11CompileFromFile(L"../../Data/TessellationShader.hlsl", nullptr, nullptr, "MyHullShader", "hs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr,
		&pHSBuffer, &pErrorBuffer, nullptr);
	if (hr != S_OK)
	{
		errorLogger(pErrorBuffer);
		return false;
	}


	//Compile Domain Shader
	hr = D3DX11CompileFromFile(L"../../Data/TessellationShader.hlsl", nullptr, nullptr, "MyDomainShader", "ds_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr,
		&pDSBuffer, &pErrorBuffer, nullptr);
	if (hr != S_OK)
	{
		errorLogger(pErrorBuffer);
		return false;
	}

	//Compile Pixel Shader
	hr = D3DX11CompileFromFile(L"../../Data/TessellationShader.hlsl", nullptr, nullptr, "MyPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr,
		&pPSBuffer, &pErrorBuffer, nullptr);
	if (hr != S_OK)
	{
		errorLogger(pErrorBuffer);
		return false;
	}

	hr = _pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), nullptr, &m_pVS);
	if (hr != S_OK)
		return false;

	hr = _pDevice->CreateHullShader(pHSBuffer->GetBufferPointer(), pHSBuffer->GetBufferSize(), nullptr, &m_pHS);
	if (hr != S_OK)
		return false;

	hr = _pDevice->CreateDomainShader(pDSBuffer->GetBufferPointer(), pDSBuffer->GetBufferSize(), nullptr, &m_pDS);
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
	pHSBuffer->Release();
	pDSBuffer->Release();
	pPSBuffer->Release();

	pVSBuffer = nullptr;
	pHSBuffer = nullptr;
	pDSBuffer = nullptr;
	pPSBuffer = nullptr;

	D3D11_BUFFER_DESC buffDesc;
	ZeroMemory(&buffDesc, sizeof(buffDesc));
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.ByteWidth = sizeof(stWVPBuffer);
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = _pDevice->CreateBuffer(&buffDesc, nullptr, &m_pWVPBuffer);
	if (hr != S_OK)
		return false;

	D3D11_BUFFER_DESC TessbuffDesc;
	ZeroMemory(&TessbuffDesc, sizeof(TessbuffDesc));
	TessbuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	TessbuffDesc.ByteWidth = sizeof(stTessellationBuffer);
	TessbuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	TessbuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = _pDevice->CreateBuffer(&TessbuffDesc, nullptr, &m_pTessellationBuffer);
	if (hr != S_OK)
		return false;

	D3D11_BUFFER_DESC QuadColorDesc;
	ZeroMemory(&QuadColorDesc, sizeof(QuadColorDesc));
	QuadColorDesc.Usage = D3D11_USAGE_DYNAMIC;
	QuadColorDesc.ByteWidth = sizeof(stQuadNodeColor);
	QuadColorDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	QuadColorDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = _pDevice->CreateBuffer(&QuadColorDesc, nullptr, &m_pQuadColor);
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

	D3DXVECTOR4 *pNewQuadColor = GetColor(m_uiQuadColorCounter);
	D3D11_MAPPED_SUBRESOURCE mappedQuadColorRes;
	hr = _pDeviceContext->Map(m_pQuadColor, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedQuadColorRes);
	if (hr != S_OK)
		return;
	stQuadNodeColor *pQuadColor = (stQuadNodeColor *)mappedQuadColorRes.pData;
	pQuadColor->vColor = *pNewQuadColor;
	_pDeviceContext->Unmap(m_pQuadColor, 0);
	_pDeviceContext->PSSetConstantBuffers(1, 1, &m_pQuadColor);

	_pDeviceContext->VSSetShader(m_pVS, nullptr, 0);
	_pDeviceContext->PSSetShader(m_pPS, nullptr, 0);

	//See declaration of iVertexCount
	_pDeviceContext->DrawIndexed(_pNode->iVertexCount, 0, 0);
	
	delete pNewQuadColor;
}

void Dx11_QuadTree::RenderQuadNodeWithTessellation(ID3D11DeviceContext * _pDeviceContext, stNode * _pNode)
{
	unsigned int unChildCount = 0; //Child Count of Node _pNode	
	for (int i = 0; i < 4; i++)
	{
		if (_pNode->m_pChild[i])
		{
			unChildCount++;
			RenderQuadNodeWithTessellation(_pDeviceContext, _pNode->m_pChild[i]);
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
	_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST/*D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST*/);
	
	float fLod = 0.0f;
	float fCameraDistance = D3DXVec3Length(&(m_vCameraPos - D3DXVECTOR3(_pNode->fCenterX, 0, _pNode->fCenterZ)));
	if (fCameraDistance > 10.0f)
		fLod = 1.0;
	else if (fCameraDistance <= 10.0f && fCameraDistance > 7.0f)
		fLod = 2.0;
	else if (fCameraDistance <= 7.0f && fCameraDistance > 4.0f)
		fLod = 4.0;
	else if (fCameraDistance <= 4.0f && fCameraDistance > 1.0f)
		fLod = 8.0;
	else if (fCameraDistance <= 1.0f)
		fLod = 16.0;


	//float fLod = pow(2.0, 30.0f - fCameraDistance);
	HRESULT hr;
	//if (m_bUpdateTessValue)
	{
		m_bUpdateTessValue = false;
		D3D11_MAPPED_SUBRESOURCE mappedTessRes;
		hr = _pDeviceContext->Map(m_pTessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedTessRes);
		if (hr != S_OK)
			return;
		stTessellationBuffer *pTessData = (stTessellationBuffer *)mappedTessRes.pData;
		pTessData->fTessellationAmount = fLod;
		for (int i = 0; i < 3; i++)
			pTessData->fPadding[i] = 0.0f;
		_pDeviceContext->Unmap(m_pTessellationBuffer, 0);
	}

	_pDeviceContext->HSSetConstantBuffers(0, 1, &m_pTessellationBuffer);		
	_pDeviceContext->DSSetConstantBuffers(1, 1, &m_pWVPBuffer);

	
	
	/*m_uiQuadColorCounter++;
	if (m_uiQuadColorCounter > 3)
		m_uiQuadColorCounter = 0;
	D3DXVECTOR4 *pNewQuadColor = GetColor(m_uiQuadColorCounter);
	D3D11_MAPPED_SUBRESOURCE mappedQuadColorRes;
	hr = _pDeviceContext->Map(m_pQuadColor, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedQuadColorRes);
	if (hr != S_OK)
		return;
	stQuadNodeColor *pQuadColor = (stQuadNodeColor *)mappedQuadColorRes.pData;
	pQuadColor->vColor = *pNewQuadColor;
	_pDeviceContext->Unmap(m_pQuadColor, 0);
	_pDeviceContext->PSSetConstantBuffers(3, 1, &m_pQuadColor);*/

	_pDeviceContext->VSSetShader(m_pVS, nullptr, 0);
	_pDeviceContext->HSSetShader(m_pHS, nullptr, 0);
	_pDeviceContext->DSSetShader(m_pDS, nullptr, 0);
	_pDeviceContext->PSSetShader(m_pPS, nullptr, 0);

	//See declaration of iVertexCount
	_pDeviceContext->DrawIndexed(_pNode->iVertexCount, 0, 0);

	//Remove the Shader from the Pipeline
	_pDeviceContext->VSSetShader(nullptr, nullptr, 0);
	_pDeviceContext->HSSetShader(nullptr, nullptr, 0);
	_pDeviceContext->DSSetShader(nullptr, nullptr, 0);
	_pDeviceContext->PSSetShader(nullptr, nullptr, 0);
	//delete pNewQuadColor;
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

void Dx11_QuadTree::CreateTreeNode(ID3D11Device * _pDevice, stNode **_pNode, float _fCenterX, float _fCenterZ, float _fWidth, unsigned int _uiLOD)
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
	
	if (uiTriangles <= m_cMAX_TRIANGLES_PER_NODE)
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
		_uiLOD++;
		//Create Four Child
		for (int iChildIndex = 0; iChildIndex < 4; iChildIndex++)
		{
			float fOffsetX = ((iChildIndex % 2 == 0) ? -1 : 1) *_fWidth / 4;
			float fOffsetZ = ((iChildIndex % 4 < 2) ? -1 : 1 ) *_fWidth / 4;

			unsigned int iTriangles = CountTriangle((_fCenterX + fOffsetX), (_fCenterZ + fOffsetZ), (_fWidth / 2.0));
			if (iTriangles > 0)
				CreateTreeNode(_pDevice, &(*_pNode)->m_pChild[iChildIndex], _fCenterX + fOffsetX, _fCenterZ + fOffsetZ, _fWidth / 2.0, _uiLOD);
		}		
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
	
	//if centerX and centerZ lies in between the vertices 
	//then boundary condition will check with margin one as distance between vertices is one.
	if ((minX + 1 >= _fCenterX - fRadius) && (minZ + 1 >= _fCenterZ - fRadius) && (maxX - 1 <= _fCenterX + fRadius) && (maxZ - 1 <= _fCenterZ + fRadius))
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

D3DXVECTOR4* Dx11_QuadTree::GetColor(unsigned int _index)
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
	return pColor;
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
		
#ifndef _TESSELLATION_
	////Init Shader
	if (!IntializeShader(_pDevice))
		return false;
#else
	//Init Tesselation Shader
	if (!InitializaTessalationShader(_pDevice))
		return false;
#endif

	


	//Build Tree
	m_uiVertexCount = _pTerrain->GetVertexCount();
	m_pVertexList = new Dx11_Terrain::stVertex[m_uiVertexCount];
	_pTerrain->CopyVertices(m_pVertexList);
	

	float centerX = 0.0f, centerZ = 0.0f, width = 0.0f;
	CalculateMeshDimension(m_uiVertexCount, centerX, centerZ, width);
	unsigned int lod = 0;
	CreateTreeNode(_pDevice, &m_pRootNode, centerX, centerZ, width, lod);

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

void Dx11_QuadTree::Render(ID3D11DeviceContext * _pDeviceContext, D3DXMATRIX _worldMat, D3DXMATRIX _viewMat, D3DXMATRIX _projMat, D3DXVECTOR3 _vCam)
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
		
	D3D11_MAPPED_SUBRESOURCE mappedWVPResource;
	HRESULT hr = _pDeviceContext->Map(m_pWVPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedWVPResource);
	if (hr != S_OK)
		return;
	stWVPBuffer *pWVPData = (stWVPBuffer *)mappedWVPResource.pData;
	pWVPData->worldMat = m_worldMat;
	pWVPData->viewMat = m_viewMat;
	pWVPData->projMat = m_projMat;
	_pDeviceContext->Unmap(m_pWVPBuffer, 0);
	
	m_vCameraPos = _vCam;

#ifndef _TESSELLATION_
	RenderQuadNode(_pDeviceContext, m_pRootNode);
#else
	RenderQuadNodeWithTessellation(_pDeviceContext, m_pRootNode);
#endif // !_TESS

}

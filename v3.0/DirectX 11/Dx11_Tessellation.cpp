
#include "Dx11_Tessellation.h"
#include <fstream>

#include "Dx11_Terrain.h"
#include "Dx11_QuadTree.h"


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


bool Dx11_Tessellation::IntializeShader(ID3D11Device * _pDevice)
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

	//hr = _pDevice->CreateHullShader(pHSBuffer->GetBufferPointer(), pHSBuffer->GetBufferSize(), nullptr, &m_pHS);
	//if (hr != S_OK)
	//	return false;

	//hr = _pDevice->CreateDomainShader(pDSBuffer->GetBufferPointer(), pDSBuffer->GetBufferSize(), nullptr, &m_pDS);
	//if (hr != S_OK)
	//	return false;

	hr = _pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), nullptr, &m_pPS);
	if (hr != S_OK)
		return false;

	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		/*{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }		*/

		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(inputLayout);
	hr = _pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pInputLayout);
	if (hr != S_OK)
		return false;

	pVSBuffer->Release();
	/*pHSBuffer->Release();
	pDSBuffer->Release();*/
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
		
	D3D11_BUFFER_DESC TickbuffDesc;
	ZeroMemory(&TickbuffDesc, sizeof(TickbuffDesc));
	TickbuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	TickbuffDesc.ByteWidth = sizeof(stTessellationBuffer);
	TickbuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	TickbuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = _pDevice->CreateBuffer(&TickbuffDesc, nullptr, &m_pTickBuffer);
	if (hr != S_OK)
		return false;


	return true;
}

bool Dx11_Tessellation::IntializeGeometry(ID3D11Device * _pDevice)
{
	int nVertexCount = 3;
	stVertex *pVertex = new stVertex[nVertexCount];
	pVertex[0].Pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVertex[0].Color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

	pVertex[1].Pos = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	pVertex[1].Color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

	pVertex[2].Pos = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVertex[2].Color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

	D3D11_BUFFER_DESC vertexBuffDesc;
	ZeroMemory(&vertexBuffDesc, sizeof(vertexBuffDesc));
	vertexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBuffDesc.ByteWidth = sizeof(stVertex) * nVertexCount;
	vertexBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	
	D3D11_SUBRESOURCE_DATA vertexBuffRes;
	ZeroMemory(&vertexBuffRes, sizeof(vertexBuffRes));
	vertexBuffRes.pSysMem = pVertex;
	HRESULT hr = _pDevice->CreateBuffer(&vertexBuffDesc, &vertexBuffRes, &m_pVertexBuffer);
	if (hr != S_OK)
		return false;

	m_uiIndexCount = 3;
	unsigned int *pIndex = new unsigned int[m_uiIndexCount];
	pIndex[0] = 0;
	pIndex[1] = 1;
	pIndex[2] = 2;
	D3D11_BUFFER_DESC indexBuffDesc;
	ZeroMemory(&indexBuffDesc, sizeof(indexBuffDesc));
	indexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBuffDesc.ByteWidth = sizeof(unsigned int) * m_uiIndexCount;
	indexBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA indexBuffRes;
	ZeroMemory(&indexBuffRes, sizeof(indexBuffRes));
	indexBuffRes.pSysMem = pIndex;
	hr = _pDevice->CreateBuffer(&indexBuffDesc, &indexBuffRes, &m_pIndexBuffer);
	if (hr != S_OK)
		return false;
	
	delete pVertex;
	delete pIndex;

	return true;
}

bool Dx11_Tessellation::IntializeQuad(ID3D11Device * _pDevice)
{
	int nVertexCount = 8;
	stVertex *pVertex = new stVertex[nVertexCount];
	pVertex[0].Pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVertex[0].Color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

	pVertex[1].Pos = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	pVertex[1].Color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

	pVertex[2].Pos = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	pVertex[2].Color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

	pVertex[3].Pos = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	pVertex[3].Color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

	pVertex[4].Pos = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	pVertex[4].Color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

	pVertex[5].Pos = D3DXVECTOR3(2.0f, 0.0f, 0.0f);
	pVertex[5].Color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

	pVertex[6].Pos = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	pVertex[6].Color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

	pVertex[7].Pos = D3DXVECTOR3(2.0f, 1.0f, 0.0f);
	pVertex[7].Color = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);

	D3D11_BUFFER_DESC vertexBuffDesc;
	ZeroMemory(&vertexBuffDesc, sizeof(vertexBuffDesc));
	vertexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBuffDesc.ByteWidth = sizeof(stVertex) * nVertexCount;
	vertexBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA vertexBuffRes;
	ZeroMemory(&vertexBuffRes, sizeof(vertexBuffRes));
	vertexBuffRes.pSysMem = pVertex;
	HRESULT hr = _pDevice->CreateBuffer(&vertexBuffDesc, &vertexBuffRes, &m_pVertexBuffer);
	if (hr != S_OK)
		return false;

	m_uiIndexCount = 6*2;
	unsigned int *pIndex = new unsigned int[m_uiIndexCount];
	pIndex[0] = 0;
	pIndex[1] = 1;
	pIndex[2] = 3;
	pIndex[3] = 3;
	pIndex[4] = 2;
	pIndex[5] = 0;
	pIndex[6] = 4;
	pIndex[7] = 5;
	pIndex[8] = 7;
	pIndex[9] = 7;
	pIndex[10] = 6;
	pIndex[11] = 4;

	D3D11_BUFFER_DESC indexBuffDesc;
	ZeroMemory(&indexBuffDesc, sizeof(indexBuffDesc));
	indexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBuffDesc.ByteWidth = sizeof(unsigned int) * m_uiIndexCount;
	indexBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	D3D11_SUBRESOURCE_DATA indexBuffRes;
	ZeroMemory(&indexBuffRes, sizeof(indexBuffRes));
	indexBuffRes.pSysMem = pIndex;
	hr = _pDevice->CreateBuffer(&indexBuffDesc, &indexBuffRes, &m_pIndexBuffer);
	if (hr != S_OK)
		return false;

	delete pVertex;
	delete pIndex;

	return true;
}


Dx11_Tessellation::Dx11_Tessellation()
	:m_pTerrain(new Dx11_Terrain()),
		m_pQuadTree(new Dx11_QuadTree())
{
	
}


Dx11_Tessellation::~Dx11_Tessellation()
{
	//m_pTerrain->Release();
	delete m_pTerrain;
}

bool Dx11_Tessellation::Init(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
{
	bool bFlag = false;
	//Init Shader
	if (IntializeShader(_pDevice) && 0)
	{
		////Init Geomtery
		//if (IntializeGeometry(_pDevice))
		//	return true;		

		////Init 2 Quads
		//if (IntializeQuad(_pDevice))
		//	return true;

		/*
		if (m_pTerrain->Init(_pDevice))
		{
			unsigned int vertexCount = m_pTerrain->GetVertexCount();
			Dx11_Terrain::stVertex *pVertex = new Dx11_Terrain::stVertex[vertexCount];
			m_pTerrain->CopyVertices(pVertex);

			D3D11_BUFFER_DESC vertexBuffDesc;
			ZeroMemory(&vertexBuffDesc, sizeof(vertexBuffDesc));
			vertexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBuffDesc.ByteWidth = sizeof(Dx11_Terrain::stVertex) * vertexCount;
			vertexBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			D3D11_SUBRESOURCE_DATA vertexBuffRes;
			ZeroMemory(&vertexBuffRes, sizeof(vertexBuffRes));
			vertexBuffRes.pSysMem = pVertex;
			HRESULT hr = _pDevice->CreateBuffer(&vertexBuffDesc, &vertexBuffRes, &m_pVertexBuffer);
			if (hr != S_OK)
				return false;

			m_uiIndexCount = vertexCount;
			unsigned int *pIndex = new unsigned int[m_uiIndexCount];
			for (unsigned int i = 0; i < m_uiIndexCount; i++)
				pIndex[i] = i;

			D3D11_BUFFER_DESC indexBuffDesc;
			ZeroMemory(&indexBuffDesc, sizeof(indexBuffDesc));
			indexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBuffDesc.ByteWidth = sizeof(unsigned int) * m_uiIndexCount;
			indexBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			D3D11_SUBRESOURCE_DATA indexBuffRes;
			ZeroMemory(&indexBuffRes, sizeof(indexBuffRes));
			indexBuffRes.pSysMem = pIndex;
			hr = _pDevice->CreateBuffer(&indexBuffDesc, &indexBuffRes, &m_pIndexBuffer);
			if (hr != S_OK)
				return false;

			delete pVertex;
			delete pIndex;

			return true;
		}*/
		
	}

	//QuadTree
	bFlag = m_pQuadTree->BuildQuadTree(_pDevice, m_pTerrain);	
	
	return bFlag;
}

void Dx11_Tessellation::Render(ID3D11DeviceContext * _pDeviceContext, float _fTick, D3DXMATRIX _worldMat, D3DXMATRIX _viewMat, D3DXMATRIX _projMat, float _fCameraDistance)
{
	if (_pDeviceContext)
	{
		_pDeviceContext->IASetInputLayout(m_pInputLayout);

		//SetVertex Buffer
		UINT stride = sizeof(Dx11_Terrain::stVertex);
		UINT offset = 0;
		_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
		

				
		//Set Buffer To Hull Shader
		D3D11_MAPPED_SUBRESOURCE mappedTessRes;
		HRESULT hr = _pDeviceContext->Map(m_pTessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedTessRes);
		if (hr != S_OK)
			return;
		stTessellationBuffer *pTessData = (stTessellationBuffer *)mappedTessRes.pData;
		pTessData->fTessellationAmount = 1.0f;// (float)m_uiTessValue;//10.0f
		pTessData->fCameraDistance = _fCameraDistance;
		for (int i = 0; i < 2; i++)
			pTessData->fPadding[i] = 0.0f;
		_pDeviceContext->Unmap(m_pTessellationBuffer, 0);
		_pDeviceContext->HSSetConstantBuffers(0, 1, &m_pTessellationBuffer);
		//////////////////////////////////////////////////////////////////////

		//Set Buffer To Domain Shader
		D3DXMatrixTranspose(&_worldMat, &_worldMat);
		D3DXMatrixTranspose(&_viewMat, &_viewMat);
		D3DXMatrixTranspose(&_projMat, &_projMat);		
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		hr = _pDeviceContext->Map(m_pWVPBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (hr != S_OK)
			return;
		stWVPBuffer *pData = (stWVPBuffer *)mappedResource.pData;
		pData->worldMat = _worldMat;
		pData->viewMat = _viewMat;
		pData->projMat = _projMat;
		_pDeviceContext->Unmap(m_pWVPBuffer, 0);
		_pDeviceContext->DSSetConstantBuffers(1, 1, &m_pWVPBuffer);
		

		D3D11_MAPPED_SUBRESOURCE mappedTickResource;
		hr = _pDeviceContext->Map(m_pTickBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedTickResource);
		if (hr != S_OK)
			return;
		stTickBuffer *pTickData = (stTickBuffer *)mappedTickResource.pData;
		pTickData->fTick = _fTick;
		pTickData->vPadding = D3DXVECTOR3(0.0f, 0.0f, 0.0f);		
		_pDeviceContext->Unmap(m_pTickBuffer, 0);
		_pDeviceContext->DSSetConstantBuffers(2, 1, &m_pTickBuffer);
		//////////////////////////////////////////////////////////////////////

		_pDeviceContext->VSSetShader(m_pVS, nullptr, 0);
		_pDeviceContext->HSSetShader(m_pHS, nullptr, 0);
		_pDeviceContext->DSSetShader(m_pDS, nullptr, 0);
		_pDeviceContext->PSSetShader(m_pPS, nullptr, 0);

		//_uIndexCount : NOT USING HERE : SHOULD BE REMOVE AS PARAMTER
		_pDeviceContext->DrawIndexed(m_uiIndexCount, 0, 0);


		//Remove the Shader from the Pipeline
		_pDeviceContext->VSSetShader(nullptr, nullptr, 0);
		_pDeviceContext->HSSetShader(nullptr, nullptr, 0);
		_pDeviceContext->DSSetShader(nullptr, nullptr, 0);
		_pDeviceContext->PSSetShader(nullptr, nullptr, 0);
	}
}

void Dx11_Tessellation::Render(ID3D11DeviceContext * _pDeviceContext, float _fTick, D3DXMATRIX _worldMat, D3DXMATRIX _viewMat, D3DXMATRIX _projMat)
{
	if (_pDeviceContext)
	{	
		//RenderNode		
		m_pQuadTree->Render(_pDeviceContext, _fTick, _worldMat, _viewMat, _projMat);
	}
}

void Dx11_Tessellation::Release()
{
	if (m_pQuadTree)
	{
		m_pQuadTree->Release();
		delete m_pQuadTree;
		m_pQuadTree = nullptr;
	}

	if (m_pTerrain)
	{
		m_pTerrain->Release();
		delete m_pTerrain;
		m_pTerrain = nullptr;
	}


	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_pVertexBuffer = nullptr;
	}

	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_pIndexBuffer = nullptr;
	}

	if (m_pTessellationBuffer)
	{
		m_pTessellationBuffer->Release();
		m_pTessellationBuffer = nullptr;
	}

	if (m_pWVPBuffer)
	{
		m_pWVPBuffer->Release();
		m_pWVPBuffer = nullptr;
	}

	if (m_pInputLayout)
	{
		m_pInputLayout->Release();
		m_pInputLayout = nullptr;
	}

	if (m_pPS)
	{
		m_pPS->Release();
		m_pPS = nullptr;
	}

	if (m_pDS)
	{
		m_pDS->Release();
		m_pDS = nullptr;
	}

	if (m_pHS)
	{
		m_pHS->Release();
		m_pHS = nullptr;
	}

	if (m_pVS)
	{
		m_pVS->Release();
		m_pVS = nullptr;
	}
}

void Dx11_Tessellation::UpdateTessellationFactor(int _iDelta)
{
	m_uiTessValue += _iDelta;
	if (m_uiTessValue <= 1)
		m_uiTessValue = 1;
}

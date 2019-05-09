#include "Dx11_Model.h"


Dx11_Model::Dx11_Model()
{
	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;
	m_nVertexCount = 0;
	m_nIndexCount = 0;

	m_pTexture = new Dx11_Texture();



	//FLOOR 
	m_pFloorVertexBuffer = NULL;
	m_pFloorIndexBuffer = NULL;	
	m_nFloorVertexCount = m_nFloorIndexCount = 0;

	m_fHeading = 0.0;
}


Dx11_Model::~Dx11_Model()
{
}


bool Dx11_Model::CreateVertexBuffer(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContext, D3DXCOLOR VertexColor, float LightRadius)
{

	stVERTEX *pVertex;
	unsigned long *pIndex;

	m_nVertexCount = 4;
	m_nIndexCount = 6;

	pVertex = new stVERTEX[m_nVertexCount];
	pIndex = new unsigned long[m_nIndexCount];


	//D3DXCOLOR clr = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	//float  radius = 0.5;
	float  radius = LightRadius;
	D3DXCOLOR clr = VertexColor;

	pVertex[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVertex[0].diffuse = clr;
	pVertex[0].tex = D3DXVECTOR2(0.0f, 0.0f);

	pVertex[1].pos = D3DXVECTOR3(radius, 0.0f, 0.0f);
	pVertex[1].diffuse = clr;
	pVertex[1].tex = D3DXVECTOR2(1.0f, 0.0f);

	pVertex[2].pos = D3DXVECTOR3(radius, -1 * radius, 0.0f);
	pVertex[2].diffuse = clr;
	pVertex[2].tex = D3DXVECTOR2(1.0f, 1.0f);


	pVertex[3].pos = D3DXVECTOR3(0.0f, -1 * radius, 0.0f);
	pVertex[3].diffuse = clr;
	pVertex[3].tex = D3DXVECTOR2(0.0f, 1.0f);



	pIndex[0] = 0;
	pIndex[1] = 1;
	pIndex[2] = 2;
	pIndex[3] = 2;
	pIndex[4] = 3;
	pIndex[5] = 0;



	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(stVERTEX)* m_nVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = pVertex;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	HRESULT hr = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &m_pVertexBuffer);
	if (hr != S_OK)
		return false;

	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long)* m_nIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = pIndex;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	hr = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &m_pIndexBuffer);
	if (hr != S_OK)
		return false;


	UINT stride = sizeof(stVERTEX);
	UINT offset = 0;

	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	delete[] pVertex;
	delete[] pIndex;
	pVertex = 0;
	pIndex = 0;


	return true;
}



bool Dx11_Model::Initialize(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContext, WCHAR *textureFile, WCHAR *textureFile2)
{
	
	D3DXCOLOR clr = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	float  radius = 0.5;

	if (m_pTexture->Initiazlize(pDevice, textureFile) == false)
	{
		MessageBox(NULL, L"Texture Load Fail", L"Error", MB_ICONERROR | MB_OK);
	}

	//ADD FOR LOAD ANOTHER TEXTURE TO TEST
	if (textureFile2)
	{
		m_pTexture2->Initiazlize(pDevice, textureFile2);
	}
	
	bool reValue = CreateVertexBuffer(pDevice, pDeviceContext, clr, radius);

	

	return reValue;
}


void Dx11_Model::Shutdown()
{

	if (m_pTexture)
	{
		m_pTexture->Shutdown();
		m_pTexture = 0;
	}

	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
		m_nVertexCount = 0;
	}

	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
		m_nIndexCount = 0;
	}

}


void Dx11_Model::Render(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContext, D3DXCOLOR color, float fSize)
{

	if (pDeviceContext &&pDevice)
	{
		if (m_pVertexBuffer)
		{
			m_pVertexBuffer->Release();
			m_nVertexCount = 0;
		}

		if (m_pIndexBuffer)
		{
			m_pIndexBuffer->Release();
			m_nIndexCount = 0;
		}
		bool reValue = CreateVertexBuffer(pDevice, pDeviceContext, color, fSize);		
	}

}


bool Dx11_Model::LoadFloor(ID3D11Device *pDevice, WCHAR *pTextureFile)
{


	if(m_pTexture->Initiazlize(pDevice, pTextureFile) == false)
	{
		MessageBox(NULL, L"Texture Load Fail", L"Error", MB_ICONERROR | MB_OK);
	}

	stFloorVertex *pVertices;
	unsigned int  *pIndices;

	int nPoints = 10;
	m_nFloorVertexCount = 4;// nPoints * nPoints;// 4;
	m_nFloorIndexCount = 6;// (nPoints - 1)* (nPoints - 1) * 6;//6;
	pVertices = new stFloorVertex[m_nFloorVertexCount];
	pIndices = new unsigned int[m_nFloorIndexCount];
	pVertices[0].pos = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
	pVertices[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVertices[0].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);


	pVertices[1].pos = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	pVertices[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVertices[1].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	pVertices[2].pos = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
	pVertices[2].tex = D3DXVECTOR2(1.0f, 1.0f);
	pVertices[2].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);


	pVertices[3].pos = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
	pVertices[3].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVertices[3].normal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 2;
	pIndices[4] = 3;
	pIndices[5] = 0;

	//int r, c, index = 0;
	//for (c = 0; c < nPoints; c++)
	//{
	//	for (r = 0; r < nPoints; r++)
	//	{
	//		pVertices[index].pos = D3DXVECTOR3((float)c, 0, (float)(r));
	//		pVertices[index].tex = D3DXVECTOR2((float)(c + 1) / (float)nPoints, (float)(r + 1) / (float)nPoints);
	//		pVertices[index].normal = D3DXVECTOR3(1, 0, 0);
	//		index++;
	//	}
	//}

	//index = 0;
	//for (r = 0; r < nPoints - 1; r++)
	//{
	//	for (c = 0; c < nPoints - 1; c++)
	//	{
	//		pIndices[index++] = r*nPoints + c;
	//		pIndices[index++] = r*nPoints + (c + 1);
	//		pIndices[index++] = (r + 1)*nPoints + c;

	//		pIndices[index++] = (r + 1)*nPoints + c;
	//		pIndices[index++] = r*nPoints + (c + 1);
	//		pIndices[index++] = (r + 1)*nPoints + (c + 1);
	//	}
	//}
	D3D11_BUFFER_DESC desc, indxDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(stFloorVertex)* m_nFloorVertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;


	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	HRESULT hr = pDevice->CreateBuffer(&desc, &vertexData, &m_pFloorVertexBuffer);
	if (hr != S_OK)
		return false;

	ZeroMemory(&indxDesc, sizeof(indxDesc));
	indxDesc.Usage = D3D11_USAGE_DEFAULT;
	indxDesc.ByteWidth = sizeof(unsigned int)* m_nFloorIndexCount;
	indxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indxDesc.CPUAccessFlags = 0;
	indxDesc.MiscFlags = 0;
	indxDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	hr = pDevice->CreateBuffer(&indxDesc, &indexData, &m_pFloorIndexBuffer);
	if (hr != S_OK)
		return false;

	
	delete[] pVertices;
	delete[] pIndices;
	pVertices = 0;
	pIndices = 0;


	return true;
}



void Dx11_Model::RenderFloor( ID3D11DeviceContext *pDeviceContext)
{
	if (pDeviceContext)
	{
		UINT stride = sizeof(stFloorVertex);
		UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pFloorVertexBuffer, &stride, &offset);
		pDeviceContext->IASetIndexBuffer(m_pFloorIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

}




bool Dx11_Model::LoadCube(ID3D11Device *pDevice, WCHAR *pTextureFile)
{

	if (m_pTexture->Initiazlize(pDevice, pTextureFile) == false)
	{
		MessageBox(NULL, L"Texture Load Fail", L"Error", MB_ICONERROR | MB_OK);
	}

	m_nCubeVertexCount =  24;
	m_nCubeIndexCount =  36;
	//Create vertex buffer
	stCubeVertex pVertices[] =
	{
		{ D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f) },
		{ D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f) },
		{ D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f) },
		{ D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f) },

		{ D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f) },
		{ D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f) },
		{ D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f) },
		{ D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f) },

		{ D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f) },
		{ D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f) },
		{ D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f) },
		{ D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f) },

		{ D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f) },
		{ D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f) },
		{ D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f) },
		{ D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f) },

		{ D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f) },
		{ D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f) },
		{ D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f) },
		{ D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f) },

		{ D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f) },
		{ D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f) },
		{ D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f) },
		{ D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f) },
	};
	D3D11_BUFFER_DESC desc, indxDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(stCubeVertex)* m_nCubeVertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;


	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	HRESULT hr = pDevice->CreateBuffer(&desc, &vertexData, &m_pCubeVertexBuffer);
	if (hr != S_OK)
		return false;


	//// Create index buffer 
	unsigned int pIndices[] =
	{
		3, 1, 0,
		2, 1, 3,

		6, 4, 5,
		7, 4, 6,

		11, 9, 8,
		10, 9, 11,

		14, 12, 13,
		15, 12, 14,

		19, 17, 16,
		18, 17, 19,

		22, 20, 21,
		23, 20, 22
	};
	ZeroMemory(&indxDesc, sizeof(indxDesc));
	indxDesc.Usage = D3D11_USAGE_DEFAULT;
	indxDesc.ByteWidth = sizeof(unsigned int)* m_nCubeIndexCount;
	indxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indxDesc.CPUAccessFlags = 0;
	indxDesc.MiscFlags = 0;
	indxDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	hr = pDevice->CreateBuffer(&indxDesc, &indexData, &m_pCubeIndexBuffer);
	if (hr != S_OK)
		return false;


	//delete[] pVertices;
	//delete[] pIndices;
	//pVertices = 0;
	//pIndices = 0;


	return true;

}



void Dx11_Model::RenderCube(ID3D11DeviceContext *pDeviceContext)
{
	if (pDeviceContext)
	{
		unsigned int stride = sizeof(stCubeVertex);
		unsigned int offset = 0;

		pDeviceContext->IASetVertexBuffers(0, 1, &m_pCubeVertexBuffer, &stride, &offset);
		pDeviceContext->IASetIndexBuffer(m_pCubeIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

}



void Dx11_Model::ProcessCube(float _fTick)
{
	if (m_fHeading < 6.28)
		m_fHeading += _fTick;
	else
		m_fHeading = 0.0f;
}


bool Dx11_Model::LoadFloorDepthTest(ID3D11Device *pDevice)
{
	m_nVertexCount = 6;
	m_nIndexCount = 12;

	stDepthVertex *pVertices = new stDepthVertex[m_nVertexCount];
	unsigned int *pIndices = new unsigned int [m_nIndexCount];


	pVertices[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	pVertices[1].pos = D3DXVECTOR3(1.0f, 0.0f,0.0f);
	pVertices[2].pos = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	pVertices[3].pos = D3DXVECTOR3(1.0f, 0.0f, 1.0f);
	pVertices[4].pos = D3DXVECTOR3(0.0f, 0.0f, 2.0f);
	pVertices[5].pos = D3DXVECTOR3(1.0f, 0.0f, 2.0f);


	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 2;
	pIndices[4] = 1;
	pIndices[5] = 3;

	pIndices[6] = 2;
	pIndices[7] = 3;
	pIndices[8] = 4;
	pIndices[9] = 4;
	pIndices[10] = 3;
	pIndices[11] = 5;

	
	D3D11_BUFFER_DESC desc, indxDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(stDepthVertex)* m_nVertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;


	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	HRESULT hr = pDevice->CreateBuffer(&desc, &vertexData, &m_pVertexBuffer);
	if (hr != S_OK)
		return false;

	ZeroMemory(&indxDesc, sizeof(indxDesc));
	indxDesc.Usage = D3D11_USAGE_DEFAULT;
	indxDesc.ByteWidth = sizeof(unsigned int)* m_nIndexCount;
	indxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indxDesc.CPUAccessFlags = 0;
	indxDesc.MiscFlags = 0;
	indxDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	hr = pDevice->CreateBuffer(&indxDesc, &indexData, &m_pIndexBuffer);
	if (hr != S_OK)
		return false;


	delete[] pVertices;
	delete[] pIndices;
	pVertices = 0;
	pIndices = 0;

	return true;
}



void Dx11_Model::RenderDepth(ID3D11DeviceContext *pDeviceContext)
{
	if (pDeviceContext)
	{
		unsigned int stride, offset;
		stride = sizeof(stDepthVertex);
		offset = 0;

		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}



bool Dx11_Model::CreateLightShaftGrid(ID3D11Device *pDevice, unsigned int nPoints, WCHAR *pTextureFile)
{

	m_nVertexCount = nPoints * nPoints;
	stVERTEX_LIGHT_SHAFT_GRID *pVertices = new stVERTEX_LIGHT_SHAFT_GRID[m_nVertexCount];
	int r, c, index = 0;
	for (c = 0; c < nPoints; c++)
	{
		for (r = 0; r < nPoints; r++)
		{
			pVertices[index].pos	= D3DXVECTOR3( (float)c, 0 ,(float)(r));
			pVertices[index].tex = D3DXVECTOR2((float)(c+1) / (float)nPoints, (float)(r+1) / (float)nPoints);
			pVertices[index].normal = D3DXVECTOR3(1, 0, 0);
			pVertices[index].diffuse = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

			index++;
		}
	}

	pVertices[48 * 100 + 48].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[48 * 100 + 49].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[48 * 100 + 50].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[48 * 100 + 51].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[48 * 100 + 52].normal = D3DXVECTOR3(0, -1, 0);

	pVertices[49 * 100 + 48].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[49 * 100 + 49].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[49 * 100 + 50].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[49 * 100 + 51].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[49 * 100 + 52].normal = D3DXVECTOR3(0, -1, 0);


	pVertices[50 * 100 + 48].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[50 * 100 + 49].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[50 * 100 + 50].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[50 * 100 + 51].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[50 * 100 + 52].normal = D3DXVECTOR3(0, -1, 0);


	pVertices[51 * 100 + 48].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[51 * 100 + 49].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[51 * 100 + 50].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[51 * 100 + 51].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[51 * 100 + 52].normal = D3DXVECTOR3(0, -1, 0);


	pVertices[52 * 100 + 48].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[52 * 100 + 49].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[52 * 100 + 50].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[52 * 100 + 51].normal = D3DXVECTOR3(0, -1, 0);
	pVertices[52 * 100 + 52].normal = D3DXVECTOR3(0, -1, 0);


	D3D11_BUFFER_DESC vertexBuffDesc, indexBuffDesc;
	D3D11_SUBRESOURCE_DATA vertexSubData, indexSubData;
	ZeroMemory(&vertexBuffDesc, sizeof(vertexBuffDesc));
	vertexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBuffDesc.ByteWidth = sizeof(stVERTEX_LIGHT_SHAFT_GRID)* m_nVertexCount;
	vertexBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBuffDesc.CPUAccessFlags = 0;
	vertexBuffDesc.MiscFlags = 0;
	vertexBuffDesc.StructureByteStride = 0;

	ZeroMemory(&vertexSubData, sizeof(vertexSubData));
	vertexSubData.pSysMem = pVertices;
	vertexSubData.SysMemPitch = 0;
	vertexSubData.SysMemSlicePitch = 0;
	HRESULT hr = pDevice->CreateBuffer(&vertexBuffDesc, &vertexSubData, &m_pVertexBuffer);
	if (hr != S_OK)
		return false;


	
	m_nIndexCount = (nPoints - 1)* (nPoints - 1) * 6;
	unsigned long *pIndices = new unsigned long[m_nIndexCount];
	index = 0;
	for (r = 0; r<nPoints - 1; r++)
	{
		for (c = 0; c<nPoints - 1; c++)
		{			
			pIndices[index++] = r*nPoints + c;
			pIndices[index++] = r*nPoints + (c + 1);
			pIndices[index++] = (r + 1)*nPoints + c;
			
			pIndices[index++] = (r + 1)*nPoints + c;
			pIndices[index++] = r*nPoints + (c + 1);
			pIndices[index++] = (r + 1)*nPoints + (c + 1);
		}
	}

	ZeroMemory(&indexBuffDesc, sizeof(indexBuffDesc));
	indexBuffDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBuffDesc.ByteWidth = sizeof(unsigned long)* m_nIndexCount;
	indexBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBuffDesc.CPUAccessFlags = 0;
	indexBuffDesc.MiscFlags = 0;
	indexBuffDesc.StructureByteStride = 0;


	ZeroMemory(&indexSubData, sizeof(indexSubData));
	indexSubData.pSysMem = pIndices;
	indexSubData.SysMemPitch = 0;
	indexSubData.SysMemSlicePitch = 0;
	hr = pDevice->CreateBuffer(&indexBuffDesc, &indexSubData, &m_pIndexBuffer);
	if (hr != S_OK)
		return false;



	if (pTextureFile)
	{		
		if (m_pTexture->Initiazlize(pDevice, pTextureFile) == false)
		{
			MessageBox(NULL, L"Light Shaft Texture Fail...", L"error", MB_ICONEXCLAMATION);
		}

	}


	return true;
}



void Dx11_Model::RenderLightShaftGrid(ID3D11DeviceContext *pDeviceContext)
{
	if (pDeviceContext)
	{

		UINT stride, offset;
		stride = sizeof(stVERTEX_LIGHT_SHAFT_GRID);
		offset = 0;

		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	}
}



bool Dx11_Model::LoadSun(ID3D11Device *pDevice, WCHAR *pTextureFile, D3DXCOLOR sunColor)
{

	if (m_pTexture->Initiazlize(pDevice, pTextureFile)==false)
	{
		MessageBox(NULL, L"Texture Load Fail", L"Error", MB_ICONERROR | MB_OK);
	}

	m_nVertexCount = 4;
	m_nIndexCount = 6;

	D3DXVECTOR3 pos;
	D3DXCOLOR diffuse;
	D3DXVECTOR2 tex;

	stVERTEX *pSunVertices = new stVERTEX[m_nVertexCount];
	pSunVertices[0] = { D3DXVECTOR3(-1.0f, 1.0f, 0.0f), sunColor, D3DXVECTOR2(0.0f, 0.0f) };
	pSunVertices[1] = { D3DXVECTOR3(1.0f, 1.0f, 0.0f), sunColor, D3DXVECTOR2(1.0f, 0.0f) };
	pSunVertices[2] = { D3DXVECTOR3(-1.0f, -1.0f, 0.0f), sunColor, D3DXVECTOR2(0.0f, 1.0f) };
	pSunVertices[3] = { D3DXVECTOR3(1.0f, -1.0f, 0.0f), sunColor, D3DXVECTOR2(1.0f, 1.0f) };

	unsigned int *pIndices = new unsigned int[m_nIndexCount];
	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;

	pIndices[3] = 2;
	pIndices[4] = 1;
	pIndices[5] = 3;



	D3D11_BUFFER_DESC desc, indxDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(stVERTEX)* m_nVertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;


	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = pSunVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	HRESULT hr = pDevice->CreateBuffer(&desc, &vertexData, &m_pVertexBuffer);
	if (hr != S_OK)
		return false;

	ZeroMemory(&indxDesc, sizeof(indxDesc));
	indxDesc.Usage = D3D11_USAGE_DEFAULT;
	indxDesc.ByteWidth = sizeof(unsigned int)* m_nIndexCount;
	indxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indxDesc.CPUAccessFlags = 0;
	indxDesc.MiscFlags = 0;
	indxDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	hr = pDevice->CreateBuffer(&indxDesc, &indexData, &m_pIndexBuffer);
	if (hr != S_OK)
		return false;


	delete[] pSunVertices;
	delete[] pIndices;
	pSunVertices = 0;
	pIndices = 0;


	return true;
}


void Dx11_Model::RenderSun(ID3D11DeviceContext *pDeviceContext)
{
	UINT stride, offset;	
	stride = sizeof(stVERTEX);
	offset = 0;

	if (pDeviceContext)
	{
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		
	}

}


bool Dx11_Model::LoadDepthMapModel(ID3D11Device *pDevice)
{
	stVertexDepthMap *pVertices;
	unsigned long  *pIndices;

	
	m_nVertexCount = 4;
	m_nIndexCount = 6;
	pVertices = new stVertexDepthMap[m_nVertexCount];
	pIndices = new unsigned long[m_nIndexCount];

	pVertices[0].pos = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);
	pVertices[0].tex = D3DXVECTOR2(0.0f, 0.0f);	


	pVertices[1].pos = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	pVertices[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	

	pVertices[2].pos = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
	pVertices[2].tex = D3DXVECTOR2(1.0f, 1.0f);
	


	pVertices[3].pos = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
	pVertices[3].tex = D3DXVECTOR2(0.0f, 1.0f);	

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 2;
	pIndices[4] = 3;
	pIndices[5] = 0;

	D3D11_BUFFER_DESC desc, indxDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(stVertexDepthMap)* m_nVertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;


	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	HRESULT hr = pDevice->CreateBuffer(&desc, &vertexData, &m_pVertexBuffer);
	if (hr != S_OK)
		return false;

	ZeroMemory(&indxDesc, sizeof(indxDesc));
	indxDesc.Usage = D3D11_USAGE_DEFAULT;
	indxDesc.ByteWidth = sizeof(unsigned long)* m_nIndexCount;
	indxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indxDesc.CPUAccessFlags = 0;
	indxDesc.MiscFlags = 0;
	indxDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	hr = pDevice->CreateBuffer(&indxDesc, &indexData, &m_pIndexBuffer);
	if (hr != S_OK)
		return false;


	delete[] pVertices;
	delete[] pIndices;
	pVertices = 0;
	pIndices = 0;


	return true;
}


void Dx11_Model::RenderDepthMapModel(ID3D11DeviceContext *pDeviceContext)
{
	if (pDeviceContext)
	{
		unsigned int stride, offset;
		stride = sizeof(stVertexDepthMap);
		offset = 0;

		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}

bool Dx11_Model::LoadArrowModel(ID3D11Device * pDevice)
{

	m_pTexture->Initiazlize(pDevice, L"Data/fiarrow fwd.dds");
		
	stVertexDepthMap *pVertices;
	unsigned int  *pIndices;

	m_nVertexCount = 4;
	m_nIndexCount = 6;
	pVertices = new stVertexDepthMap[m_nVertexCount];
	pIndices = new unsigned int[m_nIndexCount];
	pVertices[0].pos = D3DXVECTOR3(-1.0f, 0.0f, 1.0f);
	pVertices[0].tex = D3DXVECTOR2(0.0f, 0.0f);

	pVertices[1].pos = D3DXVECTOR3(1.0f, 0.0f, 1.0f);
	pVertices[1].tex = D3DXVECTOR2(1.0f, 0.0f);	

	pVertices[2].pos = D3DXVECTOR3(1.0f, 0.0f, -1.0f);
	pVertices[2].tex = D3DXVECTOR2(1.0f, 1.0f);	


	pVertices[3].pos = D3DXVECTOR3(-1.0f, 0.0f, -1.0f);
	pVertices[3].tex = D3DXVECTOR2(0.0f, 1.0f);
	

	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 2;
	pIndices[4] = 3;
	pIndices[5] = 0;


	D3D11_BUFFER_DESC desc, indxDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(stVertexDepthMap)* m_nVertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;


	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	HRESULT hr = pDevice->CreateBuffer(&desc, &vertexData, &m_pVertexBuffer);
	if (hr != S_OK)
		return false;

	ZeroMemory(&indxDesc, sizeof(indxDesc));
	indxDesc.Usage = D3D11_USAGE_DEFAULT;
	indxDesc.ByteWidth = sizeof(unsigned int)* m_nIndexCount;
	indxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indxDesc.CPUAccessFlags = 0;
	indxDesc.MiscFlags = 0;
	indxDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	hr = pDevice->CreateBuffer(&indxDesc, &indexData, &m_pIndexBuffer);
	if (hr != S_OK)
		return false;


	delete[] pVertices;
	delete[] pIndices;
	pVertices = 0;
	pIndices = 0;

	return true;	
}

void Dx11_Model::RenderArrowModel(ID3D11DeviceContext * pDeviceContext)
{
	if (pDeviceContext)
	{
		UINT stride = sizeof(stVertexDepthMap);
		UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}

#include "Dx11_Font.h"



Dx11_Font::Dx11_Font()
{
	m_pFontType = new stFontType[95];
	m_pTexture = new Dx11_Texture();
}


Dx11_Font::~Dx11_Font()
{

}


Dx11_Text::Dx11_Text()
{
	m_pFont = new Dx11_Font();
	m_pFontShader = new Dx11_Shaders();

	m_nScreenWidth = m_nScreenHeight = 0;
}


Dx11_Text::~Dx11_Text()
{
	delete m_pFont;
	delete m_pFontShader;
}


bool Dx11_Font::Initialize(ID3D11Device *pDevice, char *pFontFile, WCHAR *pTextureFile)
{

	bool bFlag = false;
	bFlag = LoadFontData(pFontFile);
	if (!bFlag)
		return bFlag;

	bFlag = m_pTexture->Initiazlize(pDevice, pTextureFile);	
	if (!bFlag)	
		//MessageBox(NULL,L"Font Texture Load To Fail", L"Error", MB_ICONERROR );
		return bFlag;
	
		
	return true;
}

bool Dx11_Font::LoadFontData(char *pFontFile)
{
	std::ifstream fin;

	char ch;
	fin.open(pFontFile);
	if (fin)
	{
		for (int i = 0; i < 95; i++)
		{
			fin.get(ch);
			while (ch != ' ')
				fin.get(ch);

			fin.get(ch);
			while (ch != ' ')
				fin.get(ch);

			fin >> m_pFontType[i].uvLeft;
			fin >> m_pFontType[i].uvRight;
			fin >> m_pFontType[i].nFontWidth;

		}
		fin.close();
		return true;
	}
	else
		MessageBox(NULL, L"Font File To Load", L"Error", MB_ICONERROR);
	return false;
}


bool Dx11_Text::Initialize(ID3D11Device *pDevice, ID3D11DeviceContext *pContext,
												char *pFontFile, WCHAR *pTextureFile,
												D3DXMATRIX  baseViewMatrix,
												int screenWidth, int screenHeight
												)
{
	bool bFlag = false;
	if (m_pFont && m_pFontShader)
	{
		m_nScreenWidth = screenWidth;
		m_nScreenHeight = screenHeight;

		m_matBaseViewMatrix = baseViewMatrix;
		bFlag = m_pFont->Initialize(pDevice, pFontFile, pTextureFile);
		if (!bFlag)
			return false;

		bFlag = m_pFontShader->InitializeFontShader(pDevice, NULL, L"../../Data/MyFontShader.hlsl");
		if (!bFlag)
			return false;

		bFlag = InitializeSectence(pDevice, &m_pSentence1, 100);
		if (!bFlag)
			return false;
		UpdateSentence(pContext, m_pSentence1, "DIRECTX", 100, 100, D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f));


		/*bFlag = InitializeSectence(pDevice, &m_pSentence2, 16);
		if (!bFlag)
			return false;
		UpdateSentence(pContext, m_pSentence2, "Goodbye", 100, 200, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));*/

		
	}

	
	return bFlag;
}


bool Dx11_Text::InitializeSectence(ID3D11Device *pDevice, stSentenceType **pSentence, int maxSentenceLength)
{
	stVertexType* vertices;
	unsigned int* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;


	// Create a new sentence object.
	*pSentence = new stSentenceType;
	if (!*pSentence)
	{
		return false;
	}

	// Initialize the sentence buffers to null.
	(*pSentence)->pVB = 0;
	(*pSentence)->pIB = 0;

	// Set the maximum length of the sentence.
	(*pSentence)->nMaxlength = maxSentenceLength;

	// Set the number of vertices in the vertex array.
	(*pSentence)->nVertexCount = 4 * maxSentenceLength;

	// Set the number of indexes in the index array.
	(*pSentence)->nIndexCount = (*pSentence)->nVertexCount;

	// Create the vertex array.
	vertices = new stVertexType[(*pSentence)->nVertexCount];
	if (!vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned int[(*pSentence)->nIndexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize vertex array to zeros at first.
	memset(vertices, 0, (sizeof(stVertexType)* (*pSentence)->nVertexCount));

	// Initialize the index array.
	for (i = 0; i<(*pSentence)->nIndexCount; i++)
	{
		indices[i] = i;
	}

	// Set up the description of the dynamic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(stVertexType)* (*pSentence)->nVertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer.
	result = pDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &(*pSentence)->pVB);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int)* (*pSentence)->nIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = pDevice->CreateBuffer(&indexBufferDesc, &indexData, &(*pSentence)->pIB);
	if (FAILED(result))
	{
		return false;
	}

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;

	// Release the index array as it is no longer needed.
	delete[] indices;
	indices = 0;

	return true;
}


void Dx11_Font::BuildVertexBuffer(void *pVertex, char *text, float drawX, float drawY)
{

	stVertexType *pVertices = (stVertexType *)pVertex;
	int nLetters = strlen(text);


	int nIndex = 0;
	for (int i = 0; i < nLetters; i++)
	{
		int nFontIndex = ((int)text[i]) - 32;
		//	1-----2
		//  -     -
		//  -     -
		//  3-----4
		if (nFontIndex == 0)
		{
			drawX += 3.0f;
		}
		else
		{

			////First Triangle 1->2->3
			//pVertices[nIndex].vPosition = D3DXVECTOR3((float)drawX, (float)drawY, 0.0f);
			//pVertices[nIndex].vTexture = D3DXVECTOR2(m_pFontType[nFontIndex].uvLeft, 0);
			//nIndex++;

			//pVertices[nIndex].vPosition = D3DXVECTOR3((float)drawX + m_pFontType[nFontIndex].nFontWidth, (float)drawY, 0.0f);
			//pVertices[nIndex].vTexture = D3DXVECTOR2(m_pFontType[nFontIndex].uvRight, 0);
			//nIndex++;

			//pVertices[nIndex].vPosition = D3DXVECTOR3((float)drawX, (float)drawY + 16, 0.0f);
			//pVertices[nIndex].vTexture = D3DXVECTOR2(m_pFontType[nFontIndex].uvLeft, 1);
			//nIndex++;

			////Second Triangle 3->2->4
			//pVertices[nIndex].vPosition = D3DXVECTOR3((float)drawX, (float)drawY + 16, 0.0f);
			//pVertices[nIndex].vTexture = D3DXVECTOR2(m_pFontType[nFontIndex].uvLeft, 1);
			//nIndex++;

			//pVertices[nIndex].vPosition = D3DXVECTOR3((float)drawX + m_pFontType[nFontIndex].nFontWidth, (float)drawY, 0.0f);
			//pVertices[nIndex].vTexture = D3DXVECTOR2(m_pFontType[nFontIndex].uvRight, 0);
			//nIndex++;

			//pVertices[nIndex].vPosition = D3DXVECTOR3((float)drawX + m_pFontType[nFontIndex].nFontWidth, (float)drawY + 16, 0.0f);
			//pVertices[nIndex].vTexture = D3DXVECTOR2(m_pFontType[nFontIndex].uvRight, 1);
			//nIndex++;


			// First triangle in quad.
			pVertices[nIndex].vPosition = D3DXVECTOR3(drawX, drawY, 0.0f);  // Top left.
			pVertices[nIndex].vTexture = D3DXVECTOR2(m_pFontType[nFontIndex].uvLeft, 0.0f);
			nIndex++;

			pVertices[nIndex].vPosition = D3DXVECTOR3((drawX + m_pFontType[nFontIndex].nFontWidth), (drawY - 16), 0.0f);  // Bottom right.
			pVertices[nIndex].vTexture = D3DXVECTOR2(m_pFontType[nFontIndex].uvRight, 1.0f);
			nIndex++;

			pVertices[nIndex].vPosition = D3DXVECTOR3(drawX, (drawY - 16), 0.0f);  // Bottom left.
			pVertices[nIndex].vTexture = D3DXVECTOR2(m_pFontType[nFontIndex].uvLeft, 1.0f);
			nIndex++;

			// Second triangle in quad.
			pVertices[nIndex].vPosition = D3DXVECTOR3(drawX, drawY, 0.0f);  // Top left.
			pVertices[nIndex].vTexture = D3DXVECTOR2(m_pFontType[nFontIndex].uvLeft, 0.0f);
			nIndex++;

			pVertices[nIndex].vPosition = D3DXVECTOR3(drawX + m_pFontType[nFontIndex].nFontWidth, drawY, 0.0f);  // Top right.
			pVertices[nIndex].vTexture = D3DXVECTOR2(m_pFontType[nFontIndex].uvRight, 0.0f);
			nIndex++;

			pVertices[nIndex].vPosition = D3DXVECTOR3((drawX + m_pFontType[nFontIndex].nFontWidth), (drawY - 16), 0.0f);  // Bottom right.
			pVertices[nIndex].vTexture = D3DXVECTOR2(m_pFontType[nFontIndex].uvRight, 1.0f);
			nIndex++;

			drawX += m_pFontType[nFontIndex].nFontWidth + 2;
		}
	}
}

void Dx11_Text::UpdateSentence(ID3D11DeviceContext *pContext, stSentenceType *pSentence, char *text, int positionLeft, int positionTop, D3DXVECTOR4 color)
{

	//pSentence->color = color;
	pSentence->fR = color.x;
	pSentence->fG = color.y;
	pSentence->fB = color.z;

	stVertexType *pVertices = new stVertexType[pSentence->nVertexCount];
	memset(pVertices, 0, pSentence->nVertexCount * sizeof(stVertexType));	

	float drawX, drawY;
	drawX = (float)(((m_nScreenWidth/2)*-1)+positionLeft);
	drawY = (float)((m_nScreenHeight/2) - positionTop);	
	m_pFont->BuildVertexBuffer((void *)pVertices, text, drawX, drawY);
	//m_pFont->BuildVertexBuffer((void *)pVertices, text, positionLeft, positionTop);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pContext->Map(pSentence->pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	stVertexType *pVertexPtr = (stVertexType *)mappedResource.pData;
	memcpy(pVertexPtr, (void *)pVertices, sizeof(stVertexType)*pSentence->nVertexCount);
	pContext->Unmap(pSentence->pVB, 0);

	delete[] pVertices;
	pVertices = 0;
	
}


void Dx11_Text::Render(ID3D11DeviceContext *pContext, char *message, D3DXMATRIX worldMat, D3DXMATRIX orthoMat)
{
	
	UpdateSentence(pContext, m_pSentence1, message, m_nScreenWidth/2, 100, D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f));
	RenderSentence(pContext, m_pSentence1, worldMat, orthoMat);
	//RenderSentence(pContext, m_pSentence2, worldMat, orthoMat);
}



void Dx11_Text::RenderSentence(ID3D11DeviceContext *pContext, stSentenceType *pSecntence, D3DXMATRIX worldMat, D3DXMATRIX orthoMat)
{
	unsigned int offset, stride;
	stride = sizeof(stVertexType);
	offset = 0;


	pContext->IASetVertexBuffers(0, 1, &pSecntence->pVB, &stride, &offset);
	pContext->IASetIndexBuffer(pSecntence->pIB, DXGI_FORMAT_R32_UINT, 0);
	pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	D3DXMATRIX matScale;
	D3DXMatrixScaling(&matScale, 1.0f, 1.5f, 1.0f);
	//worldMat = worldMat*matScale;

	D3DXVECTOR4 color = D3DXVECTOR4(pSecntence->fR, pSecntence->fG, pSecntence->fB, 1.0);
	m_pFontShader->RenderShader(pContext, pSecntence->nIndexCount, worldMat, m_matBaseViewMatrix, orthoMat, m_pFont->GetTexture(), color);
}
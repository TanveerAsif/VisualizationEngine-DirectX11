#include "Dx11_Terrain.h"
#include <stdio.h>


Dx11_Terrain::Dx11_Terrain()
	:m_iWidth(300), 
	m_iHeight(200)
{

}


Dx11_Terrain::~Dx11_Terrain()
{
}

bool Dx11_Terrain::Init(ID3D11Device * _pDevice)
{
	m_iVertexCount = 6 * (m_iWidth - 1) * (m_iHeight - 1) ;
	//m_iVertexCount = m_iWidth * m_iHeight;
	m_pVertexList = new stVertex[m_iVertexCount];

	unsigned int index = 0;
	for (int iCol = 0; iCol < m_iHeight - 1; iCol++)
	{
		for (int iRow = 0; iRow < m_iWidth - 1; iRow++)
		{
			/*unsigned int v1 = (iCol * (m_iWidth - 1))  + iRow;
			unsigned int v2 = (iCol * (m_iWidth - 1)) + iRow + 1;
			unsigned int v3 = ((iCol + 1) * m_iWidth) + iRow;

			unsigned int v4 = ((iCol + 1) * m_iWidth) + iRow;
			unsigned int v5 = ((iCol + 1) * m_iWidth) + iRow + 1;
			unsigned int v6 = (iCol * (m_iWidth - 1)) + iRow + 1;*/

			//First
			m_pVertexList[index].vPos = D3DXVECTOR3(iRow , 0.0f, iCol );
			m_pVertexList[index].vTex = D3DXVECTOR2(0.0f, 0.0f);
			m_pVertexList[index].vNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			index++;			

			m_pVertexList[index].vPos = D3DXVECTOR3(iRow + 1, 0.0f, iCol);
			m_pVertexList[index].vTex = D3DXVECTOR2(0.0f, 0.0f);
			m_pVertexList[index].vNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			index++;

			m_pVertexList[index].vPos = D3DXVECTOR3(iRow , 0.0f, iCol + 1);
			m_pVertexList[index].vTex = D3DXVECTOR2(0.0f, 0.0f);
			m_pVertexList[index].vNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			index++;

			//Second
			m_pVertexList[index].vPos = D3DXVECTOR3(iRow, 0.0f, iCol + 1);
			m_pVertexList[index].vTex = D3DXVECTOR2(0.0f, 0.0f);
			m_pVertexList[index].vNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			index++;

			m_pVertexList[index].vPos = D3DXVECTOR3(iRow + 1, 0.0f, iCol + 1);
			m_pVertexList[index].vTex = D3DXVECTOR2(0.0f, 0.0f);
			m_pVertexList[index].vNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			index++;

			m_pVertexList[index].vPos = D3DXVECTOR3(iRow + 1, 0.0f, iCol);
			m_pVertexList[index].vTex = D3DXVECTOR2(0.0f, 0.0f);
			m_pVertexList[index].vNormal = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			index++;
		}
	}
	
	/*
	if (index == m_iVertexCount)
	{
		float centerX = 0, centerZ = 0;
		for (int i = 0; i < m_iVertexCount; i++)
		{
			centerX += m_pVertexList[i].vPos.x;
			centerZ += m_pVertexList[i].vPos.z;
			char msgbuf[1000];
			sprintf_s(msgbuf, "\nVertex(%d) (%f, %f, %f)", i , m_pVertexList[i].vPos.x, m_pVertexList[i].vPos.y, m_pVertexList[i].vPos.z);
			OutputDebugStringA(msgbuf);

			if ((i + 1) % 6 == 0)
			{
				char Quad[10];
				sprintf_s(Quad, "\n");
				OutputDebugStringA(Quad);

			}
		}
		char msgbuf[1000];
		sprintf_s(msgbuf, "\Center (%f, %f, %f)", centerX/ m_iVertexCount, 0, centerZ / m_iVertexCount);
		OutputDebugStringA(msgbuf);
	}*/
	
	return true;
}

void Dx11_Terrain::CopyVertices(void * _pVertexBuffer)
{	
	if(_pVertexBuffer != nullptr)
		memcpy(_pVertexBuffer, m_pVertexList, sizeof(stVertex)*m_iVertexCount);
}

void Dx11_Terrain::Release()
{
	delete m_pVertexList;
}

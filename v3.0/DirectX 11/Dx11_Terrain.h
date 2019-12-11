#pragma once
#include <D3D11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>


class Dx11_Terrain
{
public:
	struct stVertex
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR2 vTex;
		D3DXVECTOR3 vNormal;
	};
	unsigned int m_iVertexCount;

private:
	unsigned int m_iWidth, m_iHeight;
	stVertex	*m_pVertexList;

public:
	Dx11_Terrain();
	~Dx11_Terrain();

	bool			Init(ID3D11Device *_pDevice);
	unsigned int	GetVertexCount() { return m_iVertexCount; }
	void			CopyVertices(void *pVertexBuffer);
};


#pragma once
#include <D3D11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>


class Dx11_Tessellation
{

private:
	struct stVertex
	{
		D3DXVECTOR3 Pos;
		D3DXVECTOR4 Color;		
	};

	struct stWVPBuffer
	{
		D3DXMATRIX worldMat;
		D3DXMATRIX viewMat;
		D3DXMATRIX projMat;
	};

	struct stTessellationBuffer
	{
		float fTessellationAmount;
		float fPadding[3];
	};

	struct stTickBuffer 
	{
		float fTick;
		D3DXVECTOR3 vPadding;
	};

	ID3D11VertexShader *m_pVS = nullptr;
	ID3D11HullShader   *m_pHS = nullptr;
	ID3D11DomainShader *m_pDS = nullptr;
	ID3D11PixelShader  *m_pPS = nullptr;
	ID3D11InputLayout	*m_pInputLayout = nullptr;
	ID3D11Buffer		*m_pWVPBuffer = nullptr, *m_pTessellationBuffer = nullptr;
	ID3D11Buffer		*m_pTickBuffer = nullptr;

	unsigned int         m_uiIndexCount = 0;
	ID3D11Buffer		*m_pVertexBuffer = nullptr, *m_pIndexBuffer = nullptr;
	bool IntializeShader(ID3D11Device * _pDevice);
	bool IntializeGeometry(ID3D11Device * _pDevice);
	bool IntializeTerrain(ID3D11Device * _pDevice);

public:
	Dx11_Tessellation();
	~Dx11_Tessellation();

	bool Init(ID3D11Device *_pDevice, ID3D11DeviceContext *_pDeviceContext);
	void Render(ID3D11DeviceContext *_pDeviceContext, float _fTick ,D3DXMATRIX _worldMat, D3DXMATRIX _viewMat, D3DXMATRIX _projMat);
	void Release();
};



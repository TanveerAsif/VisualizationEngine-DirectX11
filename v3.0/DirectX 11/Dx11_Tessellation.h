#pragma once
#include <D3D11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>

class Dx11_Tessellation
{

private:
	struct MatrixBuffer
	{
		D3DXMATRIX worldMat;
		D3DXMATRIX viewMat;
		D3DXMATRIX projMat;
	};

	struct TessellationBuffer
	{
		float fTessellationAmount;
		float fPadding[3];
	};

	ID3D11VertexShader *m_pVS;
	ID3D11HullShader   *m_pHS;
	ID3D11DomainShader *m_pDS;
	ID3D11PixelShader  *m_pPS;

	bool IntializeShader(ID3D10Device * _pDevice);
public:
	Dx11_Tessellation();
	~Dx11_Tessellation();

	bool Init(ID3D10Device *_pDevice, ID3D11DeviceContext *_pDeviceContext);
	void Render(ID3D11DeviceContext *_pDeviceContext);
	void Release();
};



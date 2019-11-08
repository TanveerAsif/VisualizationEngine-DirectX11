


#include "Dx11_Tessellation.h"



bool Dx11_Tessellation::IntializeShader(ID3D10Device * _pDevice)
{
	ID3D10Blob *pErrorBuffer, *pVSBuffer, *pHSBuffer, *pDSBuffer, *pPSBuffer;
	//Compile Vertex Shader
	HRESULT hr = D3DX11CompileFromFile(L"TessellationShader.hlsl", nullptr, nullptr, "VertexShader", "vs_5.0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr,
		&pVSBuffer, &pErrorBuffer, nullptr);
	if (hr != S_OK)
		return false;

	//Compile Hull Shader
	hr = D3DX11CompileFromFile(L"TessellationShader.hlsl", nullptr, nullptr, "HullShader", "hs_5.0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr,
		&pHSBuffer, &pErrorBuffer, nullptr);
	if (hr != S_OK)
		return false;

	//Compile Domain Shader
	hr = D3DX11CompileFromFile(L"TessellationShader.hlsl", nullptr, nullptr, "DomainShader", "ds_5.0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr,
		&pDSBuffer, &pErrorBuffer, nullptr);
	if (hr != S_OK)
		return false;

	//Compile Pixel Shader
	hr = D3DX11CompileFromFile(L"TessellationShader.hlsl", nullptr, nullptr, "PixelShader", "ps_5.0", D3D10_SHADER_ENABLE_STRICTNESS, 0, nullptr,
		&pPSBuffer, &pErrorBuffer, nullptr);
	if (hr != S_OK)
		return false;

	//hr = _pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), nullptr, &m_pVS);

	return true;
}

Dx11_Tessellation::Dx11_Tessellation()
{
}


Dx11_Tessellation::~Dx11_Tessellation()
{
}

bool Dx11_Tessellation::Init(ID3D10Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
{
	if (IntializeShader(_pDevice))
	{

	}
	return false;
}

void Dx11_Tessellation::Render(ID3D11DeviceContext * _pDeviceContext)
{
}

void Dx11_Tessellation::Release()
{
}

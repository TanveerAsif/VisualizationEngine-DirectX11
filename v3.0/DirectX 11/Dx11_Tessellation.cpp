
#include "Dx11_Tessellation.h"
#include <fstream>


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


	return true;
}

Dx11_Tessellation::Dx11_Tessellation()
{
}


Dx11_Tessellation::~Dx11_Tessellation()
{
}

bool Dx11_Tessellation::Init(ID3D11Device * _pDevice, ID3D11DeviceContext * _pDeviceContext)
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

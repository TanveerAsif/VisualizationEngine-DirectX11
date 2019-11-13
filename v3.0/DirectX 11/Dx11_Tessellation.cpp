
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
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }		
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

void Dx11_Tessellation::Render(ID3D11DeviceContext * _pDeviceContext, unsigned int _uIndexCount, D3DXMATRIX _worldMat, D3DXMATRIX _viewMat, D3DXMATRIX _projMat)
{
	if (_pDeviceContext)
	{
		_pDeviceContext->IASetInputLayout(m_pInputLayout);

		//Set Buffer To Hull Shader
		D3D11_MAPPED_SUBRESOURCE mappedTessRes;
		HRESULT hr = _pDeviceContext->Map(m_pTessellationBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedTessRes);
		if (hr != S_OK)
			return;
		stTessellationBuffer *pTessData = (stTessellationBuffer *)mappedTessRes.pData;
		pTessData->fTessellationAmount = 12.0f;
		for (int i = 0; i < 3; i++)
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
		//////////////////////////////////////////////////////////////////////

		_pDeviceContext->VSSetShader(m_pVS, nullptr, 0);
		_pDeviceContext->HSSetShader(m_pHS, nullptr, 0);
		_pDeviceContext->DSSetShader(m_pDS, nullptr, 0);
		_pDeviceContext->PSSetShader(m_pPS, nullptr, 0);

		_pDeviceContext->DrawIndexed(_uIndexCount, 0, 0);
	}
}

void Dx11_Tessellation::Release()
{
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

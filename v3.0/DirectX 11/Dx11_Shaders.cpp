#include "Dx11_Shaders.h"
#include <fstream>

Dx11_Shaders::Dx11_Shaders()
{	
	m_pVS = NULL;
	m_pPS = NULL;
	m_pInputLayout = NULL;
	m_pMatrixBuffer = NULL;

	m_pTransparency = NULL;

	m_pRefMatrixBuffer = NULL;

	m_pSamplerState = NULL;
	m_pSamplerStateWarp = NULL;
	m_pSamplerStateClamp = NULL;

	m_pRandomLightShaft = NULL;

	m_pLightViewMatrixBuffer = NULL;

	m_pOceanTickBuffer = NULL;
	m_pSunInfoBuffer = NULL;

	m_pFogBuffer = NULL;
}


Dx11_Shaders::~Dx11_Shaders()
{
}



bool Dx11_Shaders::InitializeLightShader(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContext, WCHAR *szShaderFilename)
{
	ID3D10Blob *pError, *pVSBuffer, *pPSBuffer;



	//TextureVertexShader
	HRESULT hr = D3DX11CompileFromFile(szShaderFilename, NULL, NULL, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pVSBuffer, &pError, NULL);
	if (hr != S_OK)
		return false;

	//TexturePixelShader
	hr = D3DX11CompileFromFile(szShaderFilename, NULL, NULL, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pPSBuffer, &pError, NULL);
	if (hr != S_OK)
		return false;


	hr = pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), NULL, &m_pVS);
	if (hr != S_OK)
		return false;

	hr = pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), NULL, &m_pPS);
	if (hr != S_OK)
		return false;


	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	//unsigned int numElements = sizeof(inputLayout) / sizeof(inputLayout[0]);
	UINT numElements = ARRAYSIZE(inputLayout);
	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pInputLayout);
	if (hr != S_OK)
		return false;
	pDeviceContext->IASetInputLayout(m_pInputLayout);


	//m_pMatrixBuffer
	D3D11_BUFFER_DESC matBuffDesc;
	matBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	matBuffDesc.ByteWidth = sizeof(stMatrixBufferType);
	matBuffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matBuffDesc.MiscFlags = 0;
	matBuffDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&matBuffDesc, NULL, &m_pMatrixBuffer);
	if (hr != S_OK)
		return false;
	

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	/*samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1; */
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER; //D3D11_COMPARISON_ALWAYS;//
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	if (hr != S_OK)
		return false;


	////SET TRANSPARENCY FOR TEXTURE
	//D3D11_BUFFER_DESC transDesc;
	//ZeroMemory(&transDesc, sizeof(transDesc));
	//transDesc.Usage = D3D11_USAGE_DYNAMIC;
	//transDesc.ByteWidth = sizeof(stTransparentBufferType);
	//transDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//transDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	//transDesc.MiscFlags = 0;
	//transDesc.StructureByteStride = 0;


	//hr = pDevice->CreateBuffer(&transDesc, NULL, &m_pTransparency);
	//if (hr != S_OK)
	//	return false;


	/*
	//SET REFLECTOIN MATRIX
	D3D11_BUFFER_DESC refDesc;
	ZeroMemory(&refDesc, sizeof(refDesc));
	refDesc.Usage = D3D11_USAGE_DYNAMIC;
	refDesc.ByteWidth = sizeof(stReflectionBufferType);
	refDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	refDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	refDesc.MiscFlags = 0;
	refDesc.StructureByteStride = 0;

	hr = pDevice->CreateBuffer(&refDesc, NULL, &m_pRefMatrixBuffer);
	if (hr != S_OK)
		return false;
		*/

	pVSBuffer->Release();
	pVSBuffer = 0;

	pPSBuffer->Release();
	pPSBuffer = 0;




	return true;
}


bool Dx11_Shaders::InitializeReflectionShader(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContext, WCHAR *szShaderFilename)
{

	ID3D10Blob *pError, *pVSBuffer, *pPSBuffer;

	HRESULT hr = D3DX11CompileFromFile(szShaderFilename, NULL, NULL, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pVSBuffer, &pError, NULL);
	if (hr != S_OK)
	{
		FILE *fp = NULL;
		char *pCompileError = (char *)(pError->GetBufferPointer());
		unsigned long bufferSize = pError->GetBufferSize();

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


	hr = D3DX11CompileFromFile(szShaderFilename, NULL, NULL, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pPSBuffer, &pError, NULL);
	if (hr != S_OK)
		return false;


	hr = pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), NULL, &m_pVS);
	if (hr != S_OK)
		return false;

	hr = pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), NULL, &m_pPS);
	if (hr != S_OK)
		return false;


	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	//// Create the vertex input layout description.
	//// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	//D3D11_INPUT_ELEMENT_DESC inputLayout[2];
	//inputLayout[0].SemanticName = "POSITION";
	//inputLayout[0].SemanticIndex = 0;
	//inputLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//inputLayout[0].InputSlot = 0;
	//inputLayout[0].AlignedByteOffset = 0;
	//inputLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//inputLayout[0].InstanceDataStepRate = 0;

	//inputLayout[1].SemanticName = "TEXCOORD";
	//inputLayout[1].SemanticIndex = 0;
	//inputLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	//inputLayout[1].InputSlot = 0;
	//inputLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	//inputLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	//inputLayout[1].InstanceDataStepRate = 0;

	//unsigned int numElements = sizeof(inputLayout) / sizeof(inputLayout[0]);
	UINT numElements = ARRAYSIZE(inputLayout);
	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pInputLayout);
	if (hr != S_OK)
		return false;

	//SET IN EACH RENDER : SHIFTED IN RENDER FUNCTION
	/*pDeviceContext->IASetInputLayout(m_pInputLayout);*/

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(stMatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	if (hr != S_OK)
		return false;


	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;//D3D11_COMPARISON_NEVER; //
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	if (hr != S_OK)
		return false;




	//SET REFLECTOIN MATRIX
	D3D11_BUFFER_DESC refDesc;
	ZeroMemory(&refDesc, sizeof(refDesc));
	refDesc.Usage = D3D11_USAGE_DYNAMIC;
	refDesc.ByteWidth = sizeof(stReflectionBufferType);
	refDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	refDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	refDesc.MiscFlags = 0;
	refDesc.StructureByteStride = 0;

	hr = pDevice->CreateBuffer(&refDesc, NULL, &m_pRefMatrixBuffer);
	if (hr != S_OK)
		return false;


	pVSBuffer->Release();
	pVSBuffer = 0;

	pPSBuffer->Release();
	pPSBuffer = 0;




	return true;
}




bool Dx11_Shaders::InitializeFloorShader(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContext, WCHAR *szShaderFilename)
{

	ID3D10Blob *pError, *pVSBuffer, *pPSBuffer;
	
	HRESULT hr = D3DX11CompileFromFile(szShaderFilename, NULL, NULL, "TextureVertexShader", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pVSBuffer, &pError, NULL);
	if (hr != S_OK)
	{
		FILE *fp = NULL;
		char *pCompileError = (char *)(pError->GetBufferPointer());
		unsigned long bufferSize = pError->GetBufferSize();

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
		

	hr = D3DX11CompileFromFile(szShaderFilename, NULL, NULL, "TexturePixelShader", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pPSBuffer, &pError, NULL);
	if (hr != S_OK)
		return false;


	hr = pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), NULL, &m_pVS);
	if (hr != S_OK)
		return false;

	hr = pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), NULL, &m_pPS);
	if (hr != S_OK)
		return false;

	
	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },				
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	//unsigned int numElements = sizeof(inputLayout) / sizeof(inputLayout[0]);
	UINT numElements = ARRAYSIZE(inputLayout);
	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pInputLayout);
	if (hr != S_OK)
		return false;
	
	//SET IN EACH RENDER : SHIFTED IN RENDER FUNCTION
	/*pDeviceContext->IASetInputLayout(m_pInputLayout);*/

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(stMatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;	
	hr = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	if (hr != S_OK)
		return false;


	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP; 
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; 
	samplerDesc.MipLODBias = 0.0f; 
	samplerDesc.MaxAnisotropy = 1; 
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;//D3D11_COMPARISON_NEVER; //
	samplerDesc.BorderColor[0] = 0; 
	samplerDesc.BorderColor[1] = 0; 
	samplerDesc.BorderColor[2] = 0; 
	samplerDesc.BorderColor[3] = 0; 
	samplerDesc.MinLOD = 0; 
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	if (hr != S_OK)
		return false;


	
	
	//SET REFLECTOIN MATRIX
	D3D11_BUFFER_DESC refDesc;
	ZeroMemory(&refDesc, sizeof(refDesc));
	refDesc.Usage = D3D11_USAGE_DYNAMIC;
	refDesc.ByteWidth = sizeof(stReflectionBufferType);
	refDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	refDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	refDesc.MiscFlags = 0;
	refDesc.StructureByteStride = 0;

	hr = pDevice->CreateBuffer(&refDesc, NULL, &m_pRefMatrixBuffer);
	if (hr != S_OK)
		return false;
	

	pVSBuffer->Release();
	pVSBuffer = 0;

	pPSBuffer->Release();
	pPSBuffer = 0;




	return true;
}


void Dx11_Shaders::Shutdown()
{


	if (m_pLightBuffer)
	{
		m_pLightBuffer->Release();
		m_pLightBuffer = 0;
	}


	if (m_pFontColorBuffer)
	{
		m_pFontColorBuffer->Release();
		m_pFontColorBuffer = 0;
	}


	if (m_pRefMatrixBuffer)
	{
		m_pRefMatrixBuffer->Release();
		m_pRefMatrixBuffer = 0;
	}


	if (m_pTransparency)
	{
		m_pTransparency->Release();		
		m_pTransparency = 0;
	}
	
	if (m_pSamplerState)
	{
		m_pSamplerState->Release();		
		m_pSamplerState = 0;
	}

	if (m_pSamplerStateWarp)
	{
		m_pSamplerStateWarp->Release();
		m_pSamplerStateWarp = 0;
	}

	if (m_pSamplerStateClamp)
	{
		m_pSamplerStateClamp->Release();
		m_pSamplerStateClamp = 0;
	}


	if (m_pMatrixBuffer)
	{
		m_pMatrixBuffer->Release();
		m_pMatrixBuffer = 0;
	}


	if (m_pInputLayout)
	{
		m_pInputLayout->Release();
		m_pInputLayout = 0;
	}


	if (m_pPS)
	{
		m_pPS->Release();
		m_pPS = 0;
	}


	if (m_pVS)
	{
		m_pVS->Release();
		m_pVS = 0;
	}
		
}


bool Dx11_Shaders::SetCubeShaderParameters(ID3D11DeviceContext* pDeviceContext, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX ProjectionMat, ID3D11ShaderResourceView *pTextureRV)
{
	D3DXMatrixTranspose(&worldMat, &worldMat);
	D3DXMatrixTranspose(&viewMat, &viewMat);
	D3DXMatrixTranspose(&ProjectionMat, &ProjectionMat);



	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return false;

	stMatrixBufferType *dataPtr;
	dataPtr = (stMatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldMat;
	dataPtr->view = viewMat;
	dataPtr->projection = ProjectionMat;
	pDeviceContext->Unmap(m_pMatrixBuffer, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);	

	return true;

}

bool Dx11_Shaders::SetSkyShaderParameters(ID3D11DeviceContext* pDeviceContext, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX ProjectionMat, ID3D11ShaderResourceView *pTextureRV, float fFogStart, float fFogEnd, D3DXVECTOR3 vCamPos)
{
	D3DXMatrixTranspose(&worldMat, &worldMat);
	D3DXMatrixTranspose(&viewMat, &viewMat);
	D3DXMatrixTranspose(&ProjectionMat, &ProjectionMat);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return false;

	stMatrixBufferType *dataPtr;
	dataPtr = (stMatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldMat;
	dataPtr->view = viewMat;
	dataPtr->projection = ProjectionMat;
	dataPtr->vCameraPosition = vCamPos;
	pDeviceContext->Unmap(m_pMatrixBuffer, 0);

	hr = pDeviceContext->Map(m_pFogBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return false;

	stFogParameter *fogPtr;
	fogPtr = (stFogParameter*)mappedResource.pData;
	fogPtr->fFogStart = fFogStart;
	fogPtr->fFogEnd = fFogEnd;
	fogPtr->padding = D3DXVECTOR2(0.0f, 0.0f);
	pDeviceContext->Unmap(m_pFogBuffer, 0);

	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);
	pDeviceContext->VSSetConstantBuffers(1, 1, &m_pFogBuffer);

	pDeviceContext->PSSetShaderResources(0, 1, &pTextureRV);

	return true;
}


bool Dx11_Shaders::SetDepthMapModelShaderParameters(ID3D11DeviceContext* pDeviceContext, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX ProjectionMat, ID3D11ShaderResourceView *pTextureRV)
{

	D3DXMatrixTranspose(&worldMat, &worldMat);
	D3DXMatrixTranspose(&viewMat, &viewMat);
	D3DXMatrixTranspose(&ProjectionMat, &ProjectionMat);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return false;

	stMatrixBufferType *dataPtr;
	dataPtr = (stMatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldMat;
	dataPtr->view = viewMat;
	dataPtr->projection = ProjectionMat;

	pDeviceContext->Unmap(m_pMatrixBuffer, 0);

	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);
	pDeviceContext->PSSetShaderResources(0, 1, &pTextureRV);

	
	return true;

}


bool Dx11_Shaders::SetShaderParameters(ID3D11DeviceContext* pDeviceContext, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX ProjectionMat, ID3D11ShaderResourceView *pTextureRV)
{
	

	D3DXMatrixTranspose(&worldMat, &worldMat);
	D3DXMatrixTranspose(&viewMat, &viewMat);
	D3DXMatrixTranspose(&ProjectionMat, &ProjectionMat);

	
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return false;

	stMatrixBufferType *dataPtr;
	dataPtr = (stMatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldMat;
	dataPtr->view = viewMat;
	dataPtr->projection = ProjectionMat;

	pDeviceContext->Unmap(m_pMatrixBuffer, 0);
	
	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);
	pDeviceContext->PSSetShaderResources(0, 1, &pTextureRV);
	
	

	//FOR TEXTURE TRANSPARENCY
	//Lock The Transparency BUFFER
	//if (m_pTransparency)
	//{

	//	hr = pDeviceContext->Map(m_pTransparency, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	//	if (hr != S_OK)
	//		return false;

	//	stTransparentBufferType *dataPtr2;
	//	dataPtr2 = (stTransparentBufferType *)mappedResource.pData;
	//	dataPtr2->blendAmount = 1;
	//	pDeviceContext->Unmap(m_pTransparency, 0);
	//	//UNLOCK

	//	pDeviceContext->PSSetConstantBuffers(0, 1, &m_pTransparency);
	//}

	if (m_pRandomLightShaft)
	{
		pDeviceContext->VSSetShaderResources(0, 1, &pTextureRV);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = pDeviceContext->Map(m_pRandomLightShaft, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (hr != S_OK)
			return false;

		stRandomValueLightShaft *dataPtr;
		dataPtr = (stRandomValueLightShaft*)mappedResource.pData;
		dataPtr->randVal = (float)(rand() % 10);

		pDeviceContext->Unmap(m_pRandomLightShaft, 0);
		pDeviceContext->VSSetConstantBuffers(1, 1, &m_pRandomLightShaft);
	}

	return true;
}

void Dx11_Shaders::RenderCubeShader(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat,	ID3D11ShaderResourceView *pSRView)
{
	if (pDeviceContext)
	{
		pDeviceContext->IASetInputLayout(m_pInputLayout);
		if (SetCubeShaderParameters(pDeviceContext, worldMat, viewMat, projectionMat, pSRView))
		{
			pDeviceContext->VSSetShader(m_pVS, NULL, 0);
			pDeviceContext->PSSetShader(m_pPS, NULL, 0);			
			pDeviceContext->DrawIndexed(indexCount, 0, 0);
		}

	}
}

void Dx11_Shaders::RenderSkyShader(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat,	ID3D11ShaderResourceView *pSRView, float fFogStart, float fFogEnd, D3DXVECTOR3 vCamPos)
{
	if (pDeviceContext)
	{	
		pDeviceContext->IASetInputLayout(m_pInputLayout);
		if (SetSkyShaderParameters(pDeviceContext, worldMat, viewMat, projectionMat, pSRView, fFogStart,fFogEnd, vCamPos))
		{			
			pDeviceContext->VSSetShader(m_pVS, NULL, 0);
			pDeviceContext->PSSetShader(m_pPS, NULL, 0);
			pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);			
			pDeviceContext->DrawIndexed(indexCount, 0, 0);
		}

	}

}

void Dx11_Shaders::RenderDepthMapModelShader(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat, ID3D11ShaderResourceView *pSRView)
{
	if (pDeviceContext)
	{
		pDeviceContext->IASetInputLayout(m_pInputLayout);
		if (SetDepthMapModelShaderParameters(pDeviceContext, worldMat, viewMat, projectionMat, pSRView))
		{
			pDeviceContext->VSSetShader(m_pVS, NULL, 0);
			pDeviceContext->PSSetShader(m_pPS, NULL, 0);
			pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);
			pDeviceContext->DrawIndexed(indexCount, 0, 0);
		}

	}
}



bool Dx11_Shaders::SetShaderParameters(ID3D11DeviceContext* pDeviceContext
										, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat
										, ID3D11ShaderResourceView *pTextureRV
										, ID3D11ShaderResourceView *pRefTextureRV
										, D3DXMATRIX refViewMatrix)
{


	D3DXMatrixTranspose(&worldMat, &worldMat);
	D3DXMatrixTranspose(&viewMat, &viewMat);
	D3DXMatrixTranspose(&projectionMat, &projectionMat);

	//REFLECTION MATRIX
	D3DXMatrixTranspose(&refViewMatrix, &refViewMatrix);


	D3D11_MAPPED_SUBRESOURCE mappedResource;
	stMatrixBufferType *dataPtr;
	HRESULT hr = pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return false;
	dataPtr = (stMatrixBufferType *)mappedResource.pData;
	dataPtr->world = worldMat;
	dataPtr->view = viewMat;
	dataPtr->projection = projectionMat;
	pDeviceContext->Unmap(m_pMatrixBuffer, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);

	//NOW SET REFLECTION_MATRIX
	stReflectionBufferType *dataPtr2;
	hr = pDeviceContext->Map(m_pRefMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return false;
	dataPtr2 = (stReflectionBufferType *)mappedResource.pData;
	dataPtr2->matReflectionMatrix = refViewMatrix;
	pDeviceContext->Unmap(m_pRefMatrixBuffer, 0);
	pDeviceContext->VSSetConstantBuffers(1, 1, &m_pRefMatrixBuffer);

	pDeviceContext->PSSetShaderResources(0, 1, &pTextureRV);	
	pDeviceContext->PSSetShaderResources(1, 1, &pRefTextureRV);

	return true;

}




void Dx11_Shaders::RenderShader(ID3D11DeviceContext* pDeviceContext, int indexCount
								, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat
								, ID3D11ShaderResourceView *pTextureRV
								, ID3D11ShaderResourceView *pReflectionTextureRV
								, D3DXMATRIX refViewMatrix)
{
	if (pDeviceContext)
	{
		pDeviceContext->IASetInputLayout(m_pInputLayout);		
		if (SetShaderParameters(pDeviceContext, worldMat, viewMat, projectionMat, pTextureRV, pReflectionTextureRV, refViewMatrix))
		{
			pDeviceContext->VSSetShader(m_pVS, NULL, 0);
			pDeviceContext->PSSetShader(m_pPS, NULL, 0);

			pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);

			pDeviceContext->DrawIndexed(indexCount, 0, 0);
		}
	}
}


bool Dx11_Shaders::InitializeFontShader(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContext, WCHAR *ShaderFilename)
{

	ID3D10Blob *pError, *pVSBuffer, *pPSBuffer;

	HRESULT hr = D3DX11CompileFromFile(ShaderFilename, NULL, NULL, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pVSBuffer, &pError, NULL);
	if (hr != S_OK)
	{
		FILE *fp = NULL;
		char *pCompileError = (char *)(pError->GetBufferPointer());
		unsigned long bufferSize = pError->GetBufferSize();

		fopen_s(&fp, "../../../Data/ShaderCompileError.txt", "w");
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


	hr = D3DX11CompileFromFile(ShaderFilename, NULL, NULL, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pPSBuffer, &pError, NULL);
	if (hr != S_OK)
		return false;


	hr = pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), NULL, &m_pVS);
	if (hr != S_OK)
		return false;

	hr = pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), NULL, &m_pPS);
	if (hr != S_OK)
		return false;


	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	UINT numElements = ARRAYSIZE(inputLayout);
	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pInputLayout);
	if (hr != S_OK)
		return false;
	
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(stMatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	if (hr != S_OK)
		return false;


	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;//D3D11_COMPARISON_NEVER; //
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	if (hr != S_OK)
		return false;
	
	D3D11_BUFFER_DESC fontColorDesc;
	fontColorDesc.Usage = D3D11_USAGE_DYNAMIC;
	fontColorDesc.ByteWidth = sizeof(stFontColor);
	fontColorDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	fontColorDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	fontColorDesc.MiscFlags = 0;
	fontColorDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&fontColorDesc, NULL, &m_pFontColorBuffer);
	if (hr != S_OK)
		return false;


	pVSBuffer->Release();
	pVSBuffer = 0;

	pPSBuffer->Release();
	pPSBuffer = 0;




	return true;
}


bool Dx11_Shaders::InitializeDepthBufferShader(ID3D11Device *pDevice, WCHAR *ShaderFileName)
{


	ID3D10Blob *pError, *pVSBuffer, *pPSBuffer;

	HRESULT hr = D3DX11CompileFromFile(ShaderFileName, NULL, NULL, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pVSBuffer, &pError, NULL);
	if (hr != S_OK)
	{
		FILE *fp = NULL;
		char *pCompileError = (char *)(pError->GetBufferPointer());
		unsigned long bufferSize = pError->GetBufferSize();

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


	hr = D3DX11CompileFromFile(ShaderFileName, NULL, NULL, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pPSBuffer, &pError, NULL);
	if (hr != S_OK)
		return false;


	hr = pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), NULL, &m_pVS);
	if (hr != S_OK)
		return false;

	hr = pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), NULL, &m_pPS);
	if (hr != S_OK)
		return false;


	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		
	};

	UINT numElements = ARRAYSIZE(inputLayout);
	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pInputLayout);
	if (hr != S_OK)
		return false;

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(stMatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	if (hr != S_OK)
		return false;

	return true;
}

bool Dx11_Shaders::SetShaderParameters(ID3D11DeviceContext* pDeviceContext, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX ProjectionMat, ID3D11ShaderResourceView *pTextureRV, D3DXVECTOR4 color)
{


	D3DXMatrixTranspose(&worldMat, &worldMat);
	D3DXMatrixTranspose(&viewMat, &viewMat);
	D3DXMatrixTranspose(&ProjectionMat, &ProjectionMat);



	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return false;

	stMatrixBufferType *dataPtr;
	dataPtr = (stMatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldMat;
	dataPtr->view = viewMat;
	dataPtr->projection = ProjectionMat;

	pDeviceContext->Unmap(m_pMatrixBuffer, 0);

	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);
	pDeviceContext->PSSetShaderResources(0, 1, &pTextureRV);



	
	hr = pDeviceContext->Map(m_pFontColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return false;

	stFontColor *fontColorData = (stFontColor *)mappedResource.pData;
	fontColorData->clr = color;
	pDeviceContext->Unmap(m_pFontColorBuffer, 0);
	pDeviceContext->PSSetConstantBuffers(0, 1, &m_pFontColorBuffer);

	return true;
}



void Dx11_Shaders::RenderShader(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat, ID3D11ShaderResourceView *pTextureRV, D3DXVECTOR4 color)
{

	if (pDeviceContext)
	{
		if (SetShaderParameters(pDeviceContext, worldMat, viewMat, projectionMat, pTextureRV, color))
		{			
			pDeviceContext->IASetInputLayout(m_pInputLayout);
			pDeviceContext->VSSetShader(m_pVS, NULL, 0);
			pDeviceContext->PSSetShader(m_pPS, NULL, 0);

			pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);
			pDeviceContext->DrawIndexed(indexCount, 0, 0);
		}
	}
}



bool Dx11_Shaders::SetShaderParameters(ID3D11DeviceContext* pDeviceContext, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX ProjectionMat)
{


	D3DXMatrixTranspose(&worldMat, &worldMat);
	D3DXMatrixTranspose(&viewMat, &viewMat);
	D3DXMatrixTranspose(&ProjectionMat, &ProjectionMat);


	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return false;

	stMatrixBufferType *dataPtr;
	dataPtr = (stMatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldMat;
	dataPtr->view = viewMat;
	dataPtr->projection = ProjectionMat;

	pDeviceContext->Unmap(m_pMatrixBuffer, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);


	if (m_pRandomLightShaft)
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = pDeviceContext->Map(m_pRandomLightShaft, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (hr != S_OK)
			return false;

		stRandomValueLightShaft *dataPtr;
		dataPtr = (stRandomValueLightShaft*)mappedResource.pData;
		dataPtr->randVal = (float)(rand() % 10);

		pDeviceContext->Unmap(m_pRandomLightShaft, 0);
		pDeviceContext->VSSetConstantBuffers(1, 1, &m_pRandomLightShaft);
	}

}


void Dx11_Shaders::RenderShader(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat)
{
	if (pDeviceContext)
	{
		pDeviceContext->IASetInputLayout(m_pInputLayout);
		if (SetShaderParameters(pDeviceContext, worldMat, viewMat, projectionMat))
		{
			pDeviceContext->VSSetShader(m_pVS, NULL, 0);
			pDeviceContext->PSSetShader(m_pPS, NULL, 0);			
			pDeviceContext->DrawIndexed(indexCount, 0, 0);
		}
	}

}

void Dx11_Shaders::RenderFloorShader(ID3D11DeviceContext * pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat, ID3D11ShaderResourceView * pTextureSRV)
{

	if (pDeviceContext)
	{
		
		pDeviceContext->IASetInputLayout(m_pInputLayout);

		D3DXMatrixTranspose(&worldMat, &worldMat);
		D3DXMatrixTranspose(&viewMat, &viewMat);
		D3DXMatrixTranspose(&projectionMat, &projectionMat);

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (hr != S_OK)
			return;

		stMatrixBufferType *dataPtr;
		dataPtr = (stMatrixBufferType*)mappedResource.pData;
		dataPtr->world = worldMat;
		dataPtr->view = viewMat;
		dataPtr->projection = projectionMat;
		pDeviceContext->Unmap(m_pMatrixBuffer, 0);
		pDeviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);

		pDeviceContext->VSSetShader(m_pVS, NULL, 0);
		pDeviceContext->PSSetShader(m_pPS, NULL, 0);
		pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);
		pDeviceContext->PSSetShaderResources(0, 1, &pTextureSRV);
		pDeviceContext->DrawIndexed(indexCount, 0, 0);
		
	}	
}



bool Dx11_Shaders::InitializeSkyShader(ID3D11Device *pDevice, WCHAR *pShaderFile)
{
	ID3D10Blob *pError, *pVSBuffer, *pPSBuffer;

	HRESULT hr = D3DX11CompileFromFile(pShaderFile, NULL, NULL, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pVSBuffer, &pError, NULL);
	if (hr != S_OK)
	{
		FILE *fp = NULL;
		char *pCompileError = (char *)(pError->GetBufferPointer());
		unsigned long bufferSize = pError->GetBufferSize();

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


	hr = D3DX11CompileFromFile(pShaderFile, NULL, NULL, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pPSBuffer, &pError, NULL);
	if (hr != S_OK)
		return false;


	hr = pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), NULL, &m_pVS);
	if (hr != S_OK)
		return false;

	hr = pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), NULL, &m_pPS);
	if (hr != S_OK)
		return false;


	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};

	UINT numElements = ARRAYSIZE(inputLayout);
	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pInputLayout);
	if (hr != S_OK)
		return false;


	//m_pFogBuffer
	D3D11_BUFFER_DESC fogBufferDesc;
	fogBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	fogBufferDesc.ByteWidth = sizeof(stFogParameter);
	fogBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	fogBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	fogBufferDesc.MiscFlags = 0;
	fogBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&fogBufferDesc, NULL, &m_pFogBuffer);
	if (hr != S_OK)
		return false;



	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(stMatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	if (hr != S_OK)
		return false;

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1; 
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER; 
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	if (hr != S_OK)
		return false;


	return true;
}




bool Dx11_Shaders::InitializeLightSourceShader(ID3D11Device *pDevice, WCHAR *pShaderFilename)
{
	ID3D10Blob *pError, *pVSBuffer, *pPSBuffer;



	//TextureVertexShader
	HRESULT hr = D3DX11CompileFromFile(pShaderFilename, NULL, NULL, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pVSBuffer, &pError, NULL);
	if (hr != S_OK)
	{
		FILE *fp = NULL;
		char *pCompileError = (char *)(pError->GetBufferPointer());
		unsigned long bufferSize = pError->GetBufferSize();

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


	//TexturePixelShader
	hr = D3DX11CompileFromFile(pShaderFilename, NULL, NULL, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pPSBuffer, &pError, NULL);
	if (hr != S_OK)
		return false;


	hr = pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), NULL, &m_pVS);
	if (hr != S_OK)
		return false;

	hr = pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), NULL, &m_pPS);
	if (hr != S_OK)
		return false;


	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },		
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	
	UINT numElements = ARRAYSIZE(inputLayout);
	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pInputLayout);
	if (hr != S_OK)
		return false;

	//SAMPLER
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	if (hr != S_OK)
		return false;



	//MATRIX BUFFER
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(stMatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	if (hr != S_OK)
		return false;

	
	//LIGHT BUFFER
	D3D11_BUFFER_DESC lightDesc;
	lightDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightDesc.ByteWidth = sizeof(stLightBuffer);
	lightDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightDesc.MiscFlags = 0;
	lightDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&lightDesc, NULL, &m_pLightBuffer);
	if (hr != S_OK)
		return false;

	//m_pFogBuffer
	D3D11_BUFFER_DESC fogBufferDesc;
	fogBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	fogBufferDesc.ByteWidth = sizeof(stFogParameter);
	fogBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	fogBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	fogBufferDesc.MiscFlags = 0;
	fogBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&fogBufferDesc, NULL, &m_pFogBuffer);
	if (hr != S_OK)
		return false;


	pVSBuffer->Release();
	pVSBuffer = 0;

	pPSBuffer->Release();
	pPSBuffer = 0;


	return true;
}


bool Dx11_Shaders::SetShaderParameters(ID3D11DeviceContext* pDeviceContext, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX ProjectionMat, 
										ID3D11ShaderResourceView *pTextureRV, 
										D3DXVECTOR4 diffuseLight, D3DXVECTOR4 ambientLight,
										D3DXVECTOR3 dir, float fFogStart, float fFogEnd, D3DXVECTOR3 vCamPos)
{


	D3DXMatrixTranspose(&worldMat, &worldMat);
	D3DXMatrixTranspose(&viewMat, &viewMat);
	D3DXMatrixTranspose(&ProjectionMat, &ProjectionMat);



	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return false;
	stMatrixBufferType *dataPtr;
	dataPtr = (stMatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldMat;
	dataPtr->view = viewMat;
	dataPtr->projection = ProjectionMat;
	dataPtr->vCameraPosition = vCamPos;// D3DXVECTOR3(viewMat._41, viewMat._42, viewMat._43);
	pDeviceContext->Unmap(m_pMatrixBuffer, 0);

	hr = pDeviceContext->Map(m_pFogBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return false;
	stFogParameter *pFogPara;
	pFogPara = (stFogParameter*)mappedResource.pData;
	pFogPara->fFogStart = fFogStart;
	pFogPara->fFogEnd = fFogEnd;
	pFogPara->padding = D3DXVECTOR2(0.0f, 0.0f);
	pDeviceContext->Unmap(m_pFogBuffer, 0);

	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);
	pDeviceContext->VSSetConstantBuffers(1, 1, &m_pFogBuffer);

	hr = pDeviceContext->Map(m_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return false;
	
	stLightBuffer *pLightSource = (stLightBuffer *)mappedResource.pData;
	pLightSource->vLightAmbientColor = ambientLight;
	pLightSource->vLightDiffuseColor = diffuseLight;
	pLightSource->vLightDir = dir;
	pLightSource->fPaddingValue = 0.0f;	
	pDeviceContext->Unmap(m_pLightBuffer, 0);

	pDeviceContext->PSSetConstantBuffers(2, 1, &m_pLightBuffer);
	pDeviceContext->PSSetShaderResources(0, 1, &pTextureRV);

	
	return true;
}



void Dx11_Shaders::RenderShader(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat, 
								ID3D11ShaderResourceView *pTextureRV, 
								D3DXVECTOR4 diffuseLight, D3DXVECTOR4 ambientLight, D3DXVECTOR3 dir, float fFogStart, float fFogEnd,D3DXVECTOR3 vCamPos)
{
	if (pDeviceContext)
	{
		if (SetShaderParameters(pDeviceContext, worldMat, viewMat, projectionMat, pTextureRV, diffuseLight, ambientLight, dir, fFogStart, fFogEnd, vCamPos))
		{
			pDeviceContext->IASetInputLayout(m_pInputLayout);
			pDeviceContext->VSSetShader(m_pVS, NULL, 0);
			pDeviceContext->PSSetShader(m_pPS, NULL, 0);
			pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);
			pDeviceContext->DrawIndexed(indexCount, 0, 0);
		}
	}
}



bool Dx11_Shaders::InitializeLightShaftShader(ID3D11Device *pDevice, WCHAR *pShaderFilename)
{

	ID3D10Blob *pError, *pVSBuffer, *pPSBuffer;
	HRESULT hr = D3DX11CompileFromFile(pShaderFilename, NULL, NULL, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pVSBuffer, &pError, NULL);
	if (hr != S_OK)
	{
		FILE *fp = NULL;
		char *pCompileError = (char *)(pError->GetBufferPointer());
		unsigned long bufferSize = pError->GetBufferSize();

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


	hr = D3DX11CompileFromFile(pShaderFilename, NULL, NULL, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pPSBuffer, &pError, NULL);
	if (hr != S_OK)
		return false;


	hr = pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), NULL, &m_pVS);
	if (hr != S_OK)
		return false;

	hr = pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), NULL, &m_pPS);
	if (hr != S_OK)
		return false;


	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};


	UINT numElements = ARRAYSIZE(inputLayout);
	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pInputLayout);
	if (hr != S_OK)
		return false;

	//SAMPLER
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	if (hr != S_OK)
		return false;



	//MATRIX BUFFER
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(stMatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	if (hr != S_OK)
		return false;

	
	D3D11_BUFFER_DESC randDesc;
	randDesc.Usage = D3D11_USAGE_DYNAMIC;
	randDesc.ByteWidth = sizeof(stRandomValueLightShaft);
	randDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	randDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	randDesc.MiscFlags = 0;
	randDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&randDesc, NULL, &m_pRandomLightShaft);
	if (hr != S_OK)
		return false;


	pVSBuffer->Release();
	pVSBuffer = 0;

	pPSBuffer->Release();
	pPSBuffer = 0;


	return true;
}


bool Dx11_Shaders::InitializeParticleSystemShader(ID3D11Device *pDevice, WCHAR *pShaderFilename)
{
	ID3D10Blob *pError, *pVSBuffer, *pPSBuffer;
	HRESULT hr = D3DX11CompileFromFile(pShaderFilename, NULL, NULL, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pVSBuffer, &pError, NULL);
	if (hr != S_OK)
	{
		FILE *fp = NULL;
		char *pCompileError = (char *)(pError->GetBufferPointer());
		unsigned long bufferSize = pError->GetBufferSize();

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


	hr = D3DX11CompileFromFile(pShaderFilename, NULL, NULL, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pPSBuffer, &pError, NULL);
	if (hr != S_OK)
		return false;


	hr = pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), NULL, &m_pVS);
	if (hr != S_OK)
		return false;

	hr = pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), NULL, &m_pPS);
	if (hr != S_OK)
		return false;


	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		
	
	};


	UINT numElements = ARRAYSIZE(inputLayout);
	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pInputLayout);
	if (hr != S_OK)
		return false;

	//SAMPLER
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	if (hr != S_OK)
		return false;



	//MATRIX BUFFER
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(stMatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	if (hr != S_OK)
		return false;


	pVSBuffer->Release();
	pVSBuffer = 0;

	pPSBuffer->Release();
	pPSBuffer = 0;


	return true;
}


bool Dx11_Shaders::SetParticleShaderParameters(ID3D11DeviceContext* pDeviceContext, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX ProjectionMat, ID3D11ShaderResourceView *pTextureRV)
{


	D3DXMatrixTranspose(&worldMat, &worldMat);
	D3DXMatrixTranspose(&viewMat, &viewMat);
	D3DXMatrixTranspose(&ProjectionMat, &ProjectionMat);



	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return false;

	stMatrixBufferType *dataPtr;
	dataPtr = (stMatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldMat;
	dataPtr->view = viewMat;
	dataPtr->projection = ProjectionMat;

	pDeviceContext->Unmap(m_pMatrixBuffer, 0);

	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);
	pDeviceContext->PSSetShaderResources(0, 1, &pTextureRV);



	return true;
}


void Dx11_Shaders::RenderParticleShader(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat,
	ID3D11ShaderResourceView *pSRView)
{
	if (pDeviceContext)
	{
		pDeviceContext->IASetInputLayout(m_pInputLayout);
		if (SetParticleShaderParameters(pDeviceContext, worldMat, viewMat, projectionMat, pSRView))		
		{
			pDeviceContext->VSSetShader(m_pVS, NULL, 0);
			pDeviceContext->PSSetShader(m_pPS, NULL, 0);
			pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);
			pDeviceContext->DrawIndexed(indexCount, 0, 0);
		}

	}

}



bool Dx11_Shaders::InitializeShadowMapShader(ID3D11Device *pDevice, WCHAR *pShaderFilename)
{
	ID3D10Blob *pError, *pVSBuffer, *pPSBuffer;

	HRESULT hr = D3DX11CompileFromFile(pShaderFilename, NULL, NULL, "ShadowMap_VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pVSBuffer, &pError, NULL);
	if (hr != S_OK)
	{
		FILE *fp = NULL;
		char *pCompileError = (char *)(pError->GetBufferPointer());
		unsigned long bufferSize = pError->GetBufferSize();

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


	hr = D3DX11CompileFromFile(pShaderFilename, NULL, NULL, "ShadowMap_PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pPSBuffer, &pError, NULL);
	if (hr != S_OK)
		return false;


	hr = pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), NULL, &m_pVS);
	if (hr != S_OK)
		return false;

	hr = pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), NULL, &m_pPS);
	if (hr != S_OK)
		return false;


	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },

	};

	UINT numElements = ARRAYSIZE(inputLayout);
	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pInputLayout);
	if (hr != S_OK)
		return false;

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(stMatrixBufferShadowMap);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pLightViewMatrixBuffer);
	if (hr != S_OK)
		return false;

	D3D11_BUFFER_DESC lightDesc;
	ZeroMemory(&lightDesc, sizeof(lightDesc));
	lightDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightDesc.ByteWidth = sizeof(stLightBufferColor);
	lightDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightDesc.MiscFlags = 0;
	lightDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&lightDesc, NULL, &m_pLightColorBuffer);
	if (hr != S_OK)
		return false;

	ZeroMemory(&lightDesc, sizeof(lightDesc));
	lightDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightDesc.ByteWidth = sizeof(stLightBufferPos);
	lightDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightDesc.MiscFlags = 0;
	lightDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&lightDesc, NULL, &m_pLightPosBuffer);
	if (hr != S_OK)
		return false;

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerStateWarp);
	if (hr != S_OK)
		return false;

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	hr = pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerStateClamp);
	if (hr != S_OK)
		return false;
	

	return true;
}

void Dx11_Shaders::RenderShadowMap(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat
	, ID3D11ShaderResourceView *pDepthTextureRV, ID3D11ShaderResourceView *pTextureRV, D3DXMATRIX sunViewMat, D3DXMATRIX sunProjMat, D3DXVECTOR4 ambientLight, D3DXVECTOR4 diffuseLight, D3DXVECTOR3 LightPos)
{
	if (pDeviceContext)
	{
		pDeviceContext->IASetInputLayout(m_pInputLayout);
		if (SetShadowMapParameters(pDeviceContext, worldMat, viewMat, projectionMat, pDepthTextureRV, pTextureRV, sunViewMat, sunProjMat, ambientLight, diffuseLight, LightPos))
		{
			pDeviceContext->VSSetShader(m_pVS, NULL, 0);
			pDeviceContext->PSSetShader(m_pPS, NULL, 0);
			pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerStateClamp);
			pDeviceContext->PSSetSamplers(1, 1, &m_pSamplerStateWarp);
			pDeviceContext->DrawIndexed(indexCount, 0, 0);
		}

	}
}


bool Dx11_Shaders::SetShadowMapParameters(ID3D11DeviceContext* pDeviceContext, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX ProjectionMat
	, ID3D11ShaderResourceView *pDepthTextureRV, ID3D11ShaderResourceView *pTextureRV, D3DXMATRIX sunViewMatrix, D3DXMATRIX sunProjectMatrix, D3DXVECTOR4 ambientLight, D3DXVECTOR4 diffuseLight, D3DXVECTOR3 LightPos)
{
	D3DXMatrixTranspose(&worldMat, &worldMat);
	D3DXMatrixTranspose(&viewMat, &viewMat);
	D3DXMatrixTranspose(&ProjectionMat, &ProjectionMat);

	D3DXMatrixTranspose(&sunViewMatrix, &sunViewMatrix);
	D3DXMatrixTranspose(&sunProjectMatrix, &sunProjectMatrix);

	

	//SET WVP IN SHADER
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDeviceContext->Map(m_pLightViewMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return false;
	stMatrixBufferShadowMap *dataPtr;
	dataPtr = (stMatrixBufferShadowMap*)mappedResource.pData;
	dataPtr->world = worldMat;
	dataPtr->view = viewMat;
	dataPtr->projection = ProjectionMat;
	dataPtr->lightView = sunViewMatrix;
	dataPtr->lightProjection = sunProjectMatrix;
	pDeviceContext->Unmap(m_pLightViewMatrixBuffer, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pLightViewMatrixBuffer);

	D3D11_MAPPED_SUBRESOURCE mappedResource4;
	hr = pDeviceContext->Map(m_pLightPosBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource4);
	if (hr != S_OK)
		return false;
	stLightBufferPos *pLightPos = (stLightBufferPos *)mappedResource4.pData;
	pLightPos->vLightPos = LightPos;
	pLightPos->fPaddingValue = 0.0f;
	pDeviceContext->Unmap(m_pLightPosBuffer, 0);
	pDeviceContext->VSSetConstantBuffers(1, 1, &m_pLightPosBuffer);
	
	pDeviceContext->PSSetShaderResources(0, 1, &pTextureRV);
	pDeviceContext->PSSetShaderResources(1, 1, &pDepthTextureRV);	
	D3D11_MAPPED_SUBRESOURCE mappedResource3;
	hr = pDeviceContext->Map(m_pLightColorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource3);
	if (hr != S_OK)
		return false;
	stLightBufferColor *pLightPtr;
	pLightPtr = (stLightBufferColor*)mappedResource3.pData;
	pLightPtr->vLightAmbientColor = ambientLight;
	pLightPtr->vLightDiffuseColor = diffuseLight;
	pDeviceContext->Unmap(m_pLightColorBuffer, 0);
	pDeviceContext->PSSetConstantBuffers(0, 1, &m_pLightColorBuffer);


	


	
	return true;
}


bool Dx11_Shaders::InitializeDepthMapShader(ID3D11Device *pDevice, WCHAR *pShaderFilename)
{
	ID3D10Blob *pError, *pVSBuffer, *pPSBuffer;

	HRESULT hr = D3DX11CompileFromFile(pShaderFilename, NULL, NULL, "DepthMap_VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pVSBuffer, &pError, NULL);
	if (hr != S_OK)
	{
		FILE *fp = NULL;
		char *pCompileError = (char *)(pError->GetBufferPointer());
		unsigned long bufferSize = pError->GetBufferSize();

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


	hr = D3DX11CompileFromFile(pShaderFilename, NULL, NULL, "DepthMap_PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pPSBuffer, &pError, NULL);
	if (hr != S_OK)
		return false;


	hr = pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), NULL, &m_pVS);
	if (hr != S_OK)
		return false;

	hr = pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), NULL, &m_pPS);
	if (hr != S_OK)
		return false;


	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(inputLayout);
	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pInputLayout);
	if (hr != S_OK)
		return false;

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(stMatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	if (hr != S_OK)
		return false;


	return true;
}


bool Dx11_Shaders::InitializeOceanShader(ID3D11Device *pDevice, WCHAR *pShaderFilename)
{
	ID3D10Blob *pError, *pVSBuffer, *pPSBuffer;

	HRESULT hr = D3DX11CompileFromFile(pShaderFilename, NULL, NULL, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pVSBuffer, &pError, NULL);
	if (hr != S_OK)
	{
		FILE *fp = NULL;
		char *pCompileError = (char *)(pError->GetBufferPointer());
		unsigned long bufferSize = pError->GetBufferSize();

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


	hr = D3DX11CompileFromFile(pShaderFilename, NULL, NULL, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pPSBuffer, &pError, NULL);
	if (hr != S_OK)
		return false;


	hr = pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), NULL, &m_pVS);
	if (hr != S_OK)
		return false;

	hr = pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), NULL, &m_pPS);
	if (hr != S_OK)
		return false;


	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};

	UINT numElements = ARRAYSIZE(inputLayout);
	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pInputLayout);
	if (hr != S_OK)
		return false;

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(stMatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	if (hr != S_OK)
		return false;

	//m_pFogBuffer
	D3D11_BUFFER_DESC fogBufferDesc;
	fogBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	fogBufferDesc.ByteWidth = sizeof(stFogParameter);
	fogBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	fogBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	fogBufferDesc.MiscFlags = 0;
	fogBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&fogBufferDesc, NULL, &m_pFogBuffer);
	if (hr != S_OK)
		return false;
	

	//m_pOceanTickBuffer
	D3D11_BUFFER_DESC tickBufferDesc;
	tickBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	tickBufferDesc.ByteWidth = sizeof(stOceanTimeBufferType);
	tickBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	tickBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	tickBufferDesc.MiscFlags = 0;
	tickBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&tickBufferDesc, NULL, &m_pOceanTickBuffer);
	if (hr != S_OK)
		return false;

	//SUN INFO
	D3D11_BUFFER_DESC sunBufferDesc;
	sunBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	sunBufferDesc.ByteWidth = sizeof(stLightBuffer);
	sunBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	sunBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sunBufferDesc.MiscFlags = 0;
	sunBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&sunBufferDesc, NULL, &m_pSunInfoBuffer);
	if (hr != S_OK)
		return false;


	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerStateWarp);
	if (hr != S_OK)
		return false;

	


	return true;
}

bool Dx11_Shaders::InitializeFogShader(ID3D11Device * pDevice, WCHAR * wszShaderFilename)
{
	ID3D10Blob *pError, *pVSBuffer, *pPSBuffer;

	HRESULT hr = D3DX11CompileFromFile(wszShaderFilename, NULL, NULL, "FOG_VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pVSBuffer, &pError, NULL);
	if (hr != S_OK)
	{
		FILE *fp = NULL;
		char *pCompileError = (char *)(pError->GetBufferPointer());
		unsigned long bufferSize = pError->GetBufferSize();

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


	hr = D3DX11CompileFromFile(wszShaderFilename, NULL, NULL, "FOG_PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pPSBuffer, &pError, NULL);
	if (hr != S_OK)
		return false;


	hr = pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), NULL, &m_pVS);
	if (hr != S_OK)
		return false;

	hr = pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), NULL, &m_pPS);
	if (hr != S_OK)
		return false;


	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	UINT numElements = ARRAYSIZE(inputLayout);
	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pInputLayout);
	if (hr != S_OK)
		return false;

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(stMatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	if (hr != S_OK)
		return false;
	

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;//D3D11_COMPARISON_NEVER; //
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	if (hr != S_OK)
		return false;


	pVSBuffer->Release();
	pVSBuffer = 0;

	pPSBuffer->Release();
	pPSBuffer = 0;

	return true;
}

bool Dx11_Shaders::InitializeArrowShader(ID3D11Device * pDevice, WCHAR * ShaderFilename)
{
	ID3D10Blob *pError, *pVSBuffer, *pPSBuffer;

	HRESULT hr = D3DX11CompileFromFile(ShaderFilename, NULL, NULL, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pVSBuffer, &pError, NULL);
	if (hr != S_OK)
	{
		FILE *fp = NULL;
		char *pCompileError = (char *)(pError->GetBufferPointer());
		unsigned long bufferSize = pError->GetBufferSize();

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


	hr = D3DX11CompileFromFile(ShaderFilename, NULL, NULL, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pPSBuffer, &pError, NULL);
	if (hr != S_OK)
		return false;


	hr = pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), NULL, &m_pVS);
	if (hr != S_OK)
		return false;

	hr = pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), NULL, &m_pPS);
	if (hr != S_OK)
		return false;


	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	
	UINT numElements = ARRAYSIZE(inputLayout);
	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pInputLayout);
	if (hr != S_OK)
		return false;

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(stMatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	if (hr != S_OK)
		return false;

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = pDevice->CreateSamplerState(&samplerDesc, &m_pSamplerState);
	if (hr != S_OK)
		return false;

	pVSBuffer->Release();
	pVSBuffer = 0;

	pPSBuffer->Release();
	pPSBuffer = 0;


	return true;	
}


bool Dx11_Shaders::SetOceanShaderParameters(ID3D11DeviceContext* pDeviceContext, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX ProjectionMat, ID3D11ShaderResourceView *pTextureRV, float _time, D3DXVECTOR4 vSunAmbientLight, D3DXVECTOR3 vSunDir, float fFogStart, float fFogEnd, D3DXVECTOR3 vCamPos)
{
	D3DXMatrixTranspose(&worldMat, &worldMat);
	D3DXMatrixTranspose(&viewMat, &viewMat);
	D3DXMatrixTranspose(&ProjectionMat, &ProjectionMat);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return false;

	stMatrixBufferType *dataPtr;
	dataPtr = (stMatrixBufferType *)mappedResource.pData;
	dataPtr->world = worldMat;
	dataPtr->view = viewMat;
	dataPtr->vCameraPosition = vCamPos;//D3DXVECTOR3(viewMat._41, viewMat._42, viewMat._43);
	dataPtr->projection = ProjectionMat;	

	pDeviceContext->Unmap(m_pMatrixBuffer, 0);

	D3D11_MAPPED_SUBRESOURCE mappedResource2;
	hr = pDeviceContext->Map(m_pOceanTickBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource2);
	if (hr != S_OK)
		return false;
	stOceanTimeBufferType *dataPtr2 = (stOceanTimeBufferType *)mappedResource2.pData;
	dataPtr2->fTime = _time;	
	pDeviceContext->Unmap(m_pOceanTickBuffer, 0);

	//m_pSunInfoBuffer 
	D3D11_MAPPED_SUBRESOURCE mappedResourceSun;
	hr = pDeviceContext->Map(m_pSunInfoBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResourceSun);
	if (hr != S_OK)
		return false;
	stLightBuffer *pSunData = (stLightBuffer *)mappedResourceSun.pData;
	pSunData->vLightAmbientColor = vSunAmbientLight;
	pSunData->vLightDir = -1 * vSunDir;
	pSunData->vLightDiffuseColor = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);//USING ONLY AMBIENT LIGHT AND DIR
	pSunData->fPaddingValue = 0.0f;
	pDeviceContext->Unmap(m_pSunInfoBuffer, 0);

	hr = pDeviceContext->Map(m_pFogBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return false;
	stFogParameter *fogPtr;
	fogPtr = (stFogParameter*)mappedResource.pData;
	fogPtr->fFogStart = fFogStart;
	fogPtr->fFogEnd = fFogEnd;
	fogPtr->padding = D3DXVECTOR2(0.0f, 0.0f);
	pDeviceContext->Unmap(m_pFogBuffer, 0);

	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);
	pDeviceContext->VSSetConstantBuffers(1, 1, &m_pOceanTickBuffer);
	pDeviceContext->VSSetConstantBuffers(2, 1, &m_pSunInfoBuffer);
	pDeviceContext->VSSetConstantBuffers(3, 1, &m_pFogBuffer);

	pDeviceContext->PSSetShaderResources(0, 1, &pTextureRV);

	
	
	return true;
}

bool Dx11_Shaders::SetArrowParameters(ID3D11DeviceContext *pDeviceContext, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX ProjectionMat, ID3D11ShaderResourceView * pTextureRV)
{
	D3DXMatrixTranspose(&worldMat, &worldMat);
	D3DXMatrixTranspose(&viewMat, &viewMat);
	D3DXMatrixTranspose(&ProjectionMat, &ProjectionMat);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return false;

	stMatrixBufferType *dataPtr;
	dataPtr = (stMatrixBufferType*)mappedResource.pData;
	dataPtr->world = worldMat;
	dataPtr->view = viewMat;
	dataPtr->projection = ProjectionMat;

	pDeviceContext->Unmap(m_pMatrixBuffer, 0);

	pDeviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);
	pDeviceContext->PSSetShaderResources(0, 1, &pTextureRV);
	
	return true;
}



void Dx11_Shaders::RenderOceanShader(ID3D11DeviceContext *pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat, ID3D11ShaderResourceView *pTextureRV, float _time, D3DXVECTOR4 vLightColor, D3DXVECTOR3 vSunDir, float fFogStart, float fFogEnd, D3DXVECTOR3 vCamPos)
{
	if (pDeviceContext)
	{		
		pDeviceContext->IASetInputLayout(m_pInputLayout);
		if (SetOceanShaderParameters(pDeviceContext, worldMat, viewMat, projectionMat, pTextureRV, _time, vLightColor, vSunDir, fFogStart, fFogEnd, vCamPos))
		{
			pDeviceContext->VSSetShader(m_pVS, NULL, 0);
			pDeviceContext->PSSetShader(m_pPS, NULL, 0);
			pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerStateWarp);
			pDeviceContext->DrawIndexed(indexCount, 0, 0);
		}
	}

}


void Dx11_Shaders::RenderArrowShader(ID3D11DeviceContext * pDeviceContext, int indexCount, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat, ID3D11ShaderResourceView * pTexture)
{
	if (pDeviceContext)
	{
		pDeviceContext->IASetInputLayout(m_pInputLayout);
		if (SetArrowParameters(pDeviceContext, worldMat, viewMat, projectionMat, pTexture))
		{
			pDeviceContext->VSSetShader(m_pVS, NULL, 0);
			pDeviceContext->PSSetShader(m_pPS, NULL, 0);
			pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);
			pDeviceContext->DrawIndexed(indexCount, 0, 0);
		}
	}
}


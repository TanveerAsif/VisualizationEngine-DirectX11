#include "Dx11_Render2Texture.h"


Dx11_Render2Texture::Dx11_Render2Texture()
{
	m_pRenderTargetTexture = NULL;
	m_pRenderTargetView = NULL;
	m_pShaderResView = NULL;

}


Dx11_Render2Texture::~Dx11_Render2Texture()
{

}




bool Dx11_Render2Texture::Initialize(ID3D11Device *pDevice, int textureWidth, int textureHeight, float screenNear, float screenDepth)
{
	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Height = textureHeight;
	texDesc.Width =  textureWidth;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;	
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	HRESULT hr = pDevice->CreateTexture2D(&texDesc, NULL, &m_pRenderTargetTexture);
	if (hr != S_OK)
		return false;
	
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(rtvDesc));
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	hr = pDevice->CreateRenderTargetView(m_pRenderTargetTexture, &rtvDesc, &m_pRenderTargetView);
	if (hr != S_OK)
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;	

	hr = pDevice->CreateShaderResourceView(m_pRenderTargetTexture, &srvDesc, &m_pShaderResView);
	if (hr != S_OK)
		return false;
	
	// Initialize the description of the depth buffer.
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = textureWidth;
	depthBufferDesc.Height = textureHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	hr = pDevice->CreateTexture2D(&depthBufferDesc, NULL, &m_pDepthStencilBuffer);
	if (hr != S_OK)
		return false;
	
	// Initailze the depth stencil view description.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	hr = pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
	if (hr != S_OK)
		return false;

	m_ViewPort.Height = (float)textureHeight;
	m_ViewPort.Width = (float)textureWidth;
	m_ViewPort.MinDepth = 0.0;
	m_ViewPort.MaxDepth = 1.0;
	m_ViewPort.TopLeftX = 0.0;
	m_ViewPort.TopLeftY = 0.0;

	float fieldOfView = (float)D3DX_PI / 4.0f;
	float screenAspect = (float)textureWidth / (float)textureHeight;
	D3DXMatrixPerspectiveFovLH(&m_matProjection, fieldOfView, screenAspect, screenNear, screenDepth);

	D3DXMatrixOrthoLH(&m_matOrtho, (float)textureWidth, (float)textureHeight, screenNear, screenDepth);



	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));
	blendDesc.RenderTarget[0].BlendEnable = false;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	pDevice->CreateBlendState(&blendDesc, &m_pBlendState);

	return true;
}



void Dx11_Render2Texture::Shutdown()
{
	if (m_pDepthStencilBuffer)
	{
		m_pDepthStencilBuffer->Release();
		m_pDepthStencilBuffer = 0;
	}

	if (m_pDepthStencilView)
	{
		m_pDepthStencilView->Release();
		m_pDepthStencilView = 0;
	}

	if (m_pShaderResView)
	{
		m_pShaderResView->Release();
		m_pShaderResView = 0;
	}

	if (m_pRenderTargetView)
	{
		m_pRenderTargetView->Release();
		m_pRenderTargetView = 0;
	}


	if (m_pRenderTargetTexture)
	{
		m_pRenderTargetTexture->Release();
		m_pRenderTargetTexture = 0;
	}	
	
}


void Dx11_Render2Texture::BeginScene(ID3D11DeviceContext *pContext)
{
	if (pContext)
	{
		float clearColor[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
		BeginScene(pContext, clearColor);
	}
}

void Dx11_Render2Texture::BeginScene(ID3D11DeviceContext *pContext, float *pClearColor)
{
	if (pContext)
	{		
		float col[] = { pClearColor[0], pClearColor[1], pClearColor[2], pClearColor[3] };
		pContext->ClearRenderTargetView(m_pRenderTargetView, col);
		//pContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	}
}


void Dx11_Render2Texture::SetRenderTarget(ID3D11DeviceContext *pContext)
{
	if (pContext)
	{
		pContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL/*m_pDepthStencilView*/);
		pContext->RSSetViewports(1, &m_ViewPort);
				
		//pContext->OMSetBlendState(m_pBlendState, 0, 0xffffffff);
	}
}
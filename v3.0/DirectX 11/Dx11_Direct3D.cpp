#include "Dx11_Direct3D.h"


Dx11_Direct3D::Dx11_Direct3D()
{
	m_pSwapChain = NULL;
	m_pDevice = NULL;
	m_pDeviceContext = NULL;

	m_pRenderTargetView = NULL;

	m_pDepthStencilBuffer = NULL;
	m_pDepthStencilState = NULL;
	m_pDepthStencilView = NULL;

	/*m_pSprite = NULL;*/


	m_pBlendState = NULL;
	m_pBlendStateVolume = NULL;
	m_pBlendStateAlphaBlendingOff = NULL;		

	m_pDepthStencilStateOff = NULL;


	m_pRasterWireFrameState = NULL;
	m_pRasterState = NULL;

}


Dx11_Direct3D::~Dx11_Direct3D()
{

}


bool Dx11_Direct3D::Initialize(HWND hWnd, int screenWidth, int screenHeight)
{
	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	
	swapDesc.BufferDesc.Width = screenWidth;
	swapDesc.BufferDesc.Height = screenHeight;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED;
	
	//swapDesc.SampleDesc.Count = 8;	//MULTI SAMPLING OF A PIXEL BY 8 NEARBY PIXEL
	swapDesc.SampleDesc.Count = 1;	
	swapDesc.SampleDesc.Quality = 0;

	swapDesc.BufferCount = 1;	//#BACK BUFFER
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //SET VIEW PORT TO RENDER
	swapDesc.OutputWindow = hWnd;

	swapDesc.Windowed = TRUE;

	swapDesc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_DISCARD;



	D3D_FEATURE_LEVEL featureLevel[] = {		
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_10_0,
	};
	D3D_FEATURE_LEVEL selectedFeatureLevel;


	bool bFlag = false;
	HRESULT hr = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, featureLevel, ARRAYSIZE(featureLevel), D3D11_SDK_VERSION
		, &swapDesc, &m_pSwapChain, &m_pDevice, &selectedFeatureLevel, &m_pDeviceContext);
	if (hr == S_OK)
	{
		ID3D11Texture2D *pBackBuffer;
		hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), (LPVOID *)&pBackBuffer);
		if (hr == S_OK)
		{
			hr == m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
			if (hr == S_OK)
			{				
				D3D11_TEXTURE2D_DESC depthBufferDesc;

				ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
				// Set up the description of the depth buffer.
				depthBufferDesc.Width = screenWidth;
				depthBufferDesc.Height = screenHeight;
				depthBufferDesc.MipLevels = 1;
				depthBufferDesc.ArraySize = 1;
				depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
				//depthBufferDesc.Format = depthBufferDesc.Format;
				depthBufferDesc.SampleDesc.Count = 1;
				depthBufferDesc.SampleDesc.Quality = 0;
				depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
				depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
				depthBufferDesc.CPUAccessFlags = 0;
				depthBufferDesc.MiscFlags = 0;

				hr = m_pDevice->CreateTexture2D(&depthBufferDesc, 0, &m_pDepthStencilBuffer);
				if (hr == S_OK)
				{
					D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
					ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
					
					// Set up the description of the stencil state.
					depthStencilDesc.DepthEnable = true;
					depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
					depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
					depthStencilDesc.StencilEnable = true;
					depthStencilDesc.StencilReadMask = 0xFF;
					depthStencilDesc.StencilWriteMask = 0xFF;
					
					// Stencil operations if pixel is front-facing.
					depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
					depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
					
					// Stencil operations if pixel is back-facing.
					depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
					depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
					depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


					hr = m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState);
					if (hr == S_OK)
					{
						// Set the depth stencil state.
						//m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);

						depthStencilDesc.DepthEnable = false;
						hr = m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilStateOff);
						if (hr != S_OK)
							return false;


						D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
						ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));												
						depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
						depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
						depthStencilViewDesc.Texture2D.MipSlice = 0;

						// Create the depth stencil view.
						hr = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
						if (hr == S_OK)
						{
							// Bind the render target view and depth stencil buffer to the output render pipeline.
							//m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
							m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

							D3D11_RASTERIZER_DESC rasterDesc, rasterDescWireFrame;
							ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
							rasterDesc.AntialiasedLineEnable = true;
							rasterDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
							rasterDesc.DepthBias = 0;
							rasterDesc.DepthBiasClamp = 0.0f;
							rasterDesc.DepthClipEnable = true;
							rasterDesc.FillMode = D3D11_FILL_SOLID;
							rasterDesc.FrontCounterClockwise = true;
							rasterDesc.MultisampleEnable =  true; //false
							rasterDesc.ScissorEnable = false;
							rasterDesc.SlopeScaledDepthBias = 0.0f;
							hr = m_pDevice->CreateRasterizerState(&rasterDesc, &m_pRasterState);

							rasterDescWireFrame.AntialiasedLineEnable = true;
							rasterDescWireFrame.CullMode = D3D11_CULL_NONE;
							rasterDescWireFrame.DepthBias = 0;
							rasterDescWireFrame.DepthBiasClamp = 0.0f;
							rasterDescWireFrame.DepthClipEnable = true;								
							rasterDescWireFrame.FillMode = D3D11_FILL_WIREFRAME;
							rasterDescWireFrame.FrontCounterClockwise = false;
							rasterDescWireFrame.MultisampleEnable = true;
							rasterDescWireFrame.ScissorEnable = false;
							rasterDescWireFrame.SlopeScaledDepthBias = 0.0f;
							hr = m_pDevice->CreateRasterizerState(&rasterDescWireFrame, &m_pRasterWireFrameState);
							if (hr != S_OK)
								MessageBox(NULL, L"Raster WireFrame Fail", L"Error", MB_ICONEXCLAMATION);


							//RasterFront
							D3D11_RASTERIZER_DESC rFront;
							ZeroMemory(&rFront, sizeof(D3D11_RASTERIZER_DESC));
							rFront.CullMode = D3D11_CULL_BACK;
							rFront.FillMode = D3D11_FILL_SOLID;
							rFront.FrontCounterClockwise = false;
							rFront.DepthClipEnable = true;
							hr = m_pDevice->CreateRasterizerState(&rFront, &m_pRasterStateFront);
							if (hr != S_OK)
								MessageBox(NULL, L"RasterFront Fail", L"Error", MB_ICONEXCLAMATION);


							//RasterBack
							D3D11_RASTERIZER_DESC rBack;
							ZeroMemory(&rBack, sizeof(D3D11_RASTERIZER_DESC));
							rBack.CullMode = D3D11_CULL_FRONT;
							rBack.FillMode = D3D11_FILL_SOLID;
							rBack.FrontCounterClockwise = false;
							rBack.DepthClipEnable = true;
							hr = m_pDevice->CreateRasterizerState(&rBack, &m_pRasterStateBack);
							if (hr != S_OK)
								MessageBox(NULL, L"RasterFront Fail", L"Error", MB_ICONEXCLAMATION);

							if (hr == S_OK)
							{
								// Now set the rasterizer state.
								//m_pDeviceContext->RSSetState(m_pRasterStateFront);							 
								m_pDeviceContext->RSSetState(m_pRasterState);



								// Setup the viewport for rendering.
																
								m_viewport.Width = (float)screenWidth;
								m_viewport.Height = (float)screenHeight;
								m_viewport.MinDepth = 0.0f;
								m_viewport.MaxDepth = 1.0f;
								m_viewport.TopLeftX = 0.0f;
								m_viewport.TopLeftY = 0.0f;
								
								// Create the viewport.
								m_pDeviceContext->RSSetViewports(1, &m_viewport);




								D3DXMatrixIdentity(&m_WorldMat);

								
								float fieldOfView = (float)D3DX_PI / 4.0f;
								float screenAspect =  (float)screenWidth / (float)screenHeight;
								D3DXMatrixPerspectiveFovLH(&m_matProjection, fieldOfView, screenAspect, 0.001f, 10000.0f);
								D3DXMatrixOrthoLH(&m_matOrtho, (float)screenWidth, (float)screenHeight, 0.001, 10000.0f);
								
								bFlag = true;

								D3D11_BLEND_DESC blendDesc;
								ZeroMemory(&blendDesc, sizeof(blendDesc));
								blendDesc.RenderTarget[0].BlendEnable = true;
								blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
								blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
								blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
								blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
								blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
								blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
								blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;								
								hr = m_pDevice->CreateBlendState(&blendDesc, &m_pBlendState);
								if (hr == S_OK)
								{
									m_pDeviceContext->OMSetBlendState(m_pBlendState, 0, 0xffffffff);
								}


								D3D11_BLEND_DESC AlphablendDesc;
								ZeroMemory(&AlphablendDesc, sizeof(AlphablendDesc));
								AlphablendDesc.RenderTarget[0].BlendEnable = false;
								AlphablendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
								AlphablendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
								AlphablendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
								AlphablendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
								AlphablendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
								AlphablendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
								AlphablendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
								hr = m_pDevice->CreateBlendState(&AlphablendDesc, &m_pBlendStateAlphaBlendingOff);
								if (hr == S_OK)
								{
									//m_pDeviceContext->OMSetBlendState(m_pBlendStateAlphaBlendingOff, 0, 0xffffffff);
								}
								


								D3D11_BLEND_DESC bld;
								ZeroMemory(&bld, sizeof(bld));
								bld.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
								bld.RenderTarget[0].BlendEnable = TRUE;
								bld.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_COLOR;
								bld.RenderTarget[0].DestBlend = D3D11_BLEND_DEST_COLOR;
								bld.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
								bld.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
								bld.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
								bld.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
								hr = m_pDevice->CreateBlendState(&bld, &m_pBlendStateVolume);
								if (hr == S_OK)
								{
									//m_pDeviceContext->OMSetBlendState(m_pBlendStateVolume, 0, 0xffffffff);
								}	

								D3D11_BLEND_DESC blendOpaque;
								ZeroMemory(&blendOpaque, sizeof(blendOpaque));
								blendOpaque.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
								blendOpaque.RenderTarget[0].BlendEnable = true;
								blendOpaque.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
								blendOpaque.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
								blendOpaque.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
								blendOpaque.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
								blendOpaque.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
								blendOpaque.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
								hr = m_pDevice->CreateBlendState(&blendOpaque, &m_pBlendStateVolumeOpaque);
								if (hr == S_OK)
								{
									//m_pDeviceContext->OMSetBlendState(m_pBlendStateVolume, 0, 0xffffffff);
								}


								D3D11_BLEND_DESC blendBackToFront;
								ZeroMemory(&blendBackToFront, sizeof(blendBackToFront));
								blendBackToFront.RenderTarget[0].BlendEnable = true;
								blendBackToFront.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
								blendBackToFront.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
								blendBackToFront.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
								blendBackToFront.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
								blendBackToFront.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
								blendBackToFront.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
								blendBackToFront.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
								hr = m_pDevice->CreateBlendState(&blendBackToFront, &m_pBlendStateBackToFront);
								if (hr == S_OK)
								{
									//m_pDeviceContext->OMSetBlendState(m_pBlendStateBackToFront, 0, 0xffffffff);
								}
							}
						}						
					}
				}
			}
			pBackBuffer->Release();
			pBackBuffer = 0;
		}
		
	}

	return bFlag;
}




void Dx11_Direct3D::Shutdown()
{

	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (m_pSwapChain)
		m_pSwapChain->SetFullscreenState(false, NULL);

	if (m_pRenderTargetView)
		m_pRenderTargetView->Release();

	if (m_pRasterState)
		m_pRasterState->Release();

	if (m_pRasterWireFrameState)
		m_pRasterWireFrameState->Release();

	if (m_pRasterStateFront)
		m_pRasterStateFront->Release();

	if (m_pRasterStateBack)
		m_pRasterStateBack->Release();

	if (m_pDepthStencilView)
		m_pDepthStencilView->Release();

	if (m_pDepthStencilState)
		m_pDepthStencilState->Release();
	
	if (m_pDepthStencilBuffer)
		m_pDepthStencilBuffer->Release();


	/*if (m_pSprite)
		m_pSprite->Release();*/

	if (m_pSwapChain)
	{
		m_pSwapChain->Release();
		m_pSwapChain = 0;
	}

	if (m_pDevice)
	{
		m_pDevice->Release();
		m_pDevice = 0;
	}

	if (m_pDeviceContext)
	{
		m_pDeviceContext->Release();
		m_pDeviceContext = 0;
	}

}

void Dx11_Direct3D::Process()
{

}



//void Dx11_Direct3D::Render()
bool Dx11_Direct3D::BeginScene()
{
	if (m_pDeviceContext)
	{
		float color[4] = {0.0f, 0.0f, 0.0f, 0.0f };	//BLACK
		//float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };	//WHITE
		//float color[4] = { 0.0f, 0.0f, 0.8f, 1.0f };	//BLUE : SKY
		//float color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };	//FOG
		//float color[4] = { 0.0f, 0.125f, 0.3f, 1.0f };	
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, color);
		//m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		return true;
	}

	return false;
}



void Dx11_Direct3D::EndScene()
{
	if (m_pSwapChain)
		m_pSwapChain->Present(0, 0);
}



void Dx11_Direct3D::EnableBlendState()
{
	if (m_pBlendState)
		m_pDeviceContext->OMSetBlendState(m_pBlendState, 0, 0xffffffff);
}

void Dx11_Direct3D::EnableBlendStateVolume()
{
	if (m_pBlendStateVolume)
		m_pDeviceContext->OMSetBlendState(m_pBlendStateVolume, 0, 0xffffffff);
}

void Dx11_Direct3D::EnableBlendStateVolumeOpaque()
{
	if (m_pBlendStateVolumeOpaque)
		m_pDeviceContext->OMSetBlendState(m_pBlendStateVolumeOpaque, 0, 0xffffffff);
}

void Dx11_Direct3D::EnableBlendStateBackToFront()
{
	if (m_pBlendStateBackToFront)
		m_pDeviceContext->OMSetBlendState(m_pBlendStateBackToFront, 0, 0xffffffff);
}

void Dx11_Direct3D::DisableBlendState()
{
	if (m_pBlendStateAlphaBlendingOff)
		m_pDeviceContext->OMSetBlendState(m_pBlendStateAlphaBlendingOff, 0, 0xffffffff);
}



void Dx11_Direct3D::SetDepthBufferOFF()
{
	if (m_pDepthStencilStateOff)
		m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilStateOff, 1);
	
}


void Dx11_Direct3D::SetDepthBufferON()
{
	if (m_pDepthStencilState)
		m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);
}


//

void Dx11_Direct3D::SetBackBufferRenderTarget()
{
	if (m_pDeviceContext)
	{
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL/*m_pDepthStencilView*/);
		m_pDeviceContext->RSSetViewports(1, &m_viewport);
		
		m_pDeviceContext->OMSetBlendState(m_pBlendState, 0, 0xffffffff);
	}
}


void Dx11_Direct3D::SetRasterWireframe()
{
	if (m_pRasterWireFrameState)
		m_pDeviceContext->RSSetState(m_pRasterWireFrameState);
}

void Dx11_Direct3D::SetRasterStateFrontFace()
{
	if (m_pRasterStateFront)
		m_pDeviceContext->RSSetState(m_pRasterStateFront);
}

void Dx11_Direct3D::SetRasterStateBackFace()
{
	if (m_pRasterStateBack)
		m_pDeviceContext->RSSetState(m_pRasterStateBack);
}

void Dx11_Direct3D::SetRasterSolid()
{
	if (m_pRasterState)
		m_pDeviceContext->RSSetState(m_pRasterState);
}
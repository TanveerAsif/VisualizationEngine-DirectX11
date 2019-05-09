#pragma once

#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>



class Dx11_Direct3D
{

private:

	ID3D11Device		*m_pDevice;
	IDXGISwapChain		*m_pSwapChain;
	ID3D11DeviceContext	*m_pDeviceContext;
	
	//POINTER TO RENDER ON VIEW PORT
	ID3D11RenderTargetView* m_pRenderTargetView;
	
	ID3D11Texture2D			*m_pDepthStencilBuffer;
	ID3D11DepthStencilState *m_pDepthStencilState;
	ID3D11DepthStencilState *m_pDepthStencilStateOff;	//OFF DEPTH BUFFER
	ID3D11DepthStencilView  *m_pDepthStencilView;
	ID3D11RasterizerState   *m_pRasterState, *m_pRasterWireFrameState, *m_pRasterStateFront = NULL, *m_pRasterStateBack = NULL;
	
	D3DXMATRIX m_WorldMat, m_matProjection, m_matOrtho;


	//ID3DX10Sprite				*m_pSprite;
	//TCHAR						m_strFont[LF_FACESIZE];
	//ID3DX10Font					*m_pFont = NULL;


	ID3D11BlendState *m_pBlendState;
	ID3D11BlendState *m_pBlendStateAlphaBlendingOff;
	ID3D11BlendState *m_pBlendStateVolume, *m_pBlendStateVolumeOpaque=NULL;
	ID3D11BlendState *m_pBlendStateBackToFront = NULL;

	D3D11_VIEWPORT    m_viewport;
public:
	Dx11_Direct3D();
	~Dx11_Direct3D();

	bool Initialize(HWND hWnd, int screenWidth, int screenHeight);
	void Shutdown();

	//void Render();
	bool BeginScene();
	void EndScene();

	void Process();

	ID3D11Device* GetDevice(){ return m_pDevice; }
	ID3D11DeviceContext* GetDeviceContext(){ return m_pDeviceContext; }

	D3DXMATRIX GetWorldMatrix(){ return m_WorldMat; }
	D3DXMATRIX GetPerspectiveProjectionMatrix(){ return m_matProjection; }
	D3DXMATRIX GetOrthogonalProjectionMatrix(){ return m_matOrtho; }

	void EnableBlendState();
	void EnableBlendStateVolume();	
	void EnableBlendStateVolumeOpaque();
	void EnableBlendStateBackToFront();
	void DisableBlendState();


	void SetDepthBufferOFF();
	void SetDepthBufferON();

	void SetBackBufferRenderTarget();
	ID3D11DepthStencilView* GetDepthStencilView(){ return m_pDepthStencilView; }

	void SetRasterSolid();
	void SetRasterWireframe();
	void SetRasterStateFrontFace();
	void SetRasterStateBackFace();
};



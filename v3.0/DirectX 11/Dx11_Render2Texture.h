#pragma once

#include <D3D11.h>
#include <D3DX10math.h>


struct stRenderTexture
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex;
};


class Dx11_Render2Texture
{


private:
	ID3D11Texture2D					*m_pRenderTargetTexture;
	ID3D11RenderTargetView			*m_pRenderTargetView;
	ID3D11ShaderResourceView		*m_pShaderResView;

	D3D11_VIEWPORT					m_ViewPort;


	ID3D11Texture2D					*m_pDepthStencilBuffer;
	ID3D11DepthStencilView			*m_pDepthStencilView;

	D3DXMATRIX						m_matProjection, m_matOrtho;

	ID3D11BlendState				*m_pBlendState = NULL;

public:
	Dx11_Render2Texture();
	~Dx11_Render2Texture();

	bool Initialize(ID3D11Device *pDevice, int textureWidth, int textureHeight, float screenNear, float screenDepth);
	void Shutdown();

	void BeginScene(ID3D11DeviceContext *pContext);
	void BeginScene(ID3D11DeviceContext *pContext, float *pClearColor); 
	void SetRenderTarget(ID3D11DeviceContext *pContext);
	ID3D11ShaderResourceView* GetShaderResourceView(){ return m_pShaderResView; }

	D3DXMATRIX GetProjectionMatrix(){ return m_matProjection; };
	D3DXMATRIX GetOrthProjectionMatrix(){ return m_matOrtho; };

};


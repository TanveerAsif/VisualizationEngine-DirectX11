#pragma once
#include "D3D11.h"
#include "D3DX10math.h"
#include "Dx11_Texture.h"
#include "Dx11_Shaders.h"
#include <fstream>


class Dx11_Font
{
	struct stFontType
	{
		float uvLeft, uvRight;
		int nFontWidth;
	};

	struct stVertexType
	{
		D3DXVECTOR3 vPosition;
		D3DXVECTOR2 vTexture;
	};

private:
	Dx11_Texture *m_pTexture;
	stFontType   *m_pFontType;

	bool LoadFontData(char *pFontFile);

public:
	Dx11_Font();
	~Dx11_Font();

	bool Initialize(ID3D11Device *pDevice, char *pFontFile, WCHAR *pTextureFile);
	void BuildVertexBuffer(void *pVertex, char *text, float drawX, float drawY);
	void ShutDown();
	ID3D11ShaderResourceView* GetTexture(){ return m_pTexture->GetTexture(); }

};


class Dx11_Text
{
	struct stVertexType
	{
		D3DXVECTOR3 vPosition;
		D3DXVECTOR2 vTexture;
	};

	struct stSentenceType
	{
		ID3D11Buffer *pVB, *pIB;
		int nVertexCount, nIndexCount;
		//D3DXCOLOR color;
		float fR, fG, fB;
		int nMaxlength;
	};

	Dx11_Font		*m_pFont;
	Dx11_Shaders	*m_pFontShader;
	stSentenceType	*m_pSentence1, *m_pSentence2;
	D3DXMATRIX      m_matBaseViewMatrix;
	int				m_nScreenWidth, m_nScreenHeight;
	
	bool InitializeSectence(ID3D11Device *pDevice, stSentenceType **pSentence, int maxSentenceLength);
	void UpdateSentence(ID3D11DeviceContext *pContext, stSentenceType *pSecntence, char *text, int positionLeft, int positionTop, D3DXVECTOR4 color);
	void RenderSentence(ID3D11DeviceContext *pContext, stSentenceType *pSecntence, D3DXMATRIX worldMat, D3DXMATRIX orthoMat);

public:
	Dx11_Text();
	~Dx11_Text();

	bool Initialize(ID3D11Device *pDevice, ID3D11DeviceContext *pContext,
					char *pFontFile, WCHAR *pTextureFile, 
					D3DXMATRIX  BaseViewMatrix,
					int screenWidth, int screenHeight
					);
	void Shutdown();
	void Render(ID3D11DeviceContext *pContext, char *message,D3DXMATRIX worldMat, D3DXMATRIX orthoMat);
};

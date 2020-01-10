#include "Dx11_Texture.h"


Dx11_Texture::Dx11_Texture()
{
	m_pShaderResourceView = NULL;	
}


Dx11_Texture::~Dx11_Texture()
{
	
}



bool Dx11_Texture::Initiazlize(ID3D11Device *pDevice, WCHAR *pTextureFile)
{	
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(pDevice, pTextureFile, NULL, NULL, &m_pShaderResourceView, NULL);
	if (hr == S_OK)
	{
		m_pwszTexFileName = pTextureFile;
		return true;
	}

	return false;
}


void Dx11_Texture::Shutdown()
{
	if (m_pShaderResourceView)
	{
		m_pShaderResourceView->Release();
		m_pShaderResourceView = 0;
	}

}

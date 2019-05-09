#pragma once
#include <D3D11.h>
#include <D3DX11tex.h>

class Dx11_Texture
{

private:
	const WCHAR							*m_pwszTexFileName = NULL;
	const WCHAR							*m_pwszTexType	  = NULL;
	ID3D11ShaderResourceView			*m_pShaderResourceView = NULL;

public:
	Dx11_Texture();
	~Dx11_Texture();

	bool Initiazlize(ID3D11Device *pDevice, WCHAR *pTextureFile);
	void Shutdown();

	ID3D11ShaderResourceView *GetTexture(){ return m_pShaderResourceView;};
	const WCHAR *GetTextureFileName() { return m_pwszTexFileName; }
	const WCHAR *GetTextureType() { return m_pwszTexType; }
	
	void SetTextureType(const WCHAR *pwszTexType){ m_pwszTexType = pwszTexType;};
	void SetTextureFileName(const WCHAR *pwszTexFileName){ m_pwszTexFileName = pwszTexFileName;};
};


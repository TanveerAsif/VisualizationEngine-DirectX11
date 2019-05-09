#pragma once

#include <d3d11.h>
#include <D3DX10math.h>
#include "Dx11_Texture.h"


struct  stVERTEX
{
	D3DXVECTOR3 pos;
	D3DXCOLOR diffuse;
	D3DXVECTOR2 tex;
};

struct  stVertexDepthMap
{
	D3DXVECTOR3 pos;	
	D3DXVECTOR2 tex;
};

struct  stFloorVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex;

	D3DXVECTOR3 normal;		//ADD NORMAL FOR SUN LIGHT EFFECT 
};


struct  stCubeVertex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR2 tex;

	D3DXVECTOR3 normal;
};


struct  stDepthVertex
{
	D3DXVECTOR3 pos;	
};



struct  stVERTEX_LIGHT_SHAFT_GRID
{
	D3DXVECTOR3 pos;
	D3DXCOLOR diffuse;
	D3DXVECTOR2 tex;
	D3DXVECTOR3 normal;
};

class Dx11_Model
{


private:

	//LIGHT : BILL-BOARD
	ID3D11Buffer	*m_pVertexBuffer, *m_pIndexBuffer;
	int              m_nVertexCount, m_nIndexCount;	


	//FLOOR : FOR REFLECTION
	ID3D11Buffer	*m_pFloorVertexBuffer, *m_pFloorIndexBuffer;
	int              m_nFloorVertexCount, m_nFloorIndexCount;


	//CUBE
	ID3D11Buffer    *m_pCubeVertexBuffer, *m_pCubeIndexBuffer;
	int				m_nCubeVertexCount, m_nCubeIndexCount;
	float           m_fHeading;
	D3DXVECTOR3		m_vPos;
	D3DXVECTOR3		m_vRot;		//ROTATION IN Y STORED AS HEADING(m_fHeading)

	Dx11_Texture    *m_pTexture, *m_pTexture2;
	
private:
	bool CreateVertexBuffer(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContext, D3DXCOLOR VertexColor, float LightRadius);


public:
	Dx11_Model();
	~Dx11_Model();

	bool Initialize(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContext, WCHAR *pTextureFile, WCHAR *pTextureFile2=NULL);
	void Render(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContext, D3DXCOLOR VertexColor, float LightRadius);
	void Shutdown();

	int GetIndexCount(){ return m_nIndexCount; }
	
	ID3D11ShaderResourceView* GetTextureSRView(){ return m_pTexture->GetTexture(); }

	bool LoadFloor(ID3D11Device *pDevice, WCHAR *pTextureFile);
	void RenderFloor(ID3D11DeviceContext *pDeviceContext);
	int GetFloorIndexCount(){ return m_nFloorIndexCount; }	


	bool LoadCube(ID3D11Device *pDevice, WCHAR *pTextureFile);
	void RenderCube(ID3D11DeviceContext *pDeviceContext);
	int GetCubeIndexCount(){ return m_nCubeIndexCount; }

	float GetHeading(){ return m_fHeading;}
	void SetHeading(float fAngle){ m_fHeading = fAngle; }
	void ProcessCube(float _fTick);

	bool LoadFloorDepthTest(ID3D11Device *pDevice);
	void RenderDepth(ID3D11DeviceContext *pDeviceContext);


	bool CreateLightShaftGrid(ID3D11Device *pDevice, unsigned int noPoints, WCHAR *pTextureFile);
	void RenderLightShaftGrid(ID3D11DeviceContext *pDeviceContext);


	bool LoadSun(ID3D11Device *pDevice, WCHAR *pTextureFile, D3DXCOLOR sunColor);
	void RenderSun(ID3D11DeviceContext *pDeviceContext);
	
	D3DXVECTOR3 GetPosition(){ return m_vPos; }
	void SetPosition(D3DXVECTOR3 pos){ m_vPos = pos; }
	
	D3DXVECTOR3 GetRotation(){ return m_vRot; }
	void SetRotation(D3DXVECTOR3 rot){ m_vRot = rot; }


	bool LoadDepthMapModel(ID3D11Device *pDevice);
	void RenderDepthMapModel(ID3D11DeviceContext *pDeviceContext);

	bool LoadArrowModel(ID3D11Device *pDevice);
	void RenderArrowModel(ID3D11DeviceContext *pDeviceContext);


};


#ifndef  _ASSIMP_
#define  _ASSIMP_


#pragma once
#include <d3d11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#include <mmsystem.h>
#include <string>
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;

#include "..\..\..\assimp\Importer.hpp"
#include "..\..\..\assimp\scene.h"
#include "..\..\..\assimp\postprocess.h"


#include "Dx11_Texture.h"




struct stVertex 
{
	FLOAT x, y, z;
	FLOAT tu, tv;
	FLOAT nX, nY, nZ;
};

struct stTexture
{
	std::string szType;
	std::string szPath;
	ID3D11ShaderResourceView *pTexture;
};


class Dx11_Mesh
{
	struct stMatrixBufferType
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;

	};

	ID3D11Buffer	*m_pVertexBuffer = NULL, *m_pIndexBuffer = NULL;

	ID3D11VertexShader *m_pVS;
	ID3D11PixelShader  *m_pPS;

	ID3D11InputLayout	*m_pInputLayout;
	ID3D11Buffer		*m_pMatrixBuffer;
	ID3D11SamplerState	*m_pSamplerState;


	bool InitializeShader(ID3D11Device *pDevice, WCHAR *ShaderFilename);

public :
	Dx11_Mesh(ID3D11Device *pDevice, std::vector<stVertex> vVertices, std::vector<UINT> vIndices, std::vector<Dx11_Texture *> vTextures);
	~Dx11_Mesh();
	
	std::vector<stVertex> m_vVertices;
	std::vector<UINT> m_vIndices;
	std::vector<Dx11_Texture *> m_vTextures;

	ID3D11Device *m_pDevice = NULL;
	ID3D11DeviceContext *m_pDeviceContext = NULL;

	bool setupMesh(ID3D11Device *pDevice);
	void Render(ID3D11DeviceContext *pDeviceContext, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat);
	void ShutDown();
	
};




class Dx11_Assimp
{

private:
	std::vector<Dx11_Mesh *>		m_vMeshes;
	std::vector<Dx11_Texture *>		m_vTextures;
	std::string						m_sDirectory;

	void processNode(aiNode* pNode, const aiScene* pScene);
	Dx11_Mesh* processMesh(aiMesh* pMesh, const aiScene* pScene);
	bool LoadMesh(std::string szFilename);
	std::vector<Dx11_Texture *> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene);
	std::string determineTextureType(const aiScene* pScene, aiMaterial* mat);


public:
	Dx11_Assimp();
	~Dx11_Assimp();

	ID3D11Device				*m_pDevice = NULL;
	ID3D11DeviceContext			*m_pDeviceContext = NULL;
	HWND						m_hWnd;

	bool InitAssimp(HWND hWnd, ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);	
	void Render(ID3D11Device *pDevice, ID3D11DeviceContext *pDeviceContext, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat);
	void ShutDown();

};

#endif // ! _ASSIMP_
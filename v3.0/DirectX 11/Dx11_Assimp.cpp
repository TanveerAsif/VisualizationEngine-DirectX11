#include "Dx11_Assimp.h"



void Dx11_Assimp::processNode(aiNode * pNode, const aiScene * pScene)
{
	aiNode * node = pNode;
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = pScene->mMeshes[node->mMeshes[i]];
		m_vMeshes.push_back(this->processMesh(mesh, pScene));
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], pScene);
	}

}


Dx11_Mesh* Dx11_Assimp::processMesh(aiMesh * pMesh, const aiScene * pScene)
{
	// Data to fill
	std::vector<stVertex> vVertices;
	std::vector<UINT> vIndices;
	std::vector<Dx11_Texture *> vTextures;


	// Walk through each of the mesh's vertices
	for (UINT i = 0; i < pMesh->mNumVertices; i++)
	{
		stVertex vertex;
		vertex.x = pMesh->mVertices[i].x;
		vertex.y = pMesh->mVertices[i].y;
		vertex.z = pMesh->mVertices[i].z;

		if (pMesh->HasTextureCoords(0))
		{
			vertex.tu = pMesh->mTextureCoords[0][i].x;
			vertex.tv = pMesh->mTextureCoords[0][i].y;
		}
				
		if (pMesh->HasNormals())
		{
			vertex.nX = pMesh->mNormals[i].x;
			vertex.nY = pMesh->mNormals[i].y;
			vertex.nZ = pMesh->mNormals[i].z;
		}
		vVertices.push_back(vertex);
	}

	for (UINT i = 0; i < pMesh->mNumFaces; i++)
	{
		aiFace face = pMesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			vIndices.push_back(face.mIndices[j]);
	}

	//MATERIAL 
	if (pMesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = pScene->mMaterials[pMesh->mMaterialIndex];	
		std::vector<Dx11_Texture *> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", pScene);
		vTextures.insert(vTextures.end(), diffuseMaps.begin(), diffuseMaps.end());
	}

	Dx11_Mesh *pMeshInstance = new Dx11_Mesh(m_pDevice, vVertices, vIndices, vTextures);
	return pMeshInstance;
	
}

std::vector<Dx11_Texture *> Dx11_Assimp::loadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName, const aiScene* pScene)
{
	std::string strTextureType;
	if (strTextureType.empty())
		strTextureType = determineTextureType(pScene, material);

	std::vector<Dx11_Texture *> vTextures;
	for (UINT i = 0; i < material->GetTextureCount(type); i++)
	{
		aiString aiStr;
		material->GetTexture(type, i, &aiStr);
		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (UINT j = 0; j < m_vTextures.size(); j++)
		{
			Dx11_Texture	*pTexture = m_vTextures[j];
			const WCHAR			*pwszFileName = pTexture->GetTextureFileName();
			std::wstring	wszFileName(pwszFileName);
			std::string     szFileName(wszFileName.begin(), wszFileName.end());
			if (std::strcmp(szFileName.c_str(), aiStr.C_Str()) == 0)
			{
				vTextures.push_back(pTexture);
				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   
			//If texture hasn't been loaded already, load it
			HRESULT hr;
			Dx11_Texture *pTexture = NULL;
			if (strTextureType == "embedded compressed texture")
			{
				/*int textureindex = getTextureIndex(&aiStr);
				texture.pTexture = getTextureFromModel(pScene, textureindex);*/
			}
			else
			{
				std::string szFileName = std::string(aiStr.C_Str());
				szFileName = m_sDirectory + '/' + szFileName;
				std::wstring wszFileName = std::wstring(szFileName.begin(), szFileName.end());
				/*hr = CreateWICTextureFromFile(m_pDevice, m_pDeviceContext, filenamews.c_str(), nullptr, &texture.pTexture);
				if (FAILED(hr))
					MessageBox(m_hWnd, L"Texture couldn't be loaded", L"Error!", MB_ICONERROR | MB_OK);*/
				pTexture = new Dx11_Texture();				;
				if (pTexture->Initiazlize(m_pDevice, &wszFileName[0]) == false)
				{
					//FREE TEXTURE
					MessageBox(NULL, L"Texture Load Fail", L"Error", MB_ICONERROR | MB_OK);
					if (pTexture)
					{
						pTexture->Shutdown();
						delete	pTexture;
						pTexture = NULL;
					}
				}
				else
				{
					//STORE TEXTURE
					std::wstring wszTexType(typeName.begin(), typeName.end());
					const WCHAR	*pwszTexType = wszTexType.c_str();

					std::string  szTexFile(aiStr.C_Str());
					std::wstring wszTexFileName(szTexFile.begin(), szTexFile.end());
					const WCHAR	*pwszTexFileName = wszTexFileName.c_str();

					pTexture->SetTextureType(pwszTexType);
					pTexture->SetTextureFileName(pwszTexFileName);
					vTextures.push_back(pTexture);
					m_vTextures.push_back(pTexture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
				}				
			}			
		}
	}
	return vTextures;
}

std::string Dx11_Assimp::determineTextureType(const aiScene * pScene, aiMaterial * mat)
{
	aiString strpath;
	unsigned int tindex = 0;
	mat->GetTexture(aiTextureType_DIFFUSE, tindex, &strpath);
	std::string strTexType = strpath.C_Str();
	if (strTexType == "*0" || strTexType == "*1" || strTexType == "*2" || strTexType == "*3" || strTexType == "*4" || strTexType == "*5")
	{
		if (pScene->mTextures[0]->mHeight == 0)
		{
			return "embedded compressed texture";
		}
		else
		{
			return "embedded non-compressed texture";
		}
	}
	if (strTexType.find('.') != std::string::npos)
	{
		return "textures are on disk";
	}
	return "";
}



Dx11_Assimp::Dx11_Assimp()
{
}


Dx11_Assimp::~Dx11_Assimp()
{
	
}

bool Dx11_Assimp::InitAssimp(HWND hWnd, ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContext;
	m_hWnd = hWnd;

	

	//MULTIPLE MESH LOAD
	bool bFlag = false;
	
	////bFlag = LoadMesh("../../../Data/Spider/spider.obj");
	bFlag = LoadMesh("../../../Data/Land/under_land.x");
	////bFlag = LoadMesh("../../../Data/Dragon 2.5/Dragon 2.5.stl");
	

	return bFlag;
}

bool Dx11_Assimp::LoadMesh(std::string szFilename)
{
	Assimp::Importer importor;
	const aiScene* pScene = importor.ReadFile(szFilename, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	if (pScene == NULL)
		return false;
	
	//Store Dir to Load Textures
	m_sDirectory = szFilename.substr(0, szFilename.find_last_of('/'));
	
	processNode(pScene->mRootNode, pScene);

	return true;
}

void Dx11_Assimp::Render(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat)
{
	if (pDevice)
	{
		for (int iMesh = 0; iMesh < m_vMeshes.size(); iMesh++)
		{
			m_vMeshes[iMesh]->Render(pDeviceContext, worldMat, viewMat, projectionMat);
		}
	}
}

void Dx11_Assimp::ShutDown()
{
	for (int iMesh = 0; iMesh < m_vMeshes.size(); iMesh++)
	{
		Dx11_Mesh *pMesh = m_vMeshes[iMesh];
		pMesh->ShutDown();
		delete pMesh;
	}

	for (int iTextureIndex = 0; iTextureIndex < m_vTextures.size(); iTextureIndex++)
	{
		Dx11_Texture *pTexture = m_vTextures[iTextureIndex];
		pTexture->Shutdown();
		delete pTexture;
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////                                       ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////              Dx11_Mesh                ///////////////////////////////////////////////////////////////////
//////////////////////////////////////////                                       ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






bool Dx11_Mesh::InitializeShader(ID3D11Device * pDevice, WCHAR * szShaderFilename)
{
	ID3D10Blob *pError, *pVSBuffer, *pPSBuffer;

	//VertexShader
	HRESULT hr = D3DX11CompileFromFile(szShaderFilename, NULL, NULL, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
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

	//PixelShader
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
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	unsigned int numElements = sizeof(inputLayout) / sizeof(inputLayout[0]);
	//UINT numElements = ARRAYSIZE(inputLayout);
	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pInputLayout);
	if (hr != S_OK)
		return false;
	//Set InputLayout Render
	//pDeviceContext->IASetInputLayout(m_pInputLayout);


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

	pVSBuffer->Release();
	pVSBuffer = 0;

	pPSBuffer->Release();
	pPSBuffer = 0;

	return true;
}

Dx11_Mesh::Dx11_Mesh(ID3D11Device *pDevice, std::vector<stVertex> vVertices, std::vector<UINT> vIndices, std::vector<Dx11_Texture *> vTextures)
{
	m_pDevice = pDevice;
	m_vVertices = vVertices;
	m_vIndices = vIndices;
	m_vTextures = vTextures;

	//Initialize Vertex Buffer and Index Buffer
	setupMesh(pDevice);

	InitializeShader(m_pDevice, L"../../../Data/MeshShader.hlsl");
}

Dx11_Mesh::~Dx11_Mesh()
{
}

bool Dx11_Mesh::setupMesh(ID3D11Device * pDevice)
{
	HRESULT hr;

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(stVertex) * m_vVertices.size();
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &m_vVertices[0];

	hr = pDevice->CreateBuffer(&vbd, &initData, &m_pVertexBuffer);
	if (FAILED(hr))
		return false;

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(UINT) * m_vIndices.size();
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	initData.pSysMem = &m_vIndices[0];

	hr = pDevice->CreateBuffer(&ibd, &initData, &m_pIndexBuffer);
	if (FAILED(hr))
		return false;

	return true;
}

void Dx11_Mesh::Render(ID3D11DeviceContext * pDeviceContext, D3DXMATRIX worldMat, D3DXMATRIX viewMat, D3DXMATRIX projectionMat)
{
	if (pDeviceContext)
	{
		UINT stride = sizeof(stVertex);
		UINT offset = 0;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
		pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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
		if (m_vTextures.size() > 0)
		{
			ID3D11ShaderResourceView* pTexture = m_vTextures[0]->GetTexture();
			pDeviceContext->PSSetShaderResources(0, 1, &pTexture);
		}
		pDeviceContext->VSSetShader(m_pVS, NULL, 0);
		pDeviceContext->PSSetShader(m_pPS, NULL, 0);
		pDeviceContext->DrawIndexed(m_vIndices.size(), 0, 0);
		
		
	}
}

void Dx11_Mesh::ShutDown()
{
	if (m_pVertexBuffer)
		m_pVertexBuffer->Release();

	if (m_pIndexBuffer)
		m_pIndexBuffer->Release();

}

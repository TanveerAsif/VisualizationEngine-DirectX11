#include "Dx11_VolumeRendering.h"
#include <fstream>
#include <algorithm>


#ifdef RAY_CASTING

bool Dx11_VolumeRendering::InitRayCast(ID3D11Device * pDevice)
{
	//Init Bounding Box
	m_nFaceVertexCount = 8;
	/*stVERTEX *pVertices = new stVERTEX[m_nRayVertexCount];
	pVertices[0] = { D3DXVECTOR3(-2.0f, -2.0f, -2.0f),  D3DXVECTOR3(0.0f, 0.0f, 0.0f) };
	pVertices[1] = { D3DXVECTOR3(-2.0f, -2.0f,  2.0f),  D3DXVECTOR3(0.0f, 0.0f, 1.0f) };
	pVertices[2] = { D3DXVECTOR3(-2.0f,  2.0f, -2.0f),  D3DXVECTOR3(0.0f, 1.0f, 0.0f) };
	pVertices[3] = { D3DXVECTOR3(-2.0f,  2.0f,  2.0f),  D3DXVECTOR3(0.0f, 1.0f, 1.0f) };
    pVertices[4] = { D3DXVECTOR3( 2.0f, -2.0f, -2.0f),  D3DXVECTOR3(1.0f, 0.0f, 0.0f) };
	pVertices[5] = { D3DXVECTOR3( 2.0f, -2.0f,  2.0f),  D3DXVECTOR3(1.0f, 0.0f, 1.0f) };
	pVertices[6] = { D3DXVECTOR3( 2.0f,  2.0f, -2.0f),  D3DXVECTOR3(1.0f, 1.0f, 0.0f) };
	pVertices[7] = { D3DXVECTOR3( 2.0f,  2.0f,  2.0f),  D3DXVECTOR3(1.0f, 1.0f, 1.0f) };*/
	
	stVERTEX pVertices[] =
	{
		D3DXVECTOR3(-2.0f,-2.0f,-2.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f),
		D3DXVECTOR3(-2.0f,-2.0f, 2.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f),
		D3DXVECTOR3(-2.0f, 2.0f,-2.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f),
		D3DXVECTOR3(-2.0f, 2.0f, 2.0f), D3DXVECTOR3(0.0f, 1.0f, 1.0f),
		D3DXVECTOR3(2.0f,-2.0f,-2.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f),
		D3DXVECTOR3(2.0f,-2.0f, 2.0f), D3DXVECTOR3(1.0f, 0.0f, 1.0f),
		D3DXVECTOR3(2.0f, 2.0f,-2.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f),
		D3DXVECTOR3(2.0f, 2.0f, 2.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f),
	};


	D3D11_BUFFER_DESC desc, indxDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(stVERTEX)* m_nFaceVertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	HRESULT hr = pDevice->CreateBuffer(&desc, &vertexData, &m_pFaceVB);
	if (hr != S_OK)
		return false;

	// Create index buffer
	m_nFaceIndexCount = 36;
	//unsigned int pIndices[] =
	//{
	//	0, 1, 2,	 2, 1, 3, //L
	//	0, 4, 2,	 2, 4, 6, //F 
	//	4, 5, 6,	 6, 5, 7, //R
	//	5, 1, 7,	 7, 1, 3, //Back
	//	0, 1, 4,	 4, 1, 5, //B
	//	2, 6, 3,	 3, 6, 7  //U
	//};

	// Create index buffer
	UINT pIndices[] =
	{
		0, 1, 3,0, 3, 2,4, 5, 1,
		4, 1, 0,5, 7, 3,5, 3, 1,
		7, 6, 2,7, 2, 3,6, 4, 0,
		6, 0, 2,6, 7, 5,6, 5, 4,
	};
	ZeroMemory(&indxDesc, sizeof(indxDesc));
	indxDesc.Usage = D3D11_USAGE_DEFAULT;
	indxDesc.ByteWidth = sizeof(unsigned int)* m_nFaceIndexCount;
	indxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indxDesc.CPUAccessFlags = 0;
	indxDesc.MiscFlags = 0;
	indxDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	hr = pDevice->CreateBuffer(&indxDesc, &indexData, &m_pFaceIB);
	if (hr != S_OK)
		return false;

	//delete pVertices;
	

	//Init Shader
	ID3D10Blob *pError, *pVSBuffer, *pPSBuffer;

	hr = D3DX11CompileFromFile(L"../../Data/VolumeShaderRayCast.hlsl", NULL, NULL, "FaceVS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pVSBuffer, &pError, NULL);
	if (hr != S_OK)
	{
		FILE *fp = NULL;
		char *pCompileError = (char *)(pError->GetBufferPointer());
		unsigned long bufferSize = pError->GetBufferSize();

		fopen_s(&fp, "../../Data/ShaderCompileError.txt", "w");
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


	hr = D3DX11CompileFromFile(L"../../Data/VolumeShaderRayCast.hlsl", NULL, NULL, "FacePS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pPSBuffer, &pError, NULL);
	if (hr != S_OK)
		return false;


	hr = pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), NULL, &m_pFaceVS);
	if (hr != S_OK)
		return false;

	hr = pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), NULL, &m_pFacePS);
	if (hr != S_OK)
		return false;


	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//unsigned int numElements = sizeof(inputLayout) / sizeof(inputLayout[0]);
	UINT numElements = ARRAYSIZE(inputLayout);
	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pFaceInLayout);
	if (hr != S_OK)
		return false;
		
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(stMatrixBuffer);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	if (hr != S_OK)
		return false;



	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
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


	//Init Ray Shader
	hr = D3DX11CompileFromFile(L"../../Data/VolumeShaderRayCast.hlsl", NULL, NULL, "RayVS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pVSBuffer, &pError, NULL);
	if (hr != S_OK)
	{
		FILE *fp = NULL;
		char *pCompileError = (char *)(pError->GetBufferPointer());
		unsigned long bufferSize = pError->GetBufferSize();

		fopen_s(&fp, "../../Data/ShaderCompileError.txt", "w");
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


	hr = D3DX11CompileFromFile(L"../../Data/VolumeShaderRayCast.hlsl", NULL, NULL, "RayPS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pPSBuffer, &pError, NULL);
	if (hr != S_OK)
		return false;


	hr = pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), NULL, &m_pRayVS);
	if (hr != S_OK)
		return false;

	hr = pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), NULL, &m_pRayPS);
	if (hr != S_OK)
		return false;


	return true;
}

void Dx11_VolumeRendering::RenderRay(ID3D11DeviceContext * _pDeviceContext, D3DXMATRIX _w, D3DXMATRIX _v, D3DXMATRIX _p)
{
	UINT stride = sizeof(stVERTEX);
	UINT offset = 0;
	_pDeviceContext->IASetVertexBuffers(0, 1, &m_pFaceVB, &stride, &offset);
	_pDeviceContext->IASetIndexBuffer(m_pFaceIB, DXGI_FORMAT_R32_UINT, 0);
	_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_pDeviceContext->IASetInputLayout(m_pFaceInLayout);

	D3DXMATRIX matRotationX,matRotationY, matTransform;
	D3DXMatrixRotationY(&matRotationY, ((m_fYaw / 360.0f ) * D3DX_PI));
	D3DXMatrixRotationX(&matRotationX, ((180 / 360.0f) * D3DX_PI));
	matTransform = matRotationX * matRotationY  * _w;

	//D3DXMatrixTranspose(&_w, &_w);
	D3DXMatrixTranspose(&matTransform, &matTransform);
	D3DXMatrixTranspose(&_v, &_v);
	D3DXMatrixTranspose(&_p, &_p);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = _pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (hr != S_OK)
		return;
	stMatrixBuffer *pBuffer = (stMatrixBuffer *)mappedResource.pData;
	pBuffer->mWorld = matTransform;
	pBuffer->mView = _v;
	pBuffer->mProj = _p;
	_pDeviceContext->Unmap(m_pMatrixBuffer, 0);
		
	_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);
	_pDeviceContext->VSSetShader(m_pFaceVS, 0, 0);
	_pDeviceContext->PSSetShader(m_pFacePS, 0, 0);
	_pDeviceContext->DrawIndexed(m_nFaceIndexCount, 0, 0);
}


#endif // RAY_CASTING


Dx11_VolumeRendering::Dx11_VolumeRendering(std::string _strVolumeFile, int _iWidth, int _iHeight, int _iDepth)
{
	m_strVolumeFile = _strVolumeFile;
	m_iWidth = _iWidth;
	m_iHeight = _iHeight;
	m_iDepth = _iDepth;
	m_iStepDepth = _iDepth; //1

#ifdef TEXTURE_3D
	
	float fieldOfView = (float)D3DX_PI / 4.0f;
	float screenAspect = (float)_iWidth / (float)_iHeight;
	D3DXMatrixPerspectiveFovLH(&m_matProspectiveProj, fieldOfView, screenAspect, 0.001f, 10000.0f);

	D3DXMatrixOrthoLH(&m_matOrthoProj, (float)_iWidth, (float)_iHeight, 0.001, 10000.0f);

#endif //  TEXTURE_3D
}


Dx11_VolumeRendering::~Dx11_VolumeRendering()
{
}

bool Dx11_VolumeRendering::Init(ID3D11Device *pDevice)
{
	bool bRetValue = false;	
	
	//READ TEXTURE DATA
	loadRAWFile(m_strVolumeFile);
	bRetValue = Create3DTexture(pDevice, m_pTex3D, m_iWidth, m_iHeight, m_iStepDepth);

#ifdef TEXTURE_3D
	   
	bRetValue = InitShaderResource(pDevice, L"../../Data/VolumeShaderViewAlignedSlicing.hlsl");
	if (!bRetValue)
		return false;

	bRetValue = InitVolume(pDevice);
	if (!bRetValue)
		return false;


	bRetValue = InitPolygonShader(pDevice, L"../../Data/VolumePolygonShader.hlsl");
	if (!bRetValue)
		return false;
	m_pDevice = pDevice;//Use IN Rendering
	
#endif // TEXTURE_3D	

#ifdef RAY_CASTING
	bRetValue = InitRayCast(pDevice);
	if (!bRetValue)
		return false;

#endif // RAY_CASTING


	return bRetValue;
}

void Dx11_VolumeRendering::ShutDown()
{
#ifdef TEXTURE_3D
	if (m_pScalars)
	{
		//FREE m_pScalars
	}

	if (m_pTexture3D)
	{
		m_pTexture3D->Release();
		m_pTexture3D = NULL;
	}

	if (m_pTexture3DSRV)
	{
		m_pTexture3DSRV->Release();
		m_pTexture3DSRV = NULL;
	}	
#endif 


}



void Dx11_VolumeRendering::SetColorScaleValue(float _fScale)
{	
#ifdef TEXTURE_3D
	if (_fScale <= 1.0f && _fScale >= 0.0f)
		m_fColorScaleValue = _fScale;
#endif // TEXTURE_3D
}



void Dx11_VolumeRendering::Render(ID3D11DeviceContext * _pDeviceContext, D3DXMATRIX _w, D3DXMATRIX _v, D3DXMATRIX _p, D3DXVECTOR3 _vCameraPos, D3DXVECTOR3 _vCameraDir)
{
#ifdef TEXTURE_3D

	if (_pDeviceContext)
	{
		float fDistance[8] = {0};
		for (int i = 0; i < m_nVertexCount; i++)
		{
			fDistance[i] = D3DXVec3Length(&(_vCameraPos - m_pVertices[i].pos));

			if (D3DXVec3Length(&(_vCameraPos - m_pVertices[m_iNearest].pos)) >= fDistance[i])
				m_iNearest = i;

			if (D3DXVec3Length(&(_vCameraPos - m_pVertices[m_iFarthest].pos)) <= fDistance[i])
				m_iFarthest = i;
		}

		float fOriginToCam = D3DXVec3Length(&(_vCameraPos - D3DXVECTOR3(0.0f, 0.0f, 0.0f)));
		float fNearestDistance = fOriginToCam -  D3DXVec3Length(&(_vCameraPos - m_pVertices[m_iNearest].pos));
		float fFurthestDistance = D3DXVec3Length(&(_vCameraPos - m_pVertices[m_iFarthest].pos)) - fOriginToCam;
		
		//m_pVertices
		D3DXVECTOR3 pViewVertices[8];
		D3DXMATRIX matMV = _w * _v;
		for (int i = 0; i < m_nVertexCount; i++)
		{
			D3DXVec3TransformCoord(&pViewVertices[i], &m_pVertices[i].pos, &matMV);
		}		
		std::sort(pViewVertices, pViewVertices + m_nVertexCount, [](D3DXVECTOR3 _l, D3DXVECTOR3 _r) {return (_l.z < _r.z); });

		
		D3DXVECTOR3 vN = _vCameraDir;
		//D3DXVec3Normalize(&vN , &vN);
		float maxD = vN.x * vN.x + vN.y * vN.y + vN.z * vN.z;

		for (float dPlane = 0; dPlane < 1.0f; dPlane += 0.0125f/2.0)
		{
			float fD = dPlane; //TO BE CALCULATE
			//fD = (pViewVertices[0].z + pViewVertices[7].z) / 2.0f;
			//Ax + By +Cz + D = 0
			D3DXPLANE Plane(vN.x, vN.y, vN.z, fD);

			std::vector<D3DXVECTOR3> vIntersectionPoints;
			calc_plane_aabb_intersection_points(Plane, pViewVertices, vIntersectionPoints);
			m_iNumIntersectionPoint = vIntersectionPoints.size();
			/*if(m_iNumIntersectionPoint > 0)
				sort_points(&vIntersectionPoints[0], m_iNumIntersectionPoint, Plane);*/

				//Draw Polygon
			RenderPolygon(_pDeviceContext, _w, _v, _p, vIntersectionPoints);

			//Draw Cube
			//RenderCube(_pDeviceContext, _w, _v, _p);				
		}
		
	}
#endif // TEXTURE_3D


#ifdef RAY_CASTING
	RenderRay(_pDeviceContext, _w, _v, _p);

#endif // RAY_CASTING

}


#ifdef TEXTURE_3D
void Dx11_VolumeRendering::calc_plane_aabb_intersection_points(D3DXPLANE plane, D3DXVECTOR3 *_pCubeVertices, std::vector<D3DXVECTOR3 > &_vIntersectionPoints)
{
	D3DXVECTOR3 vPoint;

	////D3DXVECTOR3 vO(0.0f, 0.0f, 0.0f);
	////D3DXVECTOR3 vA(1.0f, 0.0f, 0.0f);
	////D3DXVECTOR3 vB(1.0f, 1.0f, 0.0f);
	////D3DXVECTOR3 vC(0.0f, 1.0f, 0.0f);

	////D3DXVECTOR3 vD(0.0f, 1.0f, 1.0f);
	////D3DXVECTOR3 vE(1.0f, 1.0f, 1.0f);
	////D3DXVECTOR3 vF(1.0f, 0.0f, 1.0f);
	////D3DXVECTOR3 vG(0.0f, 0.0f, 1.0f);

	////
	//////GROUND
	////D3DXVECTOR3 vRayDir = vO - vA;
	////if (IsPlaneEdgeIntersect(vO, vRayDir, _Plane, vPoint))
	////	_vIntersectionPoints.push_back(vPoint);

	////vRayDir = vA - vF; 
	////if (IsPlaneEdgeIntersect(vA, vRayDir, _Plane, vPoint))
	////	_vIntersectionPoints.push_back(vPoint);

	////vRayDir = vF - vG;
	////if (IsPlaneEdgeIntersect(vF, vRayDir, _Plane, vPoint))
	////	_vIntersectionPoints.push_back(vPoint);

	////vRayDir = vG - vO;
	////if (IsPlaneEdgeIntersect(vG, vRayDir, _Plane, vPoint))
	////	_vIntersectionPoints.push_back(vPoint);

	////
	//////UPPER
	////vRayDir = vC - vB;
	////if (IsPlaneEdgeIntersect(vC, vRayDir, _Plane, vPoint))
	////	_vIntersectionPoints.push_back(vPoint);

	////vRayDir = vB - vE;
	////if (IsPlaneEdgeIntersect(vB, vRayDir, _Plane, vPoint))
	////	_vIntersectionPoints.push_back(vPoint);

	////vRayDir = vE - vD;
	////if (IsPlaneEdgeIntersect(vE, vRayDir, _Plane, vPoint))
	////	_vIntersectionPoints.push_back(vPoint);

	////vRayDir = vD - vC;
	////if (IsPlaneEdgeIntersect(vC, vRayDir, _Plane, vPoint))
	////	_vIntersectionPoints.push_back(vPoint);

	////
	//////VERTICAL
	////vRayDir = vO - vC;
	////if (IsPlaneEdgeIntersect(vO, vRayDir, _Plane, vPoint))
	////	_vIntersectionPoints.push_back(vPoint);

	////vRayDir = vA - vB;
	////if (IsPlaneEdgeIntersect(vA, vRayDir, _Plane, vPoint))
	////	_vIntersectionPoints.push_back(vPoint);

	////vRayDir = vF - vE;
	////if (IsPlaneEdgeIntersect(vF, vRayDir, _Plane, vPoint))
	////	_vIntersectionPoints.push_back(vPoint);

	////vRayDir = vG - vD;
	////if (IsPlaneEdgeIntersect(vG, vRayDir, _Plane, vPoint))
	////	_vIntersectionPoints.push_back(vPoint);	

	D3DXVECTOR3 aabb_min(0.0f, 0.0f, 0.0f), aabb_max(1.0f, 1.0f, 1.0f);

	// Test edges along X axis, pointing right.
	D3DXVECTOR3 dir = D3DXVECTOR3(aabb_max.x - aabb_min.x, 0.f, 0.f);
	D3DXVECTOR3 orig = aabb_min;
	if (IsPlaneEdgeIntersect(orig, dir, plane, vPoint))
		_vIntersectionPoints.push_back(vPoint);
	orig = D3DXVECTOR3(aabb_min.x, aabb_max.y, aabb_min.z);
	if (IsPlaneEdgeIntersect(orig, dir, plane, vPoint))
		_vIntersectionPoints.push_back(vPoint);
	orig = D3DXVECTOR3(aabb_min.x, aabb_min.y, aabb_max.z);
	if (IsPlaneEdgeIntersect(orig, dir, plane, vPoint))
		_vIntersectionPoints.push_back(vPoint);
	orig = D3DXVECTOR3(aabb_min.x, aabb_max.y, aabb_max.z);
	if (IsPlaneEdgeIntersect(orig, dir, plane, vPoint))
		_vIntersectionPoints.push_back(vPoint);

	// Test edges along Y axis, pointing up.
	dir = D3DXVECTOR3(0.f, aabb_max.y - aabb_min.y, 0.f);
	orig = D3DXVECTOR3(aabb_min.x, aabb_min.y, aabb_min.z);
	if (IsPlaneEdgeIntersect(orig, dir, plane, vPoint))
		_vIntersectionPoints.push_back(vPoint);
	orig = D3DXVECTOR3(aabb_max.x, aabb_min.y, aabb_min.z);
	if (IsPlaneEdgeIntersect(orig, dir, plane, vPoint))
		_vIntersectionPoints.push_back(vPoint);
	orig = D3DXVECTOR3(aabb_min.x, aabb_min.y, aabb_max.z);
	if (IsPlaneEdgeIntersect(orig, dir, plane, vPoint))
		_vIntersectionPoints.push_back(vPoint);
	orig = D3DXVECTOR3(aabb_max.x, aabb_min.y, aabb_max.z);
	if (IsPlaneEdgeIntersect(orig, dir, plane, vPoint))
		_vIntersectionPoints.push_back(vPoint);

	// Test edges along Z axis, pointing forward.
	dir = D3DXVECTOR3(0.f, 0.f, aabb_max.z - aabb_min.z);
	orig = D3DXVECTOR3(aabb_min.x, aabb_min.y, aabb_min.z);
	if (IsPlaneEdgeIntersect(orig, dir, plane, vPoint))
		_vIntersectionPoints.push_back(vPoint);
	orig = D3DXVECTOR3(aabb_max.x, aabb_min.y, aabb_min.z);
	if (IsPlaneEdgeIntersect(orig, dir, plane, vPoint))
		_vIntersectionPoints.push_back(vPoint);
	orig = D3DXVECTOR3(aabb_min.x, aabb_max.y, aabb_min.z);
	if (IsPlaneEdgeIntersect(orig, dir, plane, vPoint))
		_vIntersectionPoints.push_back(vPoint);
	orig = D3DXVECTOR3(aabb_max.x, aabb_max.y, aabb_min.z);
	if (IsPlaneEdgeIntersect(orig, dir, plane, vPoint))
		_vIntersectionPoints.push_back(vPoint);
}

void Dx11_VolumeRendering::sort_points(D3DXVECTOR3 *points, unsigned point_count, const D3DXPLANE &plane)
{
	if (point_count == 0) return;

	const D3DXVECTOR3 plane_normal = D3DXVECTOR3(plane.a, plane.b, plane.c);
	const D3DXVECTOR3 origin = points[0];

	std::sort(points, points + point_count, [&](const D3DXVECTOR3 &lhs, const D3DXVECTOR3 &rhs) -> bool {
		D3DXVECTOR3 v;
		D3DXVec3Cross(&v, &(lhs - origin), &(rhs - origin));
		return D3DXVec3Dot(&v, &plane_normal) < 0;
	});
}

void Dx11_VolumeRendering::RenderCube(ID3D11DeviceContext * _pDeviceContext, D3DXMATRIX _w, D3DXMATRIX _v, D3DXMATRIX _p)
{
	unsigned int stride = sizeof(stVERTEX);
	unsigned int offset = 0;
	_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	_pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_pDeviceContext->IASetInputLayout(m_pInputLayout);

	/*D3DXMATRIX matScaling, matTranslate, matTransform;
	D3DXMatrixScaling(&matScaling, 10, 10, 10);
	matTransform = matScaling * _w;*/

	D3DXMatrixTranspose(&_w, &_w);
	D3DXMatrixTranspose(&_v, &_v);
	D3DXMatrixTranspose(&_p, &_p);
	//D3DXMatrixTranspose(&matTransform, &matTransform);


	D3D11_MAPPED_SUBRESOURCE mappedResources;
	HRESULT hr = _pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResources);
	if (hr != S_OK)
		return;
	stMatrixBufferType_W_V_P *pMat = (stMatrixBufferType_W_V_P *)mappedResources.pData;
	pMat->w = _w; //matTransform;// _w;
	pMat->v = _v;
	pMat->p = _p;
	_pDeviceContext->Unmap(m_pMatrixBuffer, 0);
	_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);

	_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);
	_pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture3DSRV);

	_pDeviceContext->VSSetShader(m_pVS, NULL, NULL);
	_pDeviceContext->PSSetShader(m_pPS, NULL, NULL);

	_pDeviceContext->DrawIndexed(m_nIndexCount, 0, 0);
}


bool Dx11_VolumeRendering::CompareByZ(D3DXVECTOR3 _p1, D3DXVECTOR3 _p2)
{
	return (_p1.z < _p2.z);
}

bool Dx11_VolumeRendering::IsPlaneEdgeIntersect(D3DXVECTOR3 _vOrigin, D3DXVECTOR3 _vLineDir, D3DXPLANE _Plane, D3DXVECTOR3 &_vIntersectionPoint)
{
	D3DXVECTOR3 vN(_Plane.a, _Plane.b, _Plane.c);
	float denom = D3DXVec3Dot(&_vLineDir, &vN);
	if (abs(denom) > 0.0001f)
	{
		float fLamda = abs((_Plane.d - D3DXVec3Dot(&vN, &_vOrigin)) / denom);
		if (fLamda >= 0 && fLamda <= 1.0f)
		{
			_vIntersectionPoint = _vOrigin + fLamda * _vLineDir;
			return true;
		}
	}
	return false;

	/*if (denom == 0.0f)
		return false;

	float fLamda = abs(-(_Plane.a * _vOrigin.x + _Plane.b * _vOrigin.y + _Plane.c * _vOrigin.z + _Plane.d) / denom);
	if (fLamda >= 0 && fLamda <= 1.0f)
	{
		_vIntersectionPoint = _vOrigin + fLamda * _vLineDir;
		return true;
	}
	return false;*/
}

bool Dx11_VolumeRendering::InitPolygonShader(ID3D11Device * pDevice, WCHAR * szShaderFilename)
{
	ID3D10Blob *pError, *pVSBuffer, *pPSBuffer;

	HRESULT hr = D3DX11CompileFromFile(szShaderFilename, NULL, NULL, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pVSBuffer, &pError, NULL);
	if (hr != S_OK)
	{
		FILE *fp = NULL;
		char *pCompileError = (char *)(pError->GetBufferPointer());
		unsigned long bufferSize = pError->GetBufferSize();

		fopen_s(&fp, "../../Data/ShaderCompileError.txt", "w");
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


	hr = D3DX11CompileFromFile(szShaderFilename, NULL, NULL, "PS", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pPSBuffer, &pError, NULL);
	if (hr != S_OK)
		return false;


	hr = pDevice->CreateVertexShader(pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), NULL, &m_pPolyVS);
	if (hr != S_OK)
		return false;

	hr = pDevice->CreatePixelShader(pPSBuffer->GetBufferPointer(), pPSBuffer->GetBufferSize(), NULL, &m_pPolyPS);
	if (hr != S_OK)
		return false;


	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//unsigned int numElements = sizeof(inputLayout) / sizeof(inputLayout[0]);
	UINT numElements = ARRAYSIZE(inputLayout);
	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pPolyInLayout);
	if (hr != S_OK)
		return false;

	pVSBuffer->Release();
	pVSBuffer = 0;

	pPSBuffer->Release();
	pPSBuffer = 0;

	return true;
}

void Dx11_VolumeRendering::RenderPolygon(ID3D11DeviceContext * _pDeviceContext, D3DXMATRIX _w, D3DXMATRIX _v, D3DXMATRIX _p, std::vector<D3DXVECTOR3> _vPolygonPoints)
{
	////Convert into Triangle Fan
	//std::vector<D3DXVECTOR3> _vTempPoints;
	//for (int i = 0; i < _vPolygonPoints.size(); i++)
	//{
	//	if (i % 2 == 0)
	//		_vTempPoints.push_back(_vPolygonPoints[i / 2]);
	//	else
	//		_vTempPoints.push_back(_vPolygonPoints[_vPolygonPoints.size() - 1 - (i / 2)]);
	//}



	int nVertexCount = _vPolygonPoints.size();
	stPOLYGON *pVertices = new stPOLYGON[nVertexCount];
	//std::copy(_vPolygonPoints.begin(), _vPolygonPoints.end(), pVertices->pos);
	for (int i = 0; i < nVertexCount; i++)
		pVertices[i].pos = _vPolygonPoints[i];

	D3D11_BUFFER_DESC desc, indxDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(stPOLYGON)* nVertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	HRESULT hr = m_pDevice->CreateBuffer(&desc, &vertexData, &m_pPolyVB);
	if (hr != S_OK)
		return;

	int nIndexCount = 6;
	unsigned int pIndices[] =
	{
		0, 1, 2,
		2, 1, 3,
	};
	ZeroMemory(&indxDesc, sizeof(indxDesc));
	indxDesc.Usage = D3D11_USAGE_DEFAULT;
	indxDesc.ByteWidth = sizeof(unsigned int) * nIndexCount;
	indxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indxDesc.CPUAccessFlags = 0;
	indxDesc.MiscFlags = 0;
	indxDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	hr = m_pDevice->CreateBuffer(&indxDesc, &indexData, &m_pPolyIB);
	if (hr != S_OK)
		return;


	unsigned int iStride = sizeof(stPOLYGON);
	unsigned int iOffset = 0;
	_pDeviceContext->IASetVertexBuffers(0, 1, &m_pPolyVB, &iStride, &iOffset);
	_pDeviceContext->IASetIndexBuffer(m_pPolyIB, DXGI_FORMAT_R32_UINT, 0);
	_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST/*D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP*/);
	_pDeviceContext->IASetInputLayout(m_pPolyInLayout);

	D3DXMATRIX matScaling, matTransform;
	D3DXMatrixScaling(&matScaling, 10.0f, 10.0f, 10.0f);
	matTransform = matScaling * _w;

	//D3DXMatrixTranspose(&matTransform, &matTransform);
	D3DXMatrixTranspose(&_w, &_w);
	D3DXMatrixTranspose(&_v, &_v);
	D3DXMatrixTranspose(&_p, &_p);
	D3D11_MAPPED_SUBRESOURCE dMapped;
	hr = _pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &dMapped);
	if (hr != S_OK)
		return;
	stMatrixBufferType_W_V_P *pMatrix = (stMatrixBufferType_W_V_P *)dMapped.pData;
	pMatrix->w = matTransform;//_w; // 
	pMatrix->v = _v;
	pMatrix->p = _p;
	_pDeviceContext->Unmap(m_pMatrixBuffer, 0);

	_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);

	_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);
	_pDeviceContext->PSSetShaderResources(1, 1, &m_pTexture3DSRV);

	_pDeviceContext->VSSetShader(m_pPolyVS, 0, 0);
	_pDeviceContext->PSSetShader(m_pPolyPS, 0, 0);
	_pDeviceContext->DrawIndexed(nIndexCount, 0, 0);

	delete pVertices;
	//delete pIndices;

	if (m_pPolyVB)
		m_pPolyVB->Release();

	if (m_pPolyIB)
		m_pPolyIB->Release();
}




bool Dx11_VolumeRendering::InitShaderResource(ID3D11Device *pDevice, WCHAR *szShaderFilename)
{
	ID3D10Blob *pError, *pVSBuffer, *pPSBuffer;

	HRESULT hr = D3DX11CompileFromFile(szShaderFilename, NULL, NULL, "VS", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL
		, &pVSBuffer, &pError, NULL);
	if (hr != S_OK)
	{
		FILE *fp = NULL;
		char *pCompileError = (char *)(pError->GetBufferPointer());
		unsigned long bufferSize = pError->GetBufferSize();

		fopen_s(&fp, "../../Data/ShaderCompileError.txt", "w");
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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	//unsigned int numElements = sizeof(inputLayout) / sizeof(inputLayout[0]);
	UINT numElements = ARRAYSIZE(inputLayout);
	hr = pDevice->CreateInputLayout(inputLayout, numElements, pVSBuffer->GetBufferPointer(), pVSBuffer->GetBufferSize(), &m_pInputLayout);
	if (hr != S_OK)
		return false;

	//SET IN EACH RENDER : SHIFTED IN RENDER FUNCTION
	/*pDeviceContext->IASetInputLayout(m_pInputLayout);*/

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(stMatrixBufferType_W_V_P);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&matrixBufferDesc, NULL, &m_pMatrixBuffer);
	if (hr != S_OK)
		return false;


	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
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

	D3D11_BUFFER_DESC camDesc;
	ZeroMemory(&camDesc, sizeof(D3D11_BUFFER_DESC));
	camDesc.Usage = D3D11_USAGE_DEFAULT;
	camDesc.ByteWidth = sizeof(stCameraBuffer);
	camDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	camDesc.MiscFlags = 0;
	camDesc.StructureByteStride = 0;
	hr = pDevice->CreateBuffer(&camDesc, NULL, &m_pCameraBuffer);
	if (hr != S_OK)
		return false;

	return true;
}

bool Dx11_VolumeRendering::InitVolume(ID3D11Device *pDevice)
{
	m_nVertexCount = 8;
	//stVERTEX *pVertices = new stVERTEX[m_nVertexCount];
	//unsigned int *pIndices = new unsigned int[];

	m_pVertices = new stVERTEX[m_nVertexCount];
	m_pVertices[0] = { D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f) }; //O 
	m_pVertices[1] = { D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 0.0f) }; //A
	m_pVertices[2] = { D3DXVECTOR3(1.0f, 1.0f, 0.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f) }; //B 
	m_pVertices[3] = { D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f) }; //C
	m_pVertices[4] = { D3DXVECTOR3(0.0f, 1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f) }; //D
	m_pVertices[5] = { D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 1.0f) }; //E
	m_pVertices[6] = { D3DXVECTOR3(1.0f, 0.0f, 1.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f) }; //F 
	m_pVertices[7] = { D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 1.0f) }; //G

	D3D11_BUFFER_DESC desc, indxDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	ZeroMemory(&desc, sizeof(desc));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(stVERTEX)* m_nVertexCount;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = m_pVertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	HRESULT hr = pDevice->CreateBuffer(&desc, &vertexData, &m_pVertexBuffer);
	if (hr != S_OK)
		return false;

	// Create index buffer
	m_nIndexCount = 36;
	unsigned int pIndices[] =
	{
		0, 1, 2, 2, 3, 0,	//F 
		7, 0, 3, 3, 4, 7,   //L
		1, 6, 5, 5, 2, 1,   //R
		6, 7, 4, 4, 5, 6,   //Back
		3, 2, 5, 5, 4, 3,	//U
		1, 0, 7, 7, 6, 1,	//B
	};
	ZeroMemory(&indxDesc, sizeof(indxDesc));
	indxDesc.Usage = D3D11_USAGE_DEFAULT;
	indxDesc.ByteWidth = sizeof(unsigned int)* m_nIndexCount;
	indxDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indxDesc.CPUAccessFlags = 0;
	indxDesc.MiscFlags = 0;
	indxDesc.StructureByteStride = 0;

	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = pIndices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	hr = pDevice->CreateBuffer(&indxDesc, &indexData, &m_pIndexBuffer);
	if (hr != S_OK)
		return false;

	return true;
}


#endif

void Dx11_VolumeRendering::GetNearestAndFarthestVerticesIndex(int & _iNearer, int & _iFarthest)
{
#ifdef TEXTURE_3D
	_iNearer = m_iNearest;
	_iFarthest = m_iFarthest;
#endif // TEXTURE_3D
}

void Dx11_VolumeRendering::GetIntersectionPoint(int &_iIntersectionPoint)
{
#ifdef TEXTURE_3D
	_iIntersectionPoint = m_iNumIntersectionPoint;
#endif // TEXTURE_3D
}

void Dx11_VolumeRendering::UpdateHeading()
{	
#ifdef RAY_CASTING
	m_fYaw += 1; // in Degree
#endif
		
}



void Dx11_VolumeRendering::loadRAWFile(std::string file)
{
	std::ifstream fin(file, std::ios::binary);
	char *memblock = 0;
	int length = 0;
	if (fin.is_open())
	{
		length = static_cast<int>(fin.tellg());
		if (length > m_iWidth * m_iHeight * m_iDepth)
		{
			loadRAWFile16(fin);
		}
		else
		{
			loadRAWFile8(fin);
		}
		fin.close();
	}

}

void Dx11_VolumeRendering::loadRAWFile16(std::ifstream &file)
{
	int length = m_iWidth * m_iHeight * m_iDepth;
	unsigned short *pBuffer = new unsigned short[length];

	file.read((char *)&pBuffer, sizeof(char) * length);

	//scale the scalar values to [0, 1]
	m_pScalars = new float[length];
	for (int i = 0; i < length; i++)
	{
		m_pScalars[i] = (float)(pBuffer[i] / USHRT_MAX);
	}
	int k = 1;
	////////////////////////////////////////////
	///Transfer m_pScalars Data TO TEXTURE3D
	////////////////////////////////////////////

	//mVolume.SetData(mScalars);	
	delete pBuffer;
}


/// <summary>
/// Loads an 8-bit RAW file.
/// </summary>
/// <param name="file"></param>
void Dx11_VolumeRendering::loadRAWFile8(std::ifstream &file)
{
	int iFilelength = m_iWidth * m_iHeight * m_iDepth;
	m_pTex3D = new unsigned char[iFilelength];
	file.read(reinterpret_cast<char*>(m_pTex3D), sizeof(char) * iFilelength);
}

bool Dx11_VolumeRendering::Create3DTexture(ID3D11Device * pDevice, unsigned char *_pTex, int _iWidth, int _iHeight, int _iDepth)
{
	/*if (m_pScalars == NULL)
		return false;*/

		// CREATE TEXTURE3D
	D3D11_TEXTURE3D_DESC tex3Ddesc;
	ZeroMemory(&tex3Ddesc, sizeof(D3D11_TEXTURE3D_DESC));
	tex3Ddesc.Width = _iWidth;
	tex3Ddesc.Height = _iHeight;
	tex3Ddesc.MipLevels = 1;
	tex3Ddesc.Depth = _iDepth;
	tex3Ddesc.Format = DXGI_FORMAT_R8_UNORM;				// DXGI_FORMAT_R32_FLOAT;// DXGI_FORMAT_R8_UNORM;// DXGI_FORMAT_R32G32B32A32_FLOAT;	//IS THIS CORRECT FORMAT ? 
	tex3Ddesc.Usage = D3D11_USAGE_IMMUTABLE;
	tex3Ddesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tex3Ddesc.CPUAccessFlags = 0;
	tex3Ddesc.MiscFlags = 0;

	const unsigned int bytesPerPixel = 1;					//NO OF BYTES REQUIRE TO HOLD THE DATA
	unsigned int sliceSize = _iWidth * _iHeight * bytesPerPixel;
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(D3D11_SUBRESOURCE_DATA));
	initData.pSysMem = _pTex;// ;		
	initData.SysMemPitch = _iWidth * bytesPerPixel;
	initData.SysMemSlicePitch = sliceSize;

	HRESULT hr = pDevice->CreateTexture3D(&tex3Ddesc, &initData, &m_pTexture3D);
	if (hr != S_OK)
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = tex3Ddesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	srvDesc.Texture3D.MostDetailedMip = 0;
	srvDesc.Texture3D.MipLevels = tex3Ddesc.MipLevels;

	hr = pDevice->CreateShaderResourceView(m_pTexture3D, &srvDesc, &m_pTexture3DSRV);
	if (hr != S_OK)
		return false;


	return true;
}

void Dx11_VolumeRendering::RenderRayVolume(ID3D11DeviceContext * _pDeviceContext, D3DXMATRIX _w, D3DXMATRIX _v, D3DXMATRIX _p, ID3D11ShaderResourceView *_pFront, ID3D11ShaderResourceView *_pBack)
{

#ifdef RAY_CASTING
	
	UINT stride = sizeof(stVERTEX);
	UINT offset = 0;
	
	_pDeviceContext->IASetVertexBuffers(0, 1, &m_pFaceVB, &stride, &offset);
	_pDeviceContext->IASetIndexBuffer(m_pFaceIB, DXGI_FORMAT_R32_UINT, 0);
	_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	_pDeviceContext->IASetInputLayout(m_pFaceInLayout);

	_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);
	
	_pDeviceContext->PSSetShaderResources(0, 1, &_pFront);
	_pDeviceContext->PSSetShaderResources(1, 1, &_pBack);
	_pDeviceContext->PSSetShaderResources(2, 1, &m_pTexture3DSRV);
	
	_pDeviceContext->VSSetShader(m_pRayVS, 0, 0);
	_pDeviceContext->PSSetShader(m_pRayPS, 0, 0);
	
	_pDeviceContext->DrawIndexed(m_nFaceIndexCount, 0, 0);

#endif // RAY_CASTING

}

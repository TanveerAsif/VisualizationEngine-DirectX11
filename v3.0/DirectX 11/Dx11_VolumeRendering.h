#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <d3d11.h>
#include <D3DX10math.h>
#include <D3DX11async.h>

//#define MAX_NO_OF_SLICE 225 //1 //773
#define RAY_CASTING
//#define TEXTURE_3D

class Dx11_VolumeRendering
{
	struct stVERTEX
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 tex;
	};


	enum RENDERING_TYPE
	{
		EN_OBJECT_ALIGNED = 1,
		EN_VIEW_ALIGNED = 2,
		EN_RAY_CASTING = 3
	};
	RENDERING_TYPE					m_eRenderingType;

	ID3D11Buffer					*m_pMatrixBuffer = NULL;
	ID3D11SamplerState				*m_pSamplerState;

	ID3D11Texture3D					*m_pTexture3D = NULL;
	ID3D11ShaderResourceView		*m_pTexture3DSRV = NULL;

	int								m_iWidth, m_iHeight, m_iDepth, m_iStepDepth;
	std::string						m_strVolumeFile;
	unsigned char 					*m_pTex3D;
	float							*m_pScalars = NULL;


	void loadRAWFile(std::string strFileName);
	void loadRAWFile16(std::ifstream &file);
	void loadRAWFile8(std::ifstream &file);
	bool Create3DTexture(ID3D11Device *pDevice, unsigned char *_pTex, int _iWidth, int _iHeight, int _iDepth);

#ifdef TEXTURE_3D
	struct stMatrixBufferType
	{
		D3DXMATRIX worldViewProj;
	};

	struct stMatrixBufferType_W_V_P
	{
		D3DXMATRIX w;
		D3DXMATRIX v;
		D3DXMATRIX p;
	};

	

	struct stPOLYGON
	{
		D3DXVECTOR3 pos;
	};

	struct stCameraBuffer
	{
		D3DXVECTOR3 vCamPos;
		float		fStepSize;
	};


	struct stCameraAndColorBuffer
	{
		D3DXVECTOR3 vCamPos;
		float		fColorScaleValue;
	};


	ID3D11Buffer					*m_pVertexBuffer = NULL, *m_pIndexBuffer = NULL;
	

	
	bool InitShaderResource(ID3D11Device *pDevice, WCHAR *szShaderFilename);
	bool InitVolume(ID3D11Device *pDevice);

	bool IsPlaneEdgeIntersect(D3DXVECTOR3 _vOrigin, D3DXVECTOR3 _vLineDir, D3DXPLANE _Plane, D3DXVECTOR3 &_vIntersectionPoint);


	bool InitPolygonShader(ID3D11Device *pDevice, WCHAR *szShaderFilename);
	void RenderPolygon(ID3D11DeviceContext *_pDeviceContext, D3DXMATRIX _w, D3DXMATRIX _v, D3DXMATRIX _p, std::vector<D3DXVECTOR3> _vPolygonPoints);
	void RenderCube(ID3D11DeviceContext *pDeviceContext, D3DXMATRIX w, D3DXMATRIX v, D3DXMATRIX p);

	bool CompareByZ(D3DXVECTOR3 _p1, D3DXVECTOR3 _p2);
	void calc_plane_aabb_intersection_points(D3DXPLANE _Plane, D3DXVECTOR3 *_pCubeVertices, std::vector<D3DXVECTOR3 > &vIntersectionPoints);
	void sort_points(D3DXVECTOR3 *points, unsigned point_count, const D3DXPLANE &plane);

	ID3D11Device					*m_pDevice = NULL;
	int								m_nVertexCount, m_nIndexCount;
	ID3D11Buffer					*m_pPolyVB = NULL, *m_pPolyIB = NULL;
	int								m_iNearest = 0, m_iFarthest = 0;
	int                             m_iNumIntersectionPoint = 0;	
	ID3D11InputLayout				*m_pInputLayout, *m_pPolyInLayout = NULL;
	ID3D11Buffer					*m_pCameraBuffer = NULL, *m_pCameraAndColorScaleBuffer = NULL;
	ID3D11VertexShader				*m_pVS = NULL, *m_pPolyVS = NULL;
	ID3D11PixelShader				*m_pPS = NULL, *m_pPolyPS = NULL;
	float							m_fColorScaleValue = 0.1f;


	D3DXMATRIX						m_matProspectiveProj, m_matOrthoProj;

	stVERTEX						*m_pVertices = NULL;


#endif

#ifdef RAY_CASTING	

	struct stMatrixBuffer
	{
		D3DXMATRIX mWorld;
		D3DXMATRIX mView;
		D3DXMATRIX mProj;		
	};

	float							m_fYaw = 0.0f;
	int							    m_nFaceVertexCount, m_nFaceIndexCount, m_nRayVertexCount, m_nRayIndexCount;
	ID3D11Buffer					*m_pFaceVB = NULL, *m_pFaceIB = NULL, *m_pRayVB = NULL, *m_pRayIB = NULL;	
	ID3D11VertexShader				*m_pFaceVS = NULL, *m_pRayVS = NULL;
	ID3D11PixelShader				*m_pFacePS = NULL, *m_pRayPS = NULL;
	ID3D11InputLayout				*m_pFaceInLayout = NULL, *m_pRayInLayout = NULL;
	
	bool InitRayCast(ID3D11Device *pDevice);
	void RenderRay(ID3D11DeviceContext *_pDeviceContext, D3DXMATRIX _w, D3DXMATRIX _v, D3DXMATRIX _p);
	

#endif
	
public:
	Dx11_VolumeRendering(std::string strVolumeFile, int _iWidth, int _iHeight, int _iDepth);
	~Dx11_VolumeRendering();

	bool Init(ID3D11Device *pDevice);	
	void ShutDown();	
	void SetColorScaleValue(float _fScale);

	void Render(ID3D11DeviceContext *pDeviceContext, D3DXMATRIX w, D3DXMATRIX v, D3DXMATRIX p, D3DXVECTOR3 vCameraPos, D3DXVECTOR3 vCameraDir);
	void RenderRayVolume(ID3D11DeviceContext *_pDeviceContext, D3DXMATRIX _w, D3DXMATRIX _v, D3DXMATRIX _p, ID3D11ShaderResourceView *_pFront, ID3D11ShaderResourceView *_pBack);
	void GetNearestAndFarthestVerticesIndex(int &_iNearer, int &_iFarthest);
	void GetIntersectionPoint(int &_iIntersectionPoint);	

	void UpdateHeading();
};


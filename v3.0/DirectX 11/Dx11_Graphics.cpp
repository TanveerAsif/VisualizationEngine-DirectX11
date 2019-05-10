#include "Dx11_Graphics.h"
#include <D3DX10math.h>
#include <mmsystem.h>


const float SCREEN_DEPTH = 10000.0f;
const float SCREEN_NEAR = 0.1f;
const int SHADOW_WIDTH = 1920;// 1024;
const int SHADOW_HEIGHT = 1080;// 1024;

bool g_bComeOutFromWater = false;
bool g_bRenderWatertoAir = false;
float g_fInAirDuration = 0.0f;

//#define VOLUME_RENDER


void Dx11_Graphics::RenderFrontFace(float _fTick)
{
	ID3D11DeviceContext *pDeviceContext = m_pDirect3D->GetDeviceContext();
	m_pFrontFaceSurface->SetRenderTarget(pDeviceContext);
	m_pFrontFaceSurface->BeginScene(pDeviceContext);
	m_pDirect3D->SetRasterStateFrontFace();
	D3DXMATRIX matWorld = m_pDirect3D->GetWorldMatrix();
	D3DXMATRIX matView = m_pCamera->GetViewMatrix();
	D3DXMATRIX matProj = m_pFrontFaceSurface->GetProjectionMatrix();// m_pDirect3D->GetPerspectiveProjectionMatrix();

	//NOT USING
	D3DXVECTOR3 vCamPos = m_pCamera->GetPosition();
	D3DXVECTOR3 vCamDir = m_pCamera->GetDirection();

	m_pVolRender->Render(pDeviceContext, matWorld, matView, matProj, vCamPos, vCamDir);

	m_pDirect3D->SetBackBufferRenderTarget();
	m_pDirect3D->SetRasterSolid();
}

void Dx11_Graphics::RenderBackFace(float _fTick)
{
	ID3D11DeviceContext *pDeviceContext = m_pDirect3D->GetDeviceContext();
	m_pBackFaceSurface->SetRenderTarget(pDeviceContext);
	m_pBackFaceSurface->BeginScene(pDeviceContext);
	m_pDirect3D->SetRasterStateBackFace();
	D3DXMATRIX matWorld = m_pDirect3D->GetWorldMatrix();
	D3DXMATRIX matView = m_pCamera->GetViewMatrix();
	D3DXMATRIX matProj = m_pBackFaceSurface->GetProjectionMatrix();// m_pDirect3D->GetPerspectiveProjectionMatrix();

	//NOT USING
	D3DXVECTOR3 vCamPos = m_pCamera->GetPosition();
	D3DXVECTOR3 vCamDir = m_pCamera->GetDirection();

	m_pVolRender->Render(pDeviceContext, matWorld, matView, matProj, vCamPos, vCamDir);

	m_pDirect3D->SetBackBufferRenderTarget();
	m_pDirect3D->SetRasterSolid();
}

Dx11_Graphics::Dx11_Graphics()
{	
	m_pDirect3D = new Dx11_Direct3D();
	m_fTickCount = 0.0f;
	m_bRenderState = false;
	
	m_pText = new Dx11_Text();

	//Initialy WireFrame Mode OFF
	m_bIsWireFrameEnable = false;
	m_bSunLightEnable = true;
	m_bLightShaftEnable = false;

	m_pCamera = new Dx11_Camera();
	m_pFrontFaceSurface = new Dx11_Render2Texture();
	m_pBackFaceSurface = new Dx11_Render2Texture();

	m_pFrontModel = new Dx11_Model();
	m_pBackModel = new Dx11_Model();
	m_pFrontShader = new Dx11_Shaders();
	m_pBackShader = new Dx11_Shaders();
}


Dx11_Graphics::~Dx11_Graphics()
{

}


bool Dx11_Graphics::Initialize(HWND hWnd, UINT _width, UINT _height, std::string _strRawFilePath, UINT _ImageWidth, UINT _ImageHeight, UINT _NoOfImages)
{	
	bool bRetValue = false;	
	m_nScreenWidth = _width;// GetSystemMetrics(SM_CXSCREEN);
	m_nScreenHeight = _height;//GetSystemMetrics(SM_CYSCREEN);
	
	if (m_pDirect3D->Initialize(hWnd, m_nScreenWidth, m_nScreenHeight))
	{		

		//m_pCamera->SetPosition(D3DXVECTOR3(-550.0f, -37.0f, -3.0f));	
		
		ID3D11Device *pDevice = m_pDirect3D->GetDevice();
		ID3D11DeviceContext *pDeviceContext = m_pDirect3D->GetDeviceContext();
		
		D3DXMATRIX matViewMatrix = m_pCamera->GetViewMatrix();
		bRetValue = m_pText->Initialize(pDevice, pDeviceContext,
			"../../Data/fontdata.txt", L"../../Data/font.dds",
			matViewMatrix,
			m_nScreenWidth, m_nScreenHeight
		);
		if (!bRetValue)
			return bRetValue;


#ifndef VOLUME_RENDER
		m_pAssimp = new Dx11_Assimp();
		bRetValue = m_pAssimp->InitAssimp(hWnd, pDevice, pDeviceContext);
		if (!bRetValue)
		{
			MessageBox(hWnd, L"FAILED TO INITIALIZE ASSIMP : ", L"error", MB_ICONEXCLAMATION | MB_OK);
			return bRetValue;
		}

#else
		//James.raw
		//teapot.raw
		//m_pVolRender = new Dx11_VolumeRendering("Data/bighead.den", 256, 256, 225);
		m_uScaleWidth = _ImageWidth;
		m_uScaleHeight = _ImageHeight;
		m_uScaleDepth = _NoOfImages;
		m_pVolRender = new Dx11_VolumeRendering(_strRawFilePath, m_uScaleWidth, m_uScaleHeight, m_uScaleDepth);
		
		bRetValue = m_pVolRender->Init(pDevice);
		if (!bRetValue)
		{
			MessageBox(hWnd, L"FAILED TO INITIALIZE VolumetricData : ", L"error", MB_ICONEXCLAMATION | MB_OK);
			return bRetValue;
		}	
		
		m_pFrontFaceSurface->Initialize(pDevice, SHADOW_WIDTH, SHADOW_HEIGHT, SCREEN_NEAR, SCREEN_DEPTH);
		m_pBackFaceSurface->Initialize(pDevice, SHADOW_WIDTH, SHADOW_HEIGHT, SCREEN_NEAR, SCREEN_DEPTH);

		//
		m_pFrontModel->LoadFloor(pDevice, L"../../Data/dummy.dds");
		m_pBackModel->LoadFloor(pDevice, L"../../Data/dummy.dds");
		m_pFrontShader->InitializeFloorShader(pDevice, pDeviceContext, L"../../Data/MyShaderFloor.hlsl");
		m_pBackShader->InitializeFloorShader(pDevice, pDeviceContext, L"../../Data/MyShaderFloor.hlsl");

#endif
	}
	else
		MessageBox(hWnd, L"COULD NOT INITIALIZE Direct3D", L"error", MB_ICONEXCLAMATION | MB_OK);
	
			
	return bRetValue;
}

void Dx11_Graphics::Process(float _fTick)
{
	if (m_fHeading < 6.28f)
		m_fHeading += _fTick * 0.1;
	else
		m_fHeading = 0.0f;

	if (m_pVolRender)
		m_pVolRender->UpdateHeading();
}

void Dx11_Graphics::Render(float _fTick)
{
#ifdef VOLUME_RENDER
	RenderFrontFace(_fTick);
	RenderBackFace(_fTick);	
#endif
	RenderScene(_fTick);
}


void Dx11_Graphics::Shutdown()
{
	if (m_pFrontModel)
	{
		m_pFrontModel->Shutdown();
		delete m_pFrontModel;
	}

	if (m_pFrontShader)
	{
		m_pFrontShader->Shutdown();
		delete m_pFrontShader;
	}

	if (m_pBackModel)
	{
		m_pBackModel->Shutdown();
		delete m_pBackModel;
	}

	if (m_pBackShader)
	{
		m_pBackShader->Shutdown();
		delete m_pBackShader;
	}

	if (m_pFrontFaceSurface)
	{
		m_pFrontFaceSurface->Shutdown();
		delete m_pFrontFaceSurface;
	}

	if (m_pBackFaceSurface)
	{
		m_pBackFaceSurface->Shutdown();
		delete m_pBackFaceSurface;
	}


	if (m_pVolRender)
	{		
		m_pVolRender->ShutDown();
		delete m_pVolRender;				
	}

#ifndef VOLUME_RENDER
	if (m_pAssimp)
	{
		m_pAssimp->ShutDown();
		delete m_pAssimp;
		m_pAssimp = NULL;
	}
#endif
	
	if (m_pCamera)
	{
		delete m_pCamera;
		m_pCamera = 0;
	}

	if (m_pDirect3D)
	{
		m_pDirect3D->Shutdown();
		delete  m_pDirect3D;
		m_pDirect3D = 0;
	}


}

void Dx11_Graphics::RenderScene(float _fTick)
{
	if (m_pDirect3D->BeginScene())
	{
		ID3D11DeviceContext *pDeviceContext = m_pDirect3D->GetDeviceContext();
		D3DXMATRIX viewMat, worldMat, projectionMat, orthoMat;
		D3DXMATRIX matRotY, matTranslate, matRot, matScale, matTransform;

		viewMat = m_pCamera->GetViewMatrix();
		projectionMat = m_pDirect3D->GetPerspectiveProjectionMatrix();
		orthoMat = m_pDirect3D->GetOrthogonalProjectionMatrix();

#ifndef VOLUME_RENDER

		if (m_pAssimp)
		{		
			worldMat = m_pDirect3D->GetWorldMatrix();
			D3DXMatrixTranslation(&matTranslate, 0.0f, -10.0f, 100.0f);
			D3DXMatrixRotationY(&matRotY, -1 * D3DX_PI * 90 / 180);
			D3DXMatrixScaling(&matScale, 0.2f, 0.2f, 0.2f);
			worldMat = matScale * matRotY * matTranslate * worldMat;

			m_pAssimp->Render(m_pDirect3D->GetDevice(), pDeviceContext, worldMat, viewMat, projectionMat);
		}

#else

		ID3D11ShaderResourceView *pFront = m_pFrontFaceSurface->GetShaderResourceView();
		ID3D11ShaderResourceView *pBack = m_pBackFaceSurface->GetShaderResourceView();
		if (m_pFrontModel && m_pBackModel && m_bShowMapEnable)
		{	
			worldMat = m_pDirect3D->GetWorldMatrix();			
			D3DXMatrixTranslation(&matTranslate, -5.0f, 0.0f, 0.0f);
			D3DXMatrixScaling(&matScale, 10.0f, 10.0f, 10.0f);
			matTransform = /*matScale **/ matTranslate * worldMat;

			m_pFrontModel->RenderFloor(pDeviceContext);
			m_pFrontShader->RenderFloorShader(pDeviceContext, m_pFrontModel->GetFloorIndexCount(), matTransform, viewMat, projectionMat, pFront);

			D3DXMatrixTranslation(&matTranslate, 5.0f, 0.0f, 0.0f);
			D3DXMatrixScaling(&matScale, 10.0f, 10.0f, 10.0f);
			matTransform = /*matScale **/ matTranslate * worldMat;
			m_pBackModel->RenderFloor(pDeviceContext);
			m_pBackShader->RenderFloorShader(pDeviceContext, m_pBackModel->GetFloorIndexCount(), matTransform, viewMat, projectionMat, pBack);
		}


		//m_pDirect3D->EnableBlendStateVolume(); //SET DURING INITIALIZATION
		if (m_pVolRender)
		{			
			worldMat = m_pDirect3D->GetWorldMatrix();			
			D3DXMatrixTranslation(&matTranslate, -5.0f, -5.0f, -5.0f);
			D3DXMatrixScaling(&matScale, 10.0f, 10.0f, 10.0f);
			//worldMat = matScale * matTrans * worldMat;
			
			//m_pVolRender->Render(pDeviceContext, worldMat, viewMat, projectionMat,m_pCamera->GetPosition(), m_pCamera->GetDirection());
			m_pVolRender->RenderRayVolume(pDeviceContext, worldMat, viewMat, projectionMat, pFront, pBack);
		}
		//m_pDirect3D->SetRasterSolid();		

		int iN, iF, nPoints;
		m_pVolRender->GetNearestAndFarthestVerticesIndex(iN, iF);
		m_pVolRender->GetIntersectionPoint(nPoints);

#endif
		//RENDER TEXT
		//m_pDirect3D->SetDepthBufferOFF();
		m_pDirect3D->EnableBlendState();
		worldMat = m_pDirect3D->GetWorldMatrix();
		orthoMat = m_pDirect3D->GetOrthogonalProjectionMatrix();
		char strCameraPos[100];
		D3DXVECTOR3 vPos = m_pCamera->GetPosition();
		D3DXVECTOR3 vDir = m_pCamera->GetDirection();
		//sprintf_s(strCameraPos, "CameraPos = (%.0f,%0.0f,%0.0f) CameraDir(%.1f,%0.1f,%0.1f)", vPos.x, vPos.y, vPos.z, vDir.x, vDir.y, vDir.z);
		sprintf_s(strCameraPos, "CameraPos(%0.1f,%0.1f,%0.1f)", vPos.x, vPos.y, vPos.z);
		m_pText->Render(pDeviceContext, strCameraPos, worldMat, orthoMat);
		m_pDirect3D->DisableBlendState();
		//m_pDirect3D->SetDepthBufferON();
				
		m_pDirect3D->EndScene();
	}

}


void Dx11_Graphics::UpdateCamera(char action)
{
	m_pCamera->UpdateCamera(action);
}


void Dx11_Graphics::ShowWireFrame()
{	
	if (m_pDirect3D)
	{
		if (m_bIsWireFrameEnable == false)
			m_pDirect3D->SetRasterWireframe();
		else
			m_pDirect3D->SetRasterSolid();
		
		m_bIsWireFrameEnable = (m_bIsWireFrameEnable == false) ? true : false;		
	}
}

void Dx11_Graphics::SetTransferValue(float _fValue)
{	
	if (m_pVolRender)
	{
		m_pVolRender->SetColorScaleValue(_fValue);
	}
	
}

void Dx11_Graphics::CameraTransform(D3DXVECTOR2 _vScreenPoint)
{
	if (m_pCamera)
		m_pCamera->UpdateCamera(_vScreenPoint, m_pCamera->GetViewMatrix(), m_pDirect3D->GetPerspectiveProjectionMatrix());
}

void Dx11_Graphics::SetCameraLastDragPoint(D3DXVECTOR2 _vPoint)
{
	if (m_pCamera)
		m_pCamera->SetLastDragPoint(_vPoint);
}

void Dx11_Graphics::EnableShadowMap()
{
	m_bShowMapEnable = (m_bShowMapEnable == false) ? true : false;
}

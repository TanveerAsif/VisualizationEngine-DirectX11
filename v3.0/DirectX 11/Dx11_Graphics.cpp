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
}


Dx11_Graphics::~Dx11_Graphics()
{

}


bool Dx11_Graphics::Initialize(HWND hWnd, UINT _width, UINT _height)
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

		/*m_pAssimp = new Dx11_Assimp();
		bRetValue = m_pAssimp->InitAssimp(hWnd, pDevice, pDeviceContext);
		if (!bRetValue)
		{
			MessageBox(hWnd, L"FAILED TO INITIALIZE ASSIMP : ", L"error", MB_ICONEXCLAMATION | MB_OK);
			return bRetValue;
		}*/

		m_pTessellation = new Dx11_Tessellation();
		bRetValue = m_pTessellation->Init(pDevice, pDeviceContext);
		if (!bRetValue)
			return bRetValue;

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

}

void Dx11_Graphics::Render(float _fTick)
{
	RenderScene(_fTick);
}


void Dx11_Graphics::Shutdown()
{
	if (m_pTessellation)
	{
		m_pTessellation->Release();
		delete m_pTessellation;
		m_pTessellation = nullptr;
	}

	if (m_pAssimp)
	{
		m_pAssimp->ShutDown();
		delete m_pAssimp;
		m_pAssimp = NULL;
	}
	
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

		if (m_pAssimp)
		{		
			worldMat = m_pDirect3D->GetWorldMatrix();
			D3DXMatrixTranslation(&matTranslate, 0.0f, -10.0f, 100.0f);
			D3DXMatrixRotationY(&matRotY, -1 * D3DX_PI * 90 / 180);
			D3DXMatrixScaling(&matScale, 0.2f, 0.2f, 0.2f);
			worldMat = matScale * matRotY * matTranslate * worldMat;

			m_pAssimp->Render(m_pDirect3D->GetDevice(), pDeviceContext, worldMat, viewMat, projectionMat);
		}

		if (m_pTessellation)
		{			
			D3DXMatrixRotationX(&matRotY, -1 * D3DX_PI * 90 / 180);
			worldMat = m_pDirect3D->GetWorldMatrix();
			//worldMat = matRotY * worldMat;

			D3DXVECTOR3 vCamDis = m_pCamera->GetPosition() - D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			float fCamDistance = D3DXVec3Length(&vCamDis);
			m_pTessellation->Render(pDeviceContext, _fTick, worldMat, viewMat, projectionMat, fCamDistance);			
		}
			

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

void Dx11_Graphics::SetTessellationFactor(int _iFactor)
{
	if (m_pTessellation)
		m_pTessellation->UpdateTessellationFactor(_iFactor);
}


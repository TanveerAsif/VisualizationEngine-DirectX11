#include "Dx11_Camera.h"


Dx11_Camera::Dx11_Camera()
{
	m_fYaw = m_fPitch = 0.0f;
	m_vUp = D3DXVECTOR3(0, 1, 0);
	m_vPos = D3DXVECTOR3(0, 12, -100);
	m_vFrwdDir = D3DXVECTOR3(0, 0, 1);
	
	D3DXMatrixLookAtLH(&m_ViewMat, &m_vPos, &m_vFrwdDir, &m_vUp);
}


Dx11_Camera::~Dx11_Camera()
{
}



//void Dx11_Camera::Process()
//{
//
//	
//	D3DXVECTOR3 up = D3DXVECTOR3(0,1,0);
//	
//
//
//	D3DXMATRIX rotMatrix;
//	D3DXMatrixRotationYawPitchRoll(&rotMatrix, m_fYaw, m_fPitch, m_fRoll);
//	D3DXVec3TransformCoord(&up, &up, &rotMatrix);
//	D3DXVec3TransformCoord(&m_vFrwdDir, &m_vFrwdDir, &rotMatrix);
//
//	/*D3DXVECTOR3 Frwd;
//	Frwd.x = cosf(m_fPitch)*sinf(m_fYaw);
//	Frwd.y = sinf(m_fPitch);
//	Frwd.z = cosf(m_fPitch)*cosf(m_fYaw);
//
//	D3DXVECTOR2 tempFrwd = D3DXVECTOR2(Frwd.x, Frwd.z);
//	D3DXVec2Normalize(&tempFrwd, &tempFrwd);
//	m_vFrwdDir = D3DXVECTOR3(tempFrwd.x, 0, tempFrwd.y);*/
//
//
//	m_vFrwdDir = m_vFrwdDir + m_vPos;
//	D3DXMatrixLookAtLH(&m_ViewMat, &m_vPos, &up, &m_vFrwdDir);
//}





void Dx11_Camera::UpdateCamera(char in)
{
	float speed = 0.1f, fSpeedFactor = 5;
	float amt = 10 * speed;
	float angle = 100 * speed;


	switch (in)
	{
		case VK_LEFT:
		{
			if (GetAsyncKeyState(VK_SHIFT))
			{
				m_fYaw -= 0.05f;
			}
			else
			{
				D3DXVECTOR3 left = GetLeft();
				if (GetAsyncKeyState(VK_CONTROL))
				{
					Move(&m_vPos, left, fSpeedFactor * amt);			//CHANGING POS								
				}
				else
				{
					Move(&m_vPos, left, amt);
				}

			}

		}
		break;

		case VK_RIGHT:
		{
			if (GetAsyncKeyState(VK_SHIFT))
			{
				m_fYaw += 0.05f;
			}
			else
			{
				D3DXVECTOR3 ryt = GetRight();

				if (GetAsyncKeyState(VK_CONTROL))
				{
					Move(&m_vPos, ryt, fSpeedFactor * amt);
				}
				else
				{
					Move(&m_vPos, ryt, amt);			//CHANGING POS								
				}
			}			
		}
		break;

		case VK_UP:
		{
			if (GetAsyncKeyState(VK_CONTROL))
			{
				Move(&m_vPos, m_vFrwdDir, fSpeedFactor*amt);
			}
			else
			{
				Move(&m_vPos, m_vFrwdDir, amt);
			}			
		}
		break;

		case VK_DOWN:
		{				
			if (GetAsyncKeyState(VK_CONTROL))
			{
				Move(&m_vPos, m_vFrwdDir, -fSpeedFactor *amt);
			}
			else
			{
				Move(&m_vPos, m_vFrwdDir, -amt);
			}			
		}
		break;

		case VK_PRIOR: //Up			
		{
			if (GetAsyncKeyState(VK_CONTROL))
			{
				Move(&m_vPos, m_vUp, fSpeedFactor*amt);
			}
			else
			{
				Move(&m_vPos, m_vUp, amt);
			}
			break;
		}
			

		case VK_NEXT: //Down					
		{
			if (GetAsyncKeyState(VK_CONTROL))
			{
				Move(&m_vPos, m_vUp, -fSpeedFactor * amt);
			}
			else
			{
				Move(&m_vPos, m_vUp, -amt);
			}
			break;
		}

		case 'R':
		{
			m_fPitch = 0.0f;
			m_fYaw = 0.0f;
			m_vUp = D3DXVECTOR3(0, 1, 0);
			m_vPos = D3DXVECTOR3(0, 10, -100);
			m_vFrwdDir = D3DXVECTOR3(0, 0, 1);
			break;
		}

		case 'T':		
		{
			if (m_fPitch < 6.28)
				m_fPitch += 0.05f;
			else
				m_fPitch = 0.0f;
			break;
		}

		/*case WM_MOUSEHWHEEL:
			m_fPitch -= 0.05f;*/

			break;
	}

	D3DXVECTOR3 vDir;
	vDir.x = cosf(m_fPitch)*sinf(m_fYaw);
	vDir.y = sinf(m_fPitch);
	vDir.z = cosf(m_fPitch)*cosf(m_fYaw);

	////D3DXVECTOR2 vFrwdDir = D3DXVECTOR2(vDir.x, vDir.z);
	////D3DXVec2Normalize(&vFrwdDir, &vFrwdDir);
	////m_vFrwdDir = D3DXVECTOR3(vFrwdDir.x, 0.0f, vFrwdDir.y);
	m_vFrwdDir = vDir;
	

	D3DXVECTOR3 vTarget = m_vPos + m_vFrwdDir;
	

	D3DXMatrixLookAtLH(&m_ViewMat, &m_vPos, &vTarget, &m_vUp);
	
}


void Dx11_Camera::Move(D3DXVECTOR3 *pos, D3DXVECTOR3 dir, int amount)
{
	D3DXVec3Scale(&dir, &dir, amount);
	D3DXVec3Add(pos, pos, &dir);
}

D3DXVECTOR3 Dx11_Camera::GetLeft()
{
	D3DXVECTOR3 left;
	D3DXVec3Cross(&left, &m_vFrwdDir, &m_vUp);
	D3DXVec3Normalize(&left, &left);
	return left;
}


D3DXVECTOR3 Dx11_Camera::GetRight()
{
	D3DXVECTOR3 right;
	D3DXVec3Cross(&right, &m_vUp, &m_vFrwdDir);
	D3DXVec3Normalize(&right, &right);
	return right;
}


D3DXMATRIX Dx11_Camera::GetViewMatrix(D3DXVECTOR3 vCameraPos, D3DXVECTOR3 vCameraDir)
{
	//REFLECTION FROM 	
	//D3DXVec3Normalize(&vCameraDir, &vCameraDir);

	D3DXMATRIX matReflection;
	D3DXMatrixLookAtLH(&matReflection, &vCameraPos, &vCameraDir, &m_vUp);
	
	return matReflection;
}

D3DXMATRIX Dx11_Camera::GetViewMatrix(float posx, float posy, float posz, float dirx, float diry, float dirz)
{
	D3DXVECTOR3 vCameraPos(posx, posy, posz);
	D3DXVECTOR3 vCameraDir(dirx, diry, dirz);

	D3DXMATRIX matReflection;
	D3DXMatrixLookAtLH(&matReflection, &vCameraPos, &vCameraDir, &m_vUp);

	return matReflection;
}



void Dx11_Camera::UpdateCamera(D3DXVECTOR2 _vPoint, D3DXMATRIX _matView, D3DXMATRIX _matProj)
{
	int w = 1920, h = 1080;

	
	float dx = _vPoint.x - m_vLastDragPoint.x;
	float dy = _vPoint.y - m_vLastDragPoint.y;

	////Get Mouse Point
	////Normalize the Point (-1, 1);
	////create a vector with the z coordinate as 1
	////Transform this point by Inverse(Proj) * Inverse(View) 

	//float x = ((2.0f * dx) / w) - 1.0f;
	//float y = ((2.0f * dy) / h) - 1.0f;

	//D3DXVECTOR3 mousePosition(x, y, 1.0f);

	//D3DXMATRIX matVP = _matView * _matProj, matInverseVP;
	//FLOAT   fDeterment = D3DXMatrixDeterminant(&matVP);
	//D3DXMatrixInverse(&matInverseVP, &fDeterment, &matVP);

	//D3DXVECTOR4 vMouseWorldPos;
	//D3DXVec3Transform(&vMouseWorldPos, &mousePosition, &matInverseVP);

	////DirectX::XMVECTOR3 directXVec3;
	///*DirectX::XMVECTOR4 v1;
	//DirectX::XMStoreFloat3(mousePosition, vMouseWorldPos);*/
	//D3DXVECTOR3 vCameraFrwdDir = m_vPos - D3DXVECTOR3(vMouseWorldPos.x / vMouseWorldPos.w, vMouseWorldPos.y / vMouseWorldPos.w, vMouseWorldPos.z / vMouseWorldPos.w);
	//D3DXVec3Normalize(&vCameraFrwdDir, &vCameraFrwdDir);
	

	// Slow Down By  : 0.05;
	m_fYaw = m_fYaw + atan((dx / w) * _matProj._11) * 0.05;
	m_fPitch = m_fPitch + atan((dy / h) * _matProj._22) * 0.05;

	D3DXVECTOR3 vDir;
	vDir.x = cosf(m_fPitch)*sinf(m_fYaw);
	vDir.y = sinf(m_fPitch);
	vDir.z = cosf(m_fPitch)*cosf(m_fYaw);

	D3DXVECTOR2 vFrwdDir = D3DXVECTOR2(vDir.x, vDir.z);
	D3DXVec2Normalize(&vFrwdDir, &vFrwdDir);
	m_vFrwdDir = D3DXVECTOR3(vFrwdDir.x, 0.0f, vFrwdDir.y);


	D3DXVECTOR3 vTarget;
	D3DXVec3Add(&vTarget, &m_vPos, &vDir);
	D3DXMatrixLookAtLH(&m_ViewMat, &m_vPos, &vTarget, &m_vUp);
}

void Dx11_Camera::SetLastDragPoint(D3DXVECTOR2 _vPoint)
{
	m_vLastDragPoint = _vPoint;
}



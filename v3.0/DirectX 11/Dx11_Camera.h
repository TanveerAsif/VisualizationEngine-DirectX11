#pragma once

#include <D3DX10math.h>

class Dx11_Camera
{
private:

	D3DXVECTOR2						m_vLastDragPoint = {0.0f, 0.0f};

	float m_fRoll, m_fPitch, m_fYaw;
	D3DXVECTOR3 m_vPos, m_vUp, m_vFrwdDir;
	D3DXMATRIX m_ViewMat;



	void Move(D3DXVECTOR3 *pos, D3DXVECTOR3 dir, int amount);
	D3DXVECTOR3 GetLeft();
	D3DXVECTOR3 GetRight();

public:
	Dx11_Camera();
	~Dx11_Camera();

	void SetPosition(D3DXVECTOR3 pos) { m_vPos = pos; }
	D3DXVECTOR3 GetPosition() { return m_vPos; }

	/*void Process();*/
	void UpdateCamera(char in);

	D3DXMATRIX GetViewMatrix() { return m_ViewMat; }
	D3DXMATRIX GetViewMatrix(D3DXVECTOR3 vCameraPos, D3DXVECTOR3 vCameraDir);
	D3DXMATRIX GetViewMatrix(float posx, float posy, float posz, float dirx, float diry, float dirz);

	D3DXVECTOR3 GetDirection() { return m_vFrwdDir; }

	void UpdateCamera(D3DXVECTOR2 _vPoint, D3DXMATRIX _matView, D3DXMATRIX _matProj);
	void SetLastDragPoint(D3DXVECTOR2 _vPoint);
};


#pragma once

#include <Windows.h>
#include "Dx11_Input.h"
#include "Dx11_Graphics.h"




class Dx11_Engine
{

private:
	HINSTANCE		m_hInstance;
	HWND			m_hWnd;

	Dx11_Graphics	*m_pGraphics;
	Dx11_Input		*m_pInput;

	bool			m_bLButtonDown = false;

	bool InitializeWindow();
	bool UpdateKeyPress();
public:
	Dx11_Engine();
	~Dx11_Engine();


	bool Initialize(HWND _hWnd, unsigned int _uWidth, unsigned int _uHeight);
	void Run();
	void Shutdown();
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);
	void SetTransferFunctionValue(float _fValue);
};


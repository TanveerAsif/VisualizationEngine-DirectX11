#include "Dx11_Engine.h"



static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static Dx11_Engine *g_pEngineHandler;


Dx11_Engine::Dx11_Engine()
{
	m_hInstance = NULL;
	m_hWnd = NULL;

	m_pGraphics = NULL;
	m_pInput = NULL;
}


Dx11_Engine::~Dx11_Engine()
{
}


bool Dx11_Engine::Initialize(HWND _hWnd, unsigned int _uWidth, unsigned int _uHeight)
{
	bool bRetValue = false;

#ifdef VISUALIZATION_APPLICATION
	if (!InitializeWindow())
		return false;
#else
	m_hWnd = _hWnd;
#endif		

	m_pGraphics = new Dx11_Graphics();
	m_pInput = new Dx11_Input();

	if (m_pGraphics && m_pInput)
	{
		m_pInput->Initialize();			
		if (m_pGraphics->Initialize(m_hWnd, _uWidth, _uHeight))
		{
			bRetValue = true;
		}			
	}
	
	return bRetValue;
}





void Dx11_Engine::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	//NOTE : Winmm.lib ADD INTO LINKER TO RESOLVE timeGetTime() 
	//Returns the system time, in milliseconds.
	unsigned int tickCount = 0, lastTickCount = timeGetTime();
	double dTickDuration = 0;

	while(true)
	{
		tickCount = timeGetTime();
		float fTickDiff = (float)((tickCount - lastTickCount) / 1000.0);
		dTickDuration += (double)((tickCount - lastTickCount) / 1000.0);
		lastTickCount = tickCount;

		while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}		

		if (msg.message == WM_QUIT)
			break;

		if (UpdateKeyPress() == false)
			break;

		/*char msgbuf[100];
		sprintf_s(msgbuf, "Render Interval Tick %f\n", dTickDuration);
		OutputDebugStringA(msgbuf);*/

		//if (dTickDuration > 1)
		{
			dTickDuration = 0; //Reset

			m_pGraphics->Process(fTickDiff);
			m_pGraphics->Render(fTickDiff);
		}	

	}

}



bool Dx11_Engine::InitializeWindow()
{
	g_pEngineHandler = this;

	m_hInstance = GetModuleHandle(NULL);
	
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"Engine";
	wc.cbSize = sizeof(WNDCLASSEX);
	if (!RegisterClassEx(&wc))
		return false;


	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, L"Engine", L"Engine", WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP
	, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)
	, 0, 0, m_hInstance, NULL);
	
	if (m_hWnd == NULL)
		return false;

	ShowWindow(m_hWnd, SW_SHOW);
	return true;
	
}

bool Dx11_Engine::UpdateKeyPress()
{
	if (m_pInput->IsKeyDown(VK_ESCAPE))
		return false;

	if (m_pInput->IsKeyDown('W'))
	{
		m_pInput->KeyUp('W');
		m_pGraphics->ShowWireFrame();
	}

	if (m_pInput->IsKeyDown('R'))
	{
		m_pInput->KeyUp('R');
		m_pGraphics->UpdateCamera('R');
	}

	if (m_pInput->IsKeyDown('T'))
	{
		m_pInput->KeyUp('T');
		m_pGraphics->SetTessellationFactor(1);
	}

	if (m_pInput->IsKeyDown('Y'))
	{
		m_pInput->KeyUp('Y');
		m_pGraphics->SetTessellationFactor(-1);
	}

	if (m_pInput->IsKeyDown('M'))
	{
		m_pInput->KeyUp('M');
		m_pGraphics->EnableShadowMap();
	}

	if (m_pInput->IsKeyDown(VK_LEFT))
	{
		m_pInput->KeyUp(VK_LEFT);
		m_pGraphics->UpdateCamera(VK_LEFT);
	}

	if (m_pInput->IsKeyDown(VK_RIGHT))
	{
		m_pInput->KeyUp(VK_RIGHT);
		m_pGraphics->UpdateCamera(VK_RIGHT);
	}

	if (m_pInput->IsKeyDown(VK_UP))
	{
		m_pInput->KeyUp(VK_UP);
		m_pGraphics->UpdateCamera(VK_UP);
	}

	if (m_pInput->IsKeyDown(VK_DOWN))
	{
		m_pInput->KeyUp(VK_DOWN);
		m_pGraphics->UpdateCamera(VK_DOWN);
	}

	if (m_pInput->IsKeyDown(VK_PRIOR))
	{
		m_pInput->KeyUp(VK_PRIOR);
		m_pGraphics->UpdateCamera(VK_PRIOR);
	}

	if (m_pInput->IsKeyDown(VK_NEXT))
	{
		m_pInput->KeyUp(VK_NEXT);
		m_pGraphics->UpdateCamera(VK_NEXT);
	}

	////if (msg.message == WM_MOUSEWHEEL)
	////{
	////	//m_pGraphics->UpdateCamera('T');
	////}

	return true;
}

LRESULT CALLBACK Dx11_Engine::MessageHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{

		case WM_KEYDOWN:
			m_pInput->KeyDown((unsigned int)wParam);
			break;

		case WM_KEYUP:
			m_pInput->KeyUp((unsigned int)wParam);
			break;

		case WM_LBUTTONUP:
		{
			m_bLButtonDown = false;
			break;
		}

		case WM_MOUSEMOVE:
		{
			if (MK_LBUTTON & wParam)
			{
				D3DXVECTOR2 vPt(float((short)LOWORD(lParam)), float((short)HIWORD(lParam)));
				static bool once = true;
				if (once)
				{
					m_pGraphics->SetCameraLastDragPoint(vPt);
					once = false;
				}
				if (m_bLButtonDown == false)
				{
					m_bLButtonDown = true;
					m_pGraphics->SetCameraLastDragPoint(vPt);
				}

				
				m_pGraphics->CameraTransform(vPt);
			}			
			break;
		}

			

		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
		
	}	
	return S_OK;
}

void Dx11_Engine::SetTransferFunctionValue(float _fValue)
{
	if (m_pGraphics)
		m_pGraphics->SetTransferValue(_fValue);
}




LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{

	default:
		return g_pEngineHandler->MessageHandler(hWnd, msg, wParam, lParam);
	}
}

void Dx11_Engine::Shutdown()
{
	if (m_pGraphics)
	{
		m_pGraphics->Shutdown();
		delete m_pGraphics;
		m_pGraphics = NULL;
	}


	if (m_pInput)
	{
		delete m_pInput;
		m_pInput = NULL;
	}

}
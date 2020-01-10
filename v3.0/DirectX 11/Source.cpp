#include "Dx11_Engine.h"


Dx11_Engine *g_pEngine = NULL;


#ifdef VISUALIZATION_APPLICATION

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdShow)
{

	g_pEngine = new Dx11_Engine();	
	if (g_pEngine->Initialize(0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN)))
	{
		g_pEngine->Run();
	}
	g_pEngine->Shutdown();

	return 0;
}

#elif


//typedef void (CALLBACK * fnCallBackFunc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

extern "C"
{
	__declspec(dllexport) bool InitializeDevice(HWND _hWnd, char *_strFilePath, unsigned int _uWidth, unsigned int _uHeight)
	{
		g_pEngine = new Dx11_Engine();
		if (g_pEngine)
		{
			return g_pEngine->Initialize(_hWnd, _uWidth,  _uHeight);
		}
		return false;
	}

	__declspec(dllexport) bool InitializeDeviceAndVolume(HWND _hWnd, unsigned int _uWidth, unsigned int _uHeight,char *_strRawFilePath, unsigned int _uImageWidth, unsigned int _uImageHeight, unsigned int _uNoOfImages)
	{
		g_pEngine = new Dx11_Engine();
		if (g_pEngine)
		{
			//, _strRawFilePath, _uImageWidth, _uImageHeight, _uNoOfImages
			return g_pEngine->Initialize(_hWnd, _uWidth, _uHeight);
		}
		return false;
	}
	
	__declspec(dllexport) void Render(float _fTick)
	{
		if (g_pEngine)
		{
			g_pEngine->Run();
		}
	}

	__declspec(dllexport) void Release()
	{
		if (g_pEngine)
		{
			g_pEngine->Shutdown();
		}
	}


	__declspec(dllexport) void Register_Callback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (g_pEngine)
		{ 
			g_pEngine->MessageHandler(hWnd,  msg,  wParam,  lParam);
		}
	}

	__declspec(dllexport) void SetColorFactor(float _fValue)
	{
		if (g_pEngine)
			g_pEngine->SetTransferFunctionValue(_fValue);
	}
}



#endif
#include "PlatformHelper.h"



#ifdef WinEGL
#include "WindowsPlatform.h"
//#include "EGLService.h"

void PlatformHelper::CreateWindowAndDevice(int w, int h)
{
	//WindowsPlatform wp;
	//wp.height = h;
	//wp.width = w;
	//wp.MakeWindow();
	//eglService = new EGLService();
	//eglService->Initialise(wp.hWnd);
}


void PlatformHelper::SwapBuffer()
{
	//eglService->SwapBuffers();
}

#endif 


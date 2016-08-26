#pragma once

#define WinEGL
//#define WinDX11  //not support
//#define WinGLFW  //not support
//#define ANDROID



class EGLService;
class PlatformHelper {
public:
	void CreateWindowAndDevice(int w, int h);
	void SwapBuffer();
	EGLService* eglService;
};
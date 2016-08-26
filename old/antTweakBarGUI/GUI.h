#pragma once
#pragma comment(lib,"AntTweakBar.lib")
#include <AntTweakBar.h>
#include "PreDefine.h"

class GUI {
public:
	int w=0, h=0;
	Pipeline* pipeline=NULL;
	App* app = NULL;

	void Init(int _w,int _h) {
		w = _w;
		h = _h;
		TwInit(TW_OPENGL, NULL);
		TwWindowSize(w, h);
	}
	void Render() {
		TwDraw();
	}
	void Release() {
		TwTerminate();
	}
	void KeyEvent(int key,int action) {
		TwEventKeyGLFW(key, action);
	}
	void MouseButtonEvent(int button,int action) {
		TwEventMouseButtonGLFW(button, action);
	}
	void MouseMoveEvent(double x, double y) {
		TwEventMousePosGLFW(x, y);
	}
	void CharEvent(unsigned ch) {
		TwEventCharGLFW(ch, GLFW_PRESS);
	}

	virtual void SetupBars() {};
};


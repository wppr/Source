#pragma once

#include "ContextChoose.h"
#pragma comment(lib, "glfw3.lib")
#ifdef USE_OPENGL_CONTEXT
#include "GLContext.h"
#define GLFW_EXPOSE_NATIVE_WGL
#endif 

#ifdef USE_OPENGL_ES_CONTEXT
#include "WinEGLContext.h"
#define GLFW_EXPOSE_NATIVE_EGL
#endif 

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <stdio.h>
#include <assert.h>
#include <windows.h>
#include <chrono>
#include <thread>
#include "AppFrameWork2.h"
#include "InteractionControler.h"
#include "GUIWrapper.h"
#include "Logger.h"
using namespace std;

struct MonitorConfig {
	bool bfullscreen = true;
	int monitor_id = 0;
	META(N(bfullscreen), N(monitor_id));
};

class GLFWFrameWork
{
public:
	GLFWwindow* window = NULL;
	int w = 1024, h = 756;
	HWND hwnd;
	App* app;
	GUI imgui;
	double LastTime;
	int samples = 4;
	bool pause = false;
	MonitorConfig mconfig;
	GLFWFrameWork()
	{
		
	}
	 
	void Init() {
		if (!glfwInit()) {
			fprintf(stderr, "ERROR: could not start GLFW3\n");
			exit(1);
		}	
		//glfwWindowHint(GLFW_SAMPLES, samples);

		serializeLoad("MonitorConfig.txt", mconfig);
		int count;
		GLFWmonitor** monitors = glfwGetMonitors(&count);
		if(!mconfig.bfullscreen)
			window = glfwCreateWindow(w, h, "Window", NULL, NULL);
		else {
			window = glfwCreateWindow(w, h, "Window", NULL, NULL);
		}
		
		if (!window) {
			fprintf(stderr, "ERROR: could not open window with GLFW3\n");
			glfwTerminate();
			exit(1);
		}
		glfwMakeContextCurrent(window);
		hwnd = glfwGetWin32Window(window);

#ifdef USE_OPENGL_CONTEXT
		GLContext Context;
		Context.Init();
#endif

#ifdef USE_OPENGL_ES_CONTEXT
		WinEGLContext Context;
		Context.Init(hwnd);
#endif
		
		app->Init();
		glfwSetWindowSize(window, app->w, app->h);
		imgui.Init(window);

		//Init Interactive
		InteractionControler::SetCamera(app->camera);
		InteractionControler::SetWindow(window);
		InteractionControler::gui = &imgui;

		glfwSetKeyCallback(window, InteractionControler::key_callback);
		glfwSetMouseButtonCallback(window, InteractionControler::mouse_button_callback);
		glfwSetCursorPosCallback(window, InteractionControler::mouse_move_callback);
		glfwSetScrollCallback(window, InteractionControler::scroll_callback);
		glfwSetWindowFocusCallback(window, InteractionControler::focus_callback);
		glfwSetWindowIconifyCallback(window, InteractionControler::iconify_callback);
		glfwSetCharCallback(window, InteractionControler::char_callback);

		LastTime = glfwGetTime();
		glfwSwapInterval(0);
	}

	void Loop() {
		while (!glfwWindowShouldClose(window)) {
			if (pause) {
				std::this_thread::sleep_for(1s);
				continue;
			}
			glfwPollEvents();
			app->AbsolutTime = glfwGetTime();
			app->Render();
			imgui.Render();
			InteractionControler::key_func();
			
			glfwSwapBuffers(window);

			CalcFps();
		}

		imgui.ShutDown();
		glfwTerminate();

	}
	void CalcFps() {
		static int FrameNum = 0;
		FrameNum++;
		auto now = glfwGetTime();
		if (now - LastTime > 1.0) {
			cout << "FPS:" << FrameNum ;
			if (FrameNum > 0)
				cout << " " << 1000.0 / FrameNum<<"ms";
			cout << endl;
			LastTime = now;
			FrameNum = 0;
		}
	}
};



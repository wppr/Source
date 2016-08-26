#pragma once
#include "EGLRenderSystem.h"
#include <sstream>
#include "FrameListener.h"
#include "Timer.h"
#include "Logger.h"
#include <thread>
#include <chrono>
#include "App_SSS.h"
#include "Config.h"
using namespace std;

class PCFrameWork
{
public:
	PCFrameWork()
	{
		rendersys = NULL;
	}

	void Init() {
		Config::instance().Load();
		renderer = new app_SSS;
		createEGLDisplay();
		renderer->render = rendersys;
		renderer->Init();
		
	}
	void Loop() {
		
		Go();

	}

	virtual void createEGLDisplay()
	{
		Logger::WriteLog("Create EGLDisplay()");
		if (NULL == rendersys)
		{
			auto eglrender = new EGLRenderSystem;
			rendersys = eglrender;
			int w = 1024, h = 1024;
			if (Config::instance().valid) {
				w = Config::instance().width;
				h = Config::instance().height;
			}
				
			rendersys->SetWandH(w, h);
		}
		rendersys->Initialize();
		rendersys->GetWandH(renderer->w, renderer->h);
		
	}

	void Go()
	{
		
		MSG msg;
		while (true)
		{
			
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					break;
				else
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			else
			{
				auto start = std::chrono::steady_clock().now();
				
				renderer->Render();
				
				//LogCameraPosition();
				//calc fps
				static int frame = 0;
				static auto elpased = chrono::nanoseconds(0);
				elpased += std::chrono::steady_clock().now() - start;
				frame++;
				if (chrono::duration_cast<std::chrono::milliseconds>(elpased).count() > 1000) {
					elpased = chrono::nanoseconds(0);
					cout << "fps_" << frame << endl;
					//Logger::WriteAndroidInfoLog("fps _ %d", frame);
					frame = 0;

				}
				renderer->sysentry->getRenderSystem()->BufferSwap();
				
				//calculatedFPS();
				////limit fps
				//int max_fps = 30;
				//float frametime = 1000.0 / max_fps;
				//auto end = std::chrono::steady_clock().now();
				//auto delta = end - start;
				//auto ms = chrono::duration_cast<std::chrono::milliseconds>(delta);
				//auto sleeptime = frametime - ms.count();
				//if (sleeptime > 0)
				//	std::this_thread::sleep_for(std::chrono::milliseconds(int(sleeptime)));


			}


		}

	}
	void term_display()
	{
		Logger::WriteLog(" app term display");
		if (NULL != rendersys)
		{
			rendersys->term_display();
			Logger::WriteLog(" egl context release");
		}
	}
	void destroy()
	{
		term_display();

	}

	virtual void calculatedFPS()
	{
		static int frame = 0;
		frame++;
		double elapsed = timer.elapsed();
		if (elapsed > 1000.0)
		{
			Logger::WriteLog("FPS %d", frame);

			timer.restart();
			frame = 0;
		}
	}
	~PCFrameWork()
	{
		destroy();
		Logger::close();
	}

public:
	Timer timer;
	RenderSystem * rendersys;
	app_SSS* renderer;

};



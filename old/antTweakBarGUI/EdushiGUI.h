#pragma once
#include "GUI.h"
#include "app_edushi.h"
#include "mathlib.h"
using namespace HW;
class EdushiGUI :public GUI {
public:
	void SetupBars() {
		//static float fps = 45;
		//static float a[3] = { 2,1,0 };
		//static float b[3] = { 2,1,0 };
		//static float q[4] = { 1,2,3,4 };
		//auto bar1 = TwNewBar("Params1");
		//TwAddVarRO(bar1, "a", TW_TYPE_FLOAT, &fps, "");
		//static Vector3 d = Vector3(0, 0, 1);
		auto bar = TwNewBar("Params");
		auto p=static_cast<EdushiPipeline*>(pipeline);
		TwAddVarRW(bar, "metalness", TW_TYPE_FLOAT,&(p->metalness)," min = 0 max = 1 step = 0.01");
		TwAddVarRW(bar, "roughness", TW_TYPE_FLOAT, &(p->roughness), " min = 0.01 max = 1 step = 0.01");
		//TwAddVarRW(bar2, "Vec", TW_TYPE_DIR3F, &a, "");
		//TwAddVarRW(bar2, "Quat", TW_TYPE_QUAT4F, &q, "");
	}
};
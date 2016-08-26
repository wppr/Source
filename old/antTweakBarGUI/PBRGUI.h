#pragma once
#include "GUI.h"
#include "PBRPipeline.h"
#include "mathlib.h"
using namespace HW;
class PBRGUI :public GUI {
public:
	void SetupBars(){
		//static float fps = 45;
		//static float a[3] = { 2,1,0 };
		//static float b[3] = { 2,1,0 };
		//static float q[4] = { 1,2,3,4 };
		//auto bar1 = TwNewBar("Params1");
		//TwAddVarRO(bar1, "a", TW_TYPE_FLOAT, &fps, "");
		//static Vector3 d = Vector3(0, 0, 1);
		auto bar2 = TwNewBar("Params");
		//auto p=static_cast<PBRPipeline2*>(pipeline);
		//TwAddVarRW(bar2, "Base_color", TW_TYPE_COLOR3F,&(p->base_color),"");
		//TwAddVarRW(bar2, "Vec", TW_TYPE_DIR3F, &a, "");
		//TwAddVarRW(bar2, "Quat", TW_TYPE_QUAT4F, &q, "");
	}
};
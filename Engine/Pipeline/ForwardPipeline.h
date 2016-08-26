#pragma once
#include "effect.h"
#include "PreDefine.h"
#include <string>
#include "Pipeline.h"
#include "ShowTexturePipeline.h"
using namespace std;
using namespace HW;


class ForwardEffect :public PipelineEffect{
public:
	virtual void Init();
	virtual void Render();

	//input
	string input_scenename;
	string input_cameraname;

	//output
	RenderTarget* output_RT=NULL;
	Texture* out_depth, *out_color; 

private:
	as_Pass* forward_pass;
};



class ForwardPipeline :public Pipeline {
public:
	virtual void Init();
	virtual void Render();

	ForwardEffect eForward;
	ShowTextureEffect eShowtex;
};
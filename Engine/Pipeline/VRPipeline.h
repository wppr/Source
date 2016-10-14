#pragma once
#include "effect.h"
#include "PreDefine.h"
#include <string>
#include "Pipeline.h"
#include "ForwardPipeline.h"
#include "ShowTexturePipeline.h"
#include "EGLTexture.h"
#include "EGLUtil.h"
using namespace std;
using namespace HW;

class VRPipeline :public Pipeline {
public:
	virtual void Init();
	virtual void Render();

	RenderTarget* left, *right;

	Texture* colorLeft, *colorRight;
	as_Pass* forward_pass;
	//ForwardEffect eForward;
	ShowTextureEffect eShowtex;
	Matrix4 projLeft, viewLeft, projRight, viewRight;

	void GetStereoTex(GLuint& l, GLuint& r);
};
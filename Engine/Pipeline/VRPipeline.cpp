#include "VRPipeline.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"
#include "GlobalVariablePool.h"

void VRPipeline::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	left = rt_mgr.CreateRenderTargetFromPreset("basic", "effect_forward");
	left->createInternalRes();
	colorLeft = left->m_attach_textures["color0"];

	right = rt_mgr.CreateRenderTargetFromPreset("basic", "effect_forward");
	right->createInternalRes();
	colorRight = right->m_attach_textures["color0"];

	forward_pass = PassManager::getInstance().LoadPass("forward_prog.json");
	/*eForward.input_scenename = "scene1";
	eForward.input_cameraname = "main";
	eForward.Init();
	eShowtex.input_texture = eForward.out_color;*/
	eShowtex.Init();
}

void VRPipeline::Render()
{
	SceneManager* scene = SceneContainer::getInstance().get("scene1");

	auto camera = scene->getCamera("main");
	RenderSystem* mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;

	RenderQueue queue;
	scene->getVisibleRenderQueue_as(camera, queue);

	// render to left buffer
	/*forward_pass->mProgram->setProgramConstantData("Projection", &projLeft[0][0], "mat4", sizeof(Matrix4));
	forward_pass->mProgram->setProgramConstantData("ViewMatrix", &viewLeft[0][0], "mat4", sizeof(Matrix4));*/
	GlobalVariablePool* gp = GlobalVariablePool::GetSingletonPtr();
	gp->SetProjectMatrix(projLeft);
	gp->SetViewMatrix(viewLeft);
	mRenderSystem->RenderPass(NULL, queue, forward_pass, left);

	// render to right buffer
	/*forward_pass->mProgram->setProgramConstantData("Projection", &projRight[0][0], "mat4", sizeof(Matrix4));
	forward_pass->mProgram->setProgramConstantData("ViewMatrix", &viewRight[0][0], "mat4", sizeof(Matrix4));*/
	gp->SetProjectMatrix(projRight);
	gp->SetViewMatrix(viewRight);
	mRenderSystem->RenderPass(NULL, queue, forward_pass, right);

	eShowtex.input_texture = colorRight;
	eShowtex.Render();
}

void VRPipeline::GetStereoTex(GLuint& l, GLuint& r)
{
	l = ((EGLTexture*)(colorLeft))->texture_id;
	r = ((EGLTexture*)(colorRight))->texture_id;
}
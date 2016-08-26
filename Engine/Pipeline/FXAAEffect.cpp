#include "FXAAEffect.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"




void FXAAEffect::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	auto& tm = TextureManager::getInstance();
	auto& pm = PassManager::getInstance();

	mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;
	output_rt = rt_mgr.CreateRenderTargetFromPreset("basic", "effect_fxaa");
	output_rt->createInternalRes();
	out_fxaa = tm.get("effect_fxaa_tex1");
	fxaa_pass = pm.LoadPass("fxaa/fxaa_prog.json");
}

void FXAAEffect::Render()
{
	//get a screen quad
	auto quad = GlobalResourceManager::getInstance().as_meshManager.get("ScreenQuad.obj");
	RenderQueueItem item;
	item.asMesh = quad;
	RenderQueue queue2;
	queue2.push_back(item);

	fxaa_pass->mProgram->setProgramConstantData("Tex1", input_color);
	fxaa_pass->mProgram->setSampler("Tex1", SamplerManager::getInstance().get("RepeatLinear"));
	mRenderSystem->RenderPass(NULL, queue2, fxaa_pass, output_rt);
}

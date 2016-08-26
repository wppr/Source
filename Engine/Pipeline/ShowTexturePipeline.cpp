#include "ShowTexturePipeline.h"
#include "RenderTarget.h"
#include "SceneManager.h"
#include "engine_struct.h"
#include "ResourceManager.h"
#include "pass.h"
#include "RenderSystem.h"
#include "TextureManager.h"



void ShowTextureEffect::Init()
{
	auto& rt_mgr = RenderTargetManager::getInstance();
	mainwindow = rt_mgr.get("MainWindow");
	quad_pass = PassManager::getInstance().LoadPass("quad_prog.json");
}

void ShowTextureEffect::Render()
{
	RenderSystem* mRenderSystem = GlobalResourceManager::getInstance().m_RenderSystem;

	//get a screen quad
	auto quad = GlobalResourceManager::getInstance().as_meshManager.get("ScreenQuad.obj");
	RenderQueueItem item;
	item.asMesh = quad;
	RenderQueue queue2;
	queue2.push_back(item);


	if (input_texture!=NULL)
		quad_pass->mProgram->setProgramConstantData("test_tex", input_texture);
	if (input_texture_cube!=NULL)
		quad_pass->mProgram->setProgramConstantData("test_tex_cube", input_texture_cube);
	
	mRenderSystem->RenderPass(NULL, queue2, quad_pass, mainwindow);
}

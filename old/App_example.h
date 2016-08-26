#pragma once
#include "Engine/AppFrameWork.h"
#include "FrameListenerDebug.h"
#include <ctime>
#include "Timer.h"
#include "TextureManager.h"
#include "ResourceManager.h"
#include "pass.h"
#include "SSAOPipeline.h"
#include "ToneMappingPipeline.h"
#include "PBRPipeline.h"
#include "TestPipeline.h"
#include "SSRayTrace.h"
#include "EngineUtil.h"
#include "Image.h"

using namespace HW;
namespace HW
{
	class AppExample :public AppFrameWork
	{
	public:

		void InitScene()
		{ 
			InitDevice();
			CreateScene();
			//设定pipeline
			InitPipeline();

		}

		void InitPipeline()
		{
			auto pipeline = new TestPipeline;
			pipeline->Init();
			sysentry->setPipeline(pipeline);
		}

		void InitDevice()
		{
			Logger::WriteLog("Into  Initialize");
			gp = GlobalVariablePool::GetSingletonPtr();
			gp->initialize();
			Logger::WriteLog("gp  Initialize");
			//shaderMgr = ShaderManager::getSingletonPtr();
			//shaderMgr->initialize(new EGLShaderFactory(), new EGLGpuProgramFactory());
			
			meshMgr = MeshManager::getSingletonPtr();
			//meshMgr->initialize(new EGLTextureFactory);
			
			Logger::WriteLog("wh  Initialize");

			sysentry = SysEntry::getSingletonPtr();
			sysentry->setRenderSystem(render);
			sysentry->setMainWindow(renderwindow);


			
			InitDefaultResources();


		}
		void InitDefaultResources(){

#ifdef ANDROID
			FileUtil::addSceneFilePath("sdcard/OpenGLES/shader/wpp/es3/");
			FileUtil::addSceneFilePath("sdcard/OpenGLES/default_asserts/");
			FileUtil::addSceneFilePath("sdcard/OpenGLES/");
#else
			FileUtil::addSceneFilePath("shader/wpp/es3/");
			FileUtil::addSceneFilePath("default_asserts/");
#endif 
			//file find


		
			// init texture settings;
			auto& tm = TextureManager::getInstance();
			tm.LoadTexturePreset("texture_preset.json");

			// init rendertarget settings;
			RenderTargetManager::getInstance().LoadRenderTargetPreset("RenderTarget_preset.json");
			renderwindow = new EGLRenderWindow("MainWindow", render);
			Logger::WriteLog("Window  Initialize");
			auto& rtm = RenderTargetManager::getInstance();
			rtm.add(renderwindow);

			//get ScreenQuad
			meshMgr->loadMesh_assimp_check("screenquad", "default_asserts/ScreenQuad.obj");
			auto& grm = GlobalResourceManager::getInstance();

			Image* im = new Image;
			im->test();
		}

		void CreateScene(){
			
			//im->LoadImage("3.jpg");
			scene = new OctreeSceneManager("scene1", render);
			SceneContainer::getInstance().add(scene);
			SceneNode* pRoot = scene->GetSceneRoot();
			camera = scene->CreateCamera("main");
			camera->lookAt(Vector3(0, 3, 4), Vector3(0, 0, 0), Vector3(0, -1, 0));
			camera->setDirection(Vector3(0, -1, -1));
			camera->setPerspective(45.0, float(w) / float(h), 1, 100.0);

			camera->lookAt(Vector3(0, 4.5, 15), Vector3(0, 0, 0), Vector3(0, 1, 0));

			camera->lookAt(Vector3(0.69, 2.43, 10.12), Vector3(0, 0, 0), Vector3(0, 1, 0));

			//camera->setDirection(Vector3(-0.07, -0.00, -1.00));
			camera->setDirection(Vector3(-0.06342, -0.05234, -0.99661));

			//scene1 light
			{
				Light * sunLight;
				sunLight = scene->CreateLight("light1", Light::LT_SPOT);
				sunLight->setDirection(Vector3(1.0, -1.0, -1.0));
				sunLight->setPosition(Vector3(-1, 2, 6));
				sunLight->SetMaxDistance(9);
				sunLight->Exponent = 3;
				sunLight->Cutoff = 0.8;
				sunLight->setCastShadow(true);
				sunLight->Diffuse = Vector3(1.0, 1.0, 1.0);
				scene->setActiveLightVector(sunLight, 0);
			}

			float lightsize = 4;
			for (int i = 0; i < lightsize; i++)
			for (int j = 0; j < lightsize; j++)
			{
				int q = i * lightsize + j;
				float a = i / lightsize;
				float b = j / lightsize;
				float c = (rand() % 10000) / 10000.0;

				float offsetx = a * 6 - 3;
				float offsety = b * 6 - 3;
				Light * sunLight;
				stringstream ss;
				ss << "light_" << q;
				string s = ss.str();
				sunLight = scene->CreateLight(s.c_str(), Light::LT_SPOT);
				sunLight->setPosition(Vector3(offsetx, offsety, 0.3));
				sunLight->setDirection(Vector3(0, 0, -1));
				sunLight->SetMaxDistance(0.6);
				sunLight->Exponent = 0.2;
				sunLight->Cutoff = 1.0;
				sunLight->Diffuse = Vector3(0.7*a + 0.1, 0.7*b + 0.1, 0.7*c + 0.1);
				scene->setActiveLightVector(sunLight, q + 2);
			}
			FrameListener * camera_op = new FrameListenerDebug(camera, pRoot, scene, h, w, this, android_touch);
			framelistener_list.push_back(camera_op);

			loadModels();
		}
		void* loadModels(){
			SceneManager* scene = SceneContainer::getInstance().get("scene1");
			SceneNode* root = scene->GetSceneRoot();
			SceneNode* s,*q;
			q = scene->CreateSceneNode("rotatenode");

			//s = LoadMeshtoSceneNode("testscene/scene5/walk.fbx", "nodetest");
			//s->scale(0.04, 0.04, 0.04);
			//s->translate(0, 3, 0);
			//s->rotate(Vector3(1, 0, 0), -0.6);
			//s->rotate(Vector3(0, 0, 1), -3.1);
	
			//s->translate(0, -5, -8);
			//q->attachNode(s);

			//平面+人
			s = LoadMeshtoSceneNode("testscene/scene3/wushi.obj", "wushi2");
			s->rotate(Vector3(1, 0, 0), -1.57);
			q->attachNode(s);


			s = LoadMeshtoSceneNode("testscene/scene3/ground.obj", "ground2");
			s->translate(0, 0, -3);
			s->scale(2, 1, 2);
			q->attachNode(s);
			//   
			//s = LoadMeshtoSceneNode("testscene/scene4/heshi.obj", "heshi");
			//s->translate(-1, 0, 0);
			//q->attachNode(s);

			//s = LoadMeshtoSceneNode("testscene/scene/xsc.obj", "xsc");
			//q->attachNode(s);

			//球
			//s = LoadMeshtoSceneNode("testscene/scene6/mitsuba.obj", "mitsuba");
			//q->attachNode(s);

			//q->rotate(Vector3(1, 0, 0), 1.57);
			root->attachNode(q);


		
			return NULL;
		}
		SceneNode* LoadMeshtoSceneNode(string pfile, string name){
			MeshPtr mesh=meshMgr->loadMesh_assimp_check(name, pfile);

			Entity* entity = scene->CreateEntity(name);
			entity->setMesh(mesh);
			SceneNode* snode = scene->CreateSceneNode(name + "Node");
			snode->attachMovable(entity);
			return snode;
		}
	
	protected:
		void Draw()
		{
			fireFrameListener();
			sysentry->updateMainView();
			calculatedFPS();


#ifdef ANDROID
			android_touch->ClearAction();
#endif
		}

	};
}

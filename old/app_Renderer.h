#pragma once
#include "Engine/AppFrameWork.h"
#include "FrameListenerDebug.h"
#include <ctime>
#include "Timer.h"
#include "TextureManager.h"
#include "ResourceManager.h"
#include "pass.h"
#include "SSAOPipeline.h"
#include "PBRPipeline.h"
#include "TestPipeline.h"
#include "SSRayTrace.h"
#include "EngineUtil.h"
#include "Image.h"
#include "ShadowMapGenerateEffect.h"
#include "RenderSystem.h"
//#include "../../DOctree.h"
#include "Config.h"
#include "DynamicRHI.h"

class app_Renderer {
public:
	void Init() {
		InitDevice();
		InitPipeline();
		CreateScene();
		

	}
	void InitPipeline()
	{
		auto pipeline = new TestPipeline;
		pipeline->Init();
		sysentry->setPipeline(pipeline);
	}

	void Render() {

		fireFrameListener();
		sysentry->updateMainView();
		if (Config::instance().b_logCamera) {
			logCamera();
		}
	}

	void logCamera() {
		auto v = camera->getPosition();
		auto v2 = camera->getDirection();
		Logger::WriteLog("%f %f %f %f %f %f", v.x, v.y, v.z, v2.x, v2.y, v2.z);
	}
	void setCamera(Vector3 pos, int dir) {

		switch (dir)
		{
		case 0:
			camera->setPosition(pos);
			camera->setDirection(Vector3(1, 0, 0));
			camera->setUp(Vector3(0, 1, 0));
			break;
		case 1:
			camera->setPosition(pos);
			camera->setDirection(Vector3(-1, 0, 0));
			camera->setUp(Vector3(0, 1, 0));
			break;
		case 2:
			camera->setPosition(pos);
			camera->setDirection(Vector3(0, 1, 0));
			camera->setUp(Vector3(1, 0, 0));
			break;
		case 3:
			camera->setPosition(pos);
			camera->setDirection(Vector3(0, -1, 0));
			camera->setUp(Vector3(1, 0, 0));
			break;
		case 4:
			camera->setPosition(pos);
			camera->setDirection(Vector3(0, 0, 1));
			camera->setUp(Vector3(0, 1, 0));
			break;
		case 5:
			camera->setPosition(pos);
			camera->setDirection(Vector3(0, 0, -1));
			camera->setUp(Vector3(0, 1, 0));
			break;
		default:
			break;
		}
	}

	BoundingBox scene_bbox;
	BoundingBox getSceneBox() {
		auto ents = scene->getEntityList();

		BoundingBox b;
		bool first = true;
		//// return person
		//auto ent = ents["t1"];
		//b = ent->getMesh()->getModelBoundingBox();
		//b.transformAffine(ent->getParent()->getWorldMatrix());
		////b.setMinimum(Vector3(-100, 10, -160));
		////b.setMaximum(Vector3(230,200,250));
		//return b;


		for (auto x : ents) {
			auto b1 = x.second->getMesh()->getModelBoundingBox();
			b1.transformAffine(x.second->getParent()->getWorldMatrix());
			if (first) {
				b = b1;
			}
			else {
				b.merge(b1);
			}
		}
		BoundingBox t;

		Vector3 ce(142.85881, 5.18029, 541.14221);
		Vector3 hl(150, 120, 150);
		
		auto& config = Config::instance();
		if (config.valid)
		{
			ce = config.ce;
			hl = config.hl;
		}
		t.setMaximum(ce + hl);
		t.setMinimum(ce - hl);
		return t;
	}

	SceneNode* LoadMeshtoSceneNode(string pfile, string name) {
		MeshPtr mesh = meshMgr->loadMesh_assimp_check(name, pfile);

		Entity* entity = scene->CreateEntity(name);
		entity->setMesh(mesh);
		SceneNode* snode = scene->CreateSceneNode(name + "Node");
		snode->attachMovable(entity);
		return snode;
	}

	void CreateScene() {
		scene = new OctreeSceneManager("scene1", render);
		SceneContainer::getInstance().add(scene);
		SceneNode* pRoot = scene->GetSceneRoot();
		camera = scene->CreateCamera("main");
		camera->lookAt(Vector3(0, 3, 4), Vector3(0, 0, 0), Vector3(0, -1, 0));
		camera->setDirection(Vector3(0, -1, -1));

		float fovy = 45;
		if (Config::instance().valid)
			fovy = Config::instance().fov;
		camera->setPerspective(fovy, float(w) / float(h), 1, 30000);
		camera->lookAt(Vector3(0, 4.5, 15), Vector3(0, 0, 0), Vector3(0, 1, 0));
#ifdef ANDROID
#define SMALLSCENE
#endif
#define SMALLSCENE

#ifdef SMALLSCENE
		////small
		camera->setUp(Vector3(0, 1, 0));
		camera->setDirection(Vector3(0.25589, -0.13610, -0.95708));
		camera->setPosition(Vector3(29.70613, -179.28386, 420.57275));
#else

		//people
		camera->lookAt(Vector3(0.69, 2.43, 10.12), Vector3(0, 0, 0), Vector3(0, 1, 0));
		camera->setDirection(Vector3(-0.07, -0.00, -1.00));
		camera->setDirection(Vector3(-0.06342, -0.05234, -0.99661));

		//big scene
		camera->setPosition(Vector3(-74.11176, 0.87027, -51.32387));
		camera->setDirection(Vector3(0.45691, 0.02731, -0.88909));
		camera->setPerspective(45.0, float(w) / float(h), 1, 10000.0);
		camera->setUp(Vector3(0, 1, 0));

#endif
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

		//float lightsize = 4;
		//for (int i = 0; i < lightsize; i++)
		//	for (int j = 0; j < lightsize; j++)
		//	{
		//		int q = i * lightsize + j;
		//		float a = i / lightsize;
		//		float b = j / lightsize;
		//		float c = (rand() % 10000) / 10000.0;

		//		float offsetx = a * 6 - 3;
		//		float offsety = b * 6 - 3;
		//		Light * sunLight;
		//		stringstream ss;
		//		ss << "light_" << q;
		//		string s = ss.str();
		//		sunLight = scene->CreateLight(s.c_str(), Light::LT_SPOT);
		//		sunLight->setPosition(Vector3(offsetx, offsety, 0.3));
		//		sunLight->setDirection(Vector3(0, 0, -1));
		//		sunLight->SetMaxDistance(0.6);
		//		sunLight->Exponent = 0.2;
		//		sunLight->Cutoff = 1.0;
		//		sunLight->Diffuse = Vector3(0.7*a + 0.1, 0.7*b + 0.1, 0.7*c + 0.1);
		//		scene->setActiveLightVector(sunLight, q + 2);
		//	}
		FrameListener * camera_op = new FrameListenerDebug(camera, pRoot, scene, h, w,this);
		framelistener_list.push_back(camera_op);

		loadModels();
	}
	void* loadModels() {
		SceneManager* scene = SceneContainer::getInstance().get("scene1");
		SceneNode* root = scene->GetSceneRoot();
		SceneNode* s, *q;
		q = scene->CreateSceneNode("rotatenode");

		//s = LoadMeshtoSceneNode("testscene/scene5/walk.fbx", "nodetest");
		//s->scale(0.04, 0.04, 0.04);
		//s->translate(0, 3, 0);
		//s->rotate(Vector3(1, 0, 0), -0.6);
		//s->rotate(Vector3(0, 0, 1), -3.1);

		//s->translate(0, -5, -8);
		//q->attachNode(s);
		//s = LoadMeshtoSceneNode("testscene/scene4/heshi.obj", "heshi");
		//s->translate(-1, 0, 0);
		//q->attachNode(s);

#ifdef SMALLSCENE
		////small
		//s = LoadMeshtoSceneNode("testscene/scene7/r1.obj", "t1");
		////s->rotate(Vector3(1, 0, 0), -1.57);
		//s->translate(-Vector3(-6.52319, 285.12286, -101.13889));
		//q->attachNode(s);

		//s = LoadMeshtoSceneNode("testscene/scene8/c5.obj", "t1");
		//s->translate(-Vector3(55000, 700, 12000));
		//q->attachNode(s);
		//camera->setPerspective(45.0, float(w) / float(h), 1, 30000.0);
		//camera->lookAt(Vector3(108.21774, 109.15522, 1072.72168), Vector3(0, 0, 0), Vector3(0, 1, 0));
		//camera->setDirection(Vector3(0.06436, -0.20374, -0.97691));

		if (Config::instance().valid) {
			camera->setPosition(Config::instance().camerapos);
			camera->setDirection(Config::instance().cameradir);
		}
		////debug
		//camera->setDirection(Vector3(0.72647, 0.04961, -0.68541));
		//camera->setPosition(Vector3(179.37582, 80.76276, 482.65256));
		//
		s = LoadMeshtoSceneNode("testscene/scene6/mitsuba.obj", "mitsuba");
		q->attachNode(s);
		camera->setDirection(Vector3(0, -1, -1));
		camera->setPerspective(45.0, float(w) / float(h), 1, 1000);
		camera->lookAt(Vector3(0, 4.5, 15), Vector3(0, 0, 0), Vector3(0, 1, 0));

		//s = LoadMeshtoSceneNode("testscene/scene8/31b.obj", "j2");
		//q->attachNode(s);
		//camera->setDirection(Vector3(0, -1, -1));
		//camera->setPerspective(45.0, float(w) / float(h), 1, 10000);
		//camera->lookAt(Vector3(0, 4.5, 15), Vector3(0, 0, 0), Vector3(0, 1, 0));

		//s = LoadMeshtoSceneNode("testscene/scene9/15y2.obj", "shou1");
		//q->attachNode(s);
		//s = LoadMeshtoSceneNode("testscene/scene9/15t2.obj", "shou2");
		//q->attachNode(s);
		//camera->setDirection(Vector3(0, -1, -1));
		//camera->setPerspective(45.0, float(w) / float(h), 1, 100000);
		//camera->lookAt(Vector3(0, 4.5, 15), Vector3(0, 0, 0), Vector3(0, 1, 0));
#else
		//Æ½Ãæ+ÈË
		//s = LoadMeshtoSceneNode("testscene/scene3/wushi.obj", "t1");
		//s->rotate(Vector3(1, 0, 0), -1.57);

		//q->attachNode(s);


		//s = LoadMeshtoSceneNode("testscene/scene3/ground.obj", "ground2");
		//s->translate(0, 0, -3);
		//s->scale(2, 1, 2);
		//q->attachNode(s);
		//   
		//Çò
		//s = LoadMeshtoSceneNode("testscene/scene6/mitsuba.obj", "mitsuba");
		//q->attachNode(s);
		//camera->setDirection(Vector3(0, -1, -1));
		//camera->setPerspective(45.0, float(w) / float(h), 1, 1000);
		//camera->lookAt(Vector3(0, 4.5, 15), Vector3(0, 0, 0), Vector3(0, 1, 0));
		//big
		s = LoadMeshtoSceneNode("testscene/scene7/o3.obj", "t1");
		s->rotate(Vector3(1, 0, 0), -1.57);
		s->translate(-Vector3(-805.15277, 1358.37476, -789.63068));
		q->attachNode(s);
#endif


		//q->rotate(Vector3(1, 0, 0), 1.57);
		root->attachNode(q);
		scene_bbox = getSceneBox();
		//if(Config::instance().b_drawbox>0)
		//	DrawOctree();
		//setCamera(Vector3(0, 0, 0), 1);
		return NULL;
	}
	//void DrawOctree() {
	//	ifstream tree("Octree.txt");
	//	DOctree d;
	//	if (tree.is_open()) {
	//		tree >> d;
	//		d.Traverse([this](const auto &node) {
	//			DrawBox(node);
	//		});
	//	}
	//}
	//void DrawBox(DOctree::OctNode d) {
	//	BBox b = d.bbox;
	//	static int num = 0;
	//	num++;
	//	SceneManager* scene = SceneContainer::getInstance().get("scene1");
	//	SceneNode* root = scene->GetSceneRoot();
	//	SceneNode* s;
	//	Vector3 ce = Vector3(b.Center().x, b.Center().y, b.Center().z);
	//	auto hl = b.Center() - b.Min();

	//	stringstream ss;
	//	ss << "wirecube_" << num;
	//	//if(num<10)
	//		s = LoadMeshtoSceneNode("wirecube.obj", ss.str());
	//	//else
	//	//	s = LoadMeshtoSceneNode("wirecube2.obj", ss.str());
	//	s->scale(hl.x / 10, hl.y / 10, hl.z / 10);
	//	s->translate(ce);
	//	root->attachNode(s);

	//}
	void InitDevice()
	{
		Logger::WriteLog("Into  Initialize");
		gp = GlobalVariablePool::GetSingletonPtr();
		gp->initialize();
		Logger::WriteLog("gp  Initialize");

		meshMgr = MeshManager::getSingletonPtr();
		//meshMgr->initialize(new EGLTextureFactory);

		Logger::WriteLog("wh  Initialize");

		sysentry = SysEntry::getSingletonPtr();
		sysentry->setRenderSystem(render);
		sysentry->setMainWindow(renderwindow);
		
		InitDefaultResources();
	}

	void InitDefaultResources() {

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

	}
	void fireFrameListener()
	{
		for (std::vector<FrameListener *>::iterator it = framelistener_list.begin(); it != framelistener_list.end(); it++)
		{
			(*it)->frameStarted();
		}
	}
	void unloadFrameListener()
	{
		for (std::vector<FrameListener *>::iterator it = framelistener_list.begin(); it != framelistener_list.end(); it++)
		{
			if ((*it) != NULL)
				(*it)->frameEnded();
			else
				Logger::WriteLog("FrameListener NuLL");
		}
	}

	void takeSnapShot(int& _w, int& _h, vector<uint8_t>& v) {

		Render();
		_w = w;
		_h = h;
		v.resize(w*h * 4);
		glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, &v[0]);
		//printf("read pixel  %d", glGetError());
	}

public:
	bool shutdown = false;
	GlobalVariablePool * gp;
	RenderSystem * render;
//	AndroidTouch * android_touch;
	SceneManager * scene;
	MeshManager * meshMgr;
	Camera * camera;
	SysEntry * sysentry;
	RenderTarget * renderwindow;
	std::vector<FrameListener *> framelistener_list;
	Timer timer;
	int w=0;
	int h=0;
};
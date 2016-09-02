#pragma once
#include "EdushiPipeline.h"
#include "AppFrameWork2.h"
#include "CarSimulation.h"
#include "InteractionControler.h"
#include "GUIWrapper.h"
#include "socketSceneLoader\SceneLoader.h"

using namespace Block;
class App_edushi :public App {
public:
	CarSimulation CarSim;
	bool IsLogCamera = false;
	float scenescale = 0.16;
	Vector3 sceneTranlate = Vector3(-2.5, 0, -1.6);
	Vector3 modelTranslate = Vector3(0.0);
	float modelscale = 1.0;
	float modelrotate = 0;
	SceneLoader* sl;

	void Init() {
		w = 1000;
		h = 600;
		render = new EGLRenderSystem;
		render->SetWandH(w, h);
		render->Initialize();
		InitDevice();
		InitPipeline();
		CreateScene();

		//sceneLoader
		sl = new SceneLoader(scene, meshMgr, 32, 16);
		sl->loadMesh();
		//sl->InitServer("127.0.0.1", "56025");
		sl->LoadJson();
		sl->UpdateScene();
	}
	Vector3 getSceneCenter() {
		int w = 32, h = 16;
		Vector3 c = 0.5*Vector3(w, 0, h)*scenescale + sceneTranlate;
		return c;
	}
	void InitPipeline()
	{
		pipeline = new EdushiPipeline;
		pipeline->Init();
	}

	void Render() {
		UpdateGUI();
		//sl->UpdateScene();
		pipeline->Render();
		//CarSim.Update();
	}

	void LoadModel(string path, string name = "") {
		SceneManager* scene = SceneContainer::getInstance().get("scene1");
		SceneNode* root = scene->GetSceneRoot();
		SceneNode* s;
		if (name == "") name = path;
		s = LoadMeshtoSceneNode(path, name);
		if (s == NULL) {
			cout << "cant find model:" << path << endl;
			return;
		}
		s->setScale(Vector3(modelscale));
		s->setTranslation(modelTranslate);
		Quaternion q;
		float rad = modelrotate*PI / 180;
		q.fromAngleAxis(rad, Vector3(0, 1, 0));
		s->setOrientation(q);
		if (s != NULL)
			root->attachNode(s);
	}
	void RemoveAllNodes() {
		SceneManager* scene = SceneContainer::getInstance().get("scene1");
		SceneNode* root = scene->GetSceneRoot();
		root->detachAllNodes();
		auto nodes=root->getAllChildNodes();
		//for (auto& node : nodes) {
		//	root->detachNode(node);
		//	string nodename = node->getName();
		//	auto entity = scene->getEntity(nodename);
		//	if (node != NULL)
		//		scene->destroy(node);
		//	if (entity != NULL)
		//		scene->destroy(entity);
		//}

		
	}

	void UpdateGUI() {
		ImGui_ImplGlfwGL3_NewFrame();
		{ 
			ImGui::Begin("Params");
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			auto p =(EdushiPipeline*) pipeline;
			ImGui::TextWrapped(p->profile.ToString().c_str());
			ImGui::Text("Model Path");
			static char file[100]="model/uniform/building01.obj";
			static char name[100] = "node_b0_0";
			
			ImGui::InputText("Model Path", file, 100);
			ImGui::InputText("SceneNode Name", name, 100);
			ImGui::DragFloat3("model translate", &modelTranslate[0], 0.01);
			ImGui::DragFloat("model scale", &modelscale, 0.01);
			ImGui::DragFloat("model rotate degree", &modelrotate, 0.01);
			string tmp(file);
			string tmp2(name);
			if (ImGui::Button("Load model")) LoadModel(tmp,name);
			if (ImGui::Button("RemoveAll")) RemoveAllNodes();
			if (ImGui::Button("AttachFloor")) sl->AttachFloar();
			if (ImGui::Button("Switch Show Json")) sl->show_json = !sl->show_json;
			static char blockpath[100] = "model/uniform/data/block0.txt";
			ImGui::InputText("Block Path", blockpath, 100);
			static char blockname[100] = "1";
			ImGui::InputText("Block Name", blockname, 100);
			static char blocksize[100] = "1x1";
			ImGui::InputText("Block Size", blocksize, 100);
			static char blocktype[100] = "block";
			ImGui::InputText("Block type", blocktype, 100);
			static int blockorien = 1;
			ImGui::InputInt("Block Orientation", &blockorien);
			if (ImGui::Button("Load Block Preset"))
			{
				string bname=sl->bh.LoadBlockPreset(blockpath);
				auto& b = sl->bh.blockPresets[bname];
				b.name.copy(blockname, b.name.size()+1);
				b.size.copy(blocksize, b.size.size() + 1);
				b.type.copy(blocktype, b.type.size() + 1);
				blockorien = b.orientation;
			}
			if (ImGui::Button("Save Block Preset")) sl->bh.SaveBlock(blockpath, blockname, blocksize,blocktype,blockorien);
			static char nodename[100] = "node_b0_";
			static Vector3 blockPos(0.0);
			ImGui::DragFloat3("Block Translate", &blockPos[0], 0.01);
			ImGui::InputText("Block Node Name", nodename, 100);
			if (ImGui::Button("Attach Block"))
			{
				sl->bh.AttachBlock(blockname, string(nodename), blockPos);
			}
			if (ImGui::Button("Attach All Block"))
			{
				sl->bh.PutblockTest();
			}
			if (ImGui::Button("Attach All Mesh"))
			{
				sl->bh.attachAllMesh();
			}
			ImGui::DragFloat("scene scale", &scenescale, 0.001);
			ImGui::DragFloat3("scene translate", &sceneTranlate[0], 0.01);
			{
				SceneNode* root = scene->GetSceneRoot();
				root->setScale(Vector3(scenescale));
				root->setTranslation(sceneTranlate);
			}
			ImGui::Text("Shading");
			ImGui::InputFloat3("CameraPos", &camera->m_Position[0]);
			ImGui::ColorEdit3("LightColor0", &p->LightColor[0][0]);
			ImGui::ColorEdit3("LightColor1", &p->LightColor[1][0]);
			ImGui::DragFloat3("LightPos0", &p->LightPos[0][0]);
			ImGui::DragFloat3("LightPos1", &p->LightPos[1][0]);
			ImGui::DragFloat("Metalness", &p->metalness, 0.002, 0.0001, 1.0);
			ImGui::DragFloat("Roughness", &p->roughness, 0.002, 0.0001, 1.0);
			const char* ModeDesc[] = { "direct light+Env",
				"direct light+Env+ao",
				"ao",
				"Env",
				"Direct light",
				"Normal",
				"ShadowMap",
				"Shadow Factor",
				"Shaodw+Direct"};
			int modeNum = 9;
			
			ImGui::Combo("Shading Mode", &p->mode, ModeDesc, modeNum);
			ImGui::Combo("EnvMap", &p->envMapId, "pisa\0uffizi\0sky\0");
			ImGui::DragFloat("SSAO Radius", &p->ssao.radius,0.002,0.0001,1.0);
			ImGui::DragFloat("ToneMap exposure", &p->tonemap.exposure, 0.01, 0.1, 10.0);
			static bool f=true;
			ImGui::Checkbox("Tonemap ON", &f); p->tonemap.ToneMapOn = f?1:0;
			ImGui::Text("ShadowMap");
			static bool f2 = false;
			//ImGui::Checkbox("LinearDepth", &f2); p->shadowmap.LinearDepth = f2 ? 1 : 0;
			ImGui::DragFloat("ReduceBleeding", &p->shadowmap.ReduceBleeding, 0.002, 0.0001, 1.0);
			ImGui::DragFloat("BlurRadius", &p->shadowmap.blureffect.radius, 0.002, 0.0001, 5.0);
			ImGui::DragFloat("bias", &p->LightCamera.bias, 0.001, -0.1, 0.1);
			ImGui::DragFloat("near", &p->LightCamera.Near, 0.002, 0.0001);
			ImGui::DragFloat("far", &p->LightCamera.Far, 0.002, 0.0001);
			ImGui::DragFloat("width", &p->LightCamera.width, 0.002, 0.0001);
			ImGui::DragFloat("height", &p->LightCamera.height, 0.002, 0.0001);
			p->shadowmap.SetLightCamera(0, p->LightCamera);
			ImGui::End();

		}

	}
	void CreateScene() {
		scene = new OctreeSceneManager("scene1", render);
		SceneContainer::getInstance().add(scene);
		SceneNode* pRoot = scene->GetSceneRoot();
		camera = scene->CreateCamera("main");
		camera->lookAt(Vector3(-1.8, 0.6, 0.4), Vector3(-2, 0, -2), Vector3(0, 1, 0));
		float fovy = 45;
		camera->setPerspective(fovy, float(w) / float(h), 0.01, 200);

		loadModels();
		//sl->bh.PutblockTest();
		//CarSim.Init(this);
	}

	void loadModels() {
		SceneManager* scene = SceneContainer::getInstance().get("scene1");
		SceneNode* root = scene->GetSceneRoot();
		SceneNode* s;

		//s = LoadMeshtoSceneNode("model/edushi/exportcity/city.obj", "city");
		//s->scale(0.01, 0.01, 0.01);
		//root->attachNode(s);

		//s = LoadMeshtoSceneNode("model/edushi/city2.obj", "city");
		//s->scale(0.01, 0.01, 0.01);
		//root->attachNode(s);
	}

};
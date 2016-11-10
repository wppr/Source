#pragma once
#include "EdushiVRPipeline.h"
#include "AppFrameWork2.h"
#include "CarSimulation.h"
#include "InteractionControler.h"
#include "GUIWrapper.h"
#include "socketSceneLoader\SceneLoader.h"
#include "Interpolation.h"
#include "pattern.h"
#include <openvr.h>

using namespace Block;
struct EdushiConfig {
	Vector3 cameraPos;
	Vector3 cameraUp;
	Vector3 LookCenter;

	float LookDownAngle = 2.7;
	float UpYAngle = 0;
	float cameraCenterDist = 10;
	float cameraRoundAngle = 30;

	//int w = 1024;
	//int h = 768;
	float fov = 25;

	int mapw = 32;
	int maph = 16;

	string ip;
	string port;

	META(N(cameraPos), N(cameraUp), N(LookCenter), N(LookDownAngle), N(UpYAngle),
		N(cameraCenterDist), N(ip), N(port), N(cameraRoundAngle), N(fov), N(mapw), N(maph));
};

class App_edushi_VR :public App {
public:
	CarSimulation CarSim;
	bool IsLogCamera = false;
	float scenescale = 0.16;
	Vector3 sceneTranlate = Vector3(-2.5, 0, -1.6);
	Vector3 modelTranslate = Vector3(0.0);
	float modelscale = 1.0;
	float modelrotate = 0;
	SceneLoader* sl = NULL;

	EdushiConfig c, originConfig;

	//track
	//vector<Vector3> marks;
	//Interpolation interp;
	//bool free = false;
	//float speed = 0.02;
	//float acceleration = 0;
	//float t = 0;

	//pattern
	enum MatrixSource {
		VR,
		CAMERA
	};
	MatrixSource matrixSource = VR;
	CameraController cameraController;
	vector<InterpData> marks;
	Pattern pattern;
	double duration = 0;

	float rotatespeed = 0;
	bool rotateCamera = false;
	bool updateCamera = false;
	void SetCamera1() {
		c.LookDownAngle = 0;
		c.cameraCenterDist = 7;
		c.cameraRoundAngle = 270;
		c.LookCenter[1] = 0.72;
	}
	void SetCamera2() {
		c.LookDownAngle = 2.7;
		c.cameraCenterDist = 10;
		c.cameraRoundAngle = 180;
		c.LookCenter[1] = 1.80;
	}
	void SetCamera4() {
		c.LookDownAngle = 2.7;
		c.cameraCenterDist = 10;
		c.cameraRoundAngle = 0;
		c.LookCenter[1] = 1.82;
	}
	//void SetCamera3_2() {
	//	c.LookDownAngle = 1.7;
	//	c.cameraCenterDist = 7;
	//	c.cameraRoundAngle = 90;
	//	c.LookCenter[1] = 0.81;
	//}
	void SetCamera3() {
		c.LookDownAngle = 0;
		c.cameraCenterDist = 7;
		c.cameraRoundAngle = 90;
		c.LookCenter[1] = 1.5;
	}

	void Init() {
		w = 1024;
		h = 768;
		render = new EGLRenderSystem;
		render->SetWandH(w, h);
		render->Initialize();
		//VRInit();
		InitDevice();
		InitPipeline();
		CreateScene();
		LoadConfig();
		//sceneLoader
		sl = new SceneLoader(scene, meshMgr, 32, 16, LEFT, RIGHT, TOP, BOTTOM);
		sl->loadMesh();
		sl->InitClient(c.ip, c.port);

		sl->LoadJson();
		sl->show_json = true;
		
		//CarSim.Init(this);
		vector<Vector3> pos;
		vector<Quaternion> orients;
		//sl->GetRandomCars(pos,orients);
		//CarSim.GenRandomCars(pos, orients);

		//SetCamera1();
		//c.cameraPos = camera->getPosition();
		//c.cameraUp == camera->getUp();
		//c.LookCenter = getSceneCenter(c.mapw, c.maph);
		UpdateCamera(c);
		vrScene = "room";
	/*	vrScene = "room";
		LoadMarks(vrScene + ".txt");
		SetInterp();*/

		if ("room" == vrScene)
			sl->AttachRoom();
		else
			sl->UpdateScene(AbsolutTime);
	}
	Vector3 getSceneCenter(int w, int h) {
		Vector3 cen = 0.5*Vector3(w, 0, h)*scenescale + sceneTranlate;
		return cen;
	}
	void UpdateCamera(EdushiConfig& c) {
		auto cen = getSceneCenter(c.mapw, c.maph);
		cen.y = c.LookCenter[1];

		float radRound = c.cameraRoundAngle / 180 * PI;
		float radUp = c.LookDownAngle / 180 * PI;
		float h = c.cameraCenterDist*sin(radUp);
		Vector3 offset = Vector3(c.cameraCenterDist*sin(radRound), h, c.cameraCenterDist*cos(radRound));
		float rady = c.UpYAngle / 180 * PI;
		Quaternion q(rady, -offset);
		Vector3 up2 = q*Vector3(0, 1, 0);
		camera->lookAt(cen + offset, cen, up2);

		c.cameraPos = cen + offset;
		c.LookCenter = cen;
		c.cameraUp = up2;

	}
	//void SetCamera(float angleUp, float angleRound) {
	//	auto cen = getSceneCenter(32,16);
	//	cen.y += c.LookCenter[1];
	//	float radRound = angleRound/180* PI;
	//	float radUp = angleUp / 180 * PI;
	//	float h = c.cameraCenterDist*sin(radUp);
	//	Vector3 offset = Vector3(c.cameraCenterDist*sin(radRound), h, c.cameraCenterDist*cos(radRound));
	//	camera->lookAt(cen + offset, cen, Vector3(0, 1, 0));
	//	//camera->setPerspective(fovy, float(w) / float(h), 0.01, 200);
	//}
	bool firstRotate = true;
	void RotateCamera() {
		static float lasttime;
		if (firstRotate)
		{
			lasttime = AbsolutTime;
			firstRotate = !firstRotate;
		}
		float dura = AbsolutTime - lasttime;
		lasttime = AbsolutTime;
		c.cameraRoundAngle += 20 * dura;
		if (c.cameraRoundAngle > 360.0f)
			c.cameraRoundAngle -= 360.0f;
		UpdateCamera(c);
	}

	void InitPipeline()
	{
		pipeline = new EdushiVRPipeline;
		pipeline->Init();
		sysentry->setPipeline(pipeline);
	}

	void UpdateDuration()
	{
		static bool first = true;
		static float lasttime;
		if (first)
		{
			lasttime = AbsolutTime;
			first = false;
		}
		duration = AbsolutTime - lasttime;
		lasttime = AbsolutTime;
	}

	void Render() {
		UpdateGUI();
		UpdateDuration();
		//sl->UpdateScene(AbsolutTime);
		UpdatePorjection();

		double beforeRender = glfwGetTime();
		pipeline->Render();
		double afterRender = glfwGetTime();

		//VRSubmitTexture();
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
		auto nodes = root->getAllChildNodes();

	}
	void LoadConfig() {
		serializeLoad("EdushiConfig.txt", c);
		originConfig = c;
	}
	void SaveConfig() {
		serializeSave("EdushiConfig.txt", c);
	}
	void UpdateGUI() {
		ImGui_ImplGlfwGL3_NewFrame();
		{
			ImGui::Begin("Params");
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			auto p = (EdushiVRPipeline*)pipeline;
			ImGui::TextWrapped(p->profile.ToString().c_str());
			static bool show_load_model = false;
			if (ImGui::Button("Show Model Window")) show_load_model = !show_load_model;
			if (show_load_model) {
				ImGui::Begin("model");
				ImGui::Text("Model Path");
				static char file[100] = "model/uniform/building01.obj";
				//static char file[100] = "../../../daochu/b1.obj";
				static char name[100] = "node_b0_0";

				ImGui::InputText("Model Path", file, 100);
				ImGui::InputText("SceneNode Name", name, 100);
				ImGui::DragFloat3("model translate", &modelTranslate[0], 0.01);
				ImGui::DragFloat("model scale", &modelscale, 0.01);
				ImGui::DragFloat("model rotate degree", &modelrotate, 0.01);
				string tmp(file);
				string tmp2(name);
				if (ImGui::Button("Load model")) LoadModel(tmp, name);

				if (ImGui::Button("RemoveAll")) RemoveAllNodes(); ImGui::SameLine();
				if (ImGui::Button("AttachFloor")) sl->AttachFloar();
				if (ImGui::Button("Switch Show Json")) sl->show_json = !sl->show_json;
				ImGui::End();
			}
			static bool show_config = true;
			if (ImGui::Button("Show Camera Config")) show_config = !show_config;
			if (show_config) {
				ImGui::Begin("config");
				ImGui::DragFloat("scene scale", &scenescale, 0.001);
				ImGui::DragFloat3("scene translate", &sceneTranlate[0], 0.01);
				static bool scene2x = false;
				ImGui::Checkbox("2x Scene", &scene2x);
				{
					SceneNode* root = scene->GetSceneRoot();
					int x = 1;
					if (scene2x) x = 2;
					root->setScale(Vector3(scenescale)*x);
					root->setTranslation(sceneTranlate);
				}
				ImGui::Text("Camera");
				c.cameraPos = camera->getPosition();
				c.cameraUp = camera->getUp();
				ImGui::DragFloat3("CameraPos", &c.cameraPos[0], 0.01);
				ImGui::DragFloat3("CameraCenter", &c.LookCenter[0], 0.01);
				ImGui::DragFloat3("CameraUp", &c.cameraUp[0], 0.01);


				ImGui::DragFloat("LookCenterY", &c.LookCenter[1], 0.01);
				ImGui::DragFloat("LookDownAngle", &c.LookDownAngle, 0.1, 0, 89);
				ImGui::DragFloat("UpYAngle", &c.UpYAngle, 0.1, -89, 89);
				ImGui::DragFloat("RoundAngle", &c.cameraRoundAngle, 0.2, 0, 360);

				ImGui::DragFloat("cameraCenterDist", &c.cameraCenterDist, 0.1);
				ImGui::DragFloat("cameraRotateSpeed", &rotatespeed);
				ImGui::DragFloat("cameraFov", &c.fov, 0.1);

				static bool updatecamera = false;
				ImGui::Checkbox("Use param Camera", &updatecamera);
				if (updatecamera) UpdateCamera(c);

				ImGui::SameLine();
				if (ImGui::Button("Next Camera")) {
					static int i = 0;
					i = (i + 1) % 4;
					if (i == 0) SetCamera4();
					if (i == 1) SetCamera3();
					if (i == 2) SetCamera2();
					if (i == 3) SetCamera1();
					//cameraRoundAngle = (floor((cameraRoundAngle) / 90) + 1) * 90;
					//if (cameraRoundAngle >= 360) cameraRoundAngle -= 360;
					UpdateCamera(c);
				}
				ImGui::SameLine();
				ImGui::Checkbox("Start Rotate Camera", &rotateCamera);
				if (ImGui::Button("Save Config")) SaveConfig();
				ImGui::End();
			}

			static bool show_block_window = false;
			if (ImGui::Button("Show Block Window")) show_block_window = !show_block_window;
			if (show_block_window) {
				ImGui::Begin("");
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
					string bname = sl->bh.LoadBlockPreset(blockpath);
					auto& b = sl->bh.blockPresets[bname];
					b.name.copy(blockname, b.name.size() + 1);
					b.size.copy(blocksize, b.size.size() + 1);
					b.type.copy(blocktype, b.type.size() + 1);
					blockorien = b.orientation;
				}
				ImGui::SameLine();
				if (ImGui::Button("Save Block Preset")) sl->bh.SaveBlock(blockpath, blockname, blocksize, blocktype, blockorien);
				static char nodename[100] = "node_b0_";
				static Vector3 blockPos(0.0);
				ImGui::DragFloat3("Block Translate", &blockPos[0], 0.01);
				ImGui::InputText("Block Node Name", nodename, 100);
				if (ImGui::Button("Attach Block"))
				{
					sl->bh.AttachBlock(blockname, string(nodename), blockPos);
				}ImGui::SameLine();
				if (ImGui::Button("Attach All Block"))
				{
					sl->bh.attachAllBlock();
				}ImGui::SameLine();
				if (ImGui::Button("Attach All Mesh"))
				{
					sl->bh.attachAllMesh();
				}
				ImGui::End();
			}

			static bool show_shading = false;
			if (ImGui::Button("Show Shading")) show_shading = !show_shading;
			if (show_shading) {
				ImGui::Begin("shading");
				ImGui::Text("Shading");

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
					"Shaodw+Direct" };
				int modeNum = 9;

				ImGui::Combo("Shading Mode", &p->mode, ModeDesc, modeNum);
				ImGui::Combo("EnvMap", &p->envMapId, "pisa\0uffizi\0sky\0");
				ImGui::DragFloat("SSAO Radius", &p->ssao_left.radius, 0.002, 0.0001, 1.0);
				ImGui::DragFloat("ToneMap exposure", &p->tonemap_left.exposure, 0.01, 0.1, 10.0);
				static bool f = true;
				ImGui::Checkbox("Tonemap ON", &f); p->tonemap_left.ToneMapOn = f ? 1 : 0;
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

			static bool show_record_window = false;
			if (ImGui::Button("Show Record Pattern Window")) show_record_window = !show_record_window;
			if (show_record_window)
			{
				matrixSource = CAMERA;
				if (ImGui::TreeNode("CameraControl"))
				{
					ImGui::DragFloat("pitch", &cameraController.PitchSpeed, 0.01);
					ImGui::DragFloat("yaw", &cameraController.YawSpeed, 0.01);
					ImGui::DragFloat("linear", &cameraController.LinearSpeed, 0.01);
					ImGui::DragFloat("acceleration", &cameraController.Acceleration, 0.01);
					static bool enable = false;
					if (ImGui::Button("enable/disable")) enable = !enable;
					if (enable)
						cameraController.UpdateCamera(camera, duration);
					ImGui::TreePop();
				}
				if (ImGui::TreeNode("RecordOperations"))
				{
					if (ImGui::Button("Pop"))
					{
						if (!marks.empty())
							marks.pop_back();
					}
					ImGui::SameLine();
					if (ImGui::Button("Push"))
					{
						Vector3 pos = camera->getPosition();
						Vector3 dir = camera->getDirection();
						marks.push_back(InterpData(AbsolutTime, pos, dir));
					}
					ImGui::SameLine();
					if (ImGui::Button("Clear"))
						marks.clear();
					ImGui::TreePop();
					//display
					for (auto &m : marks)
					{
						ImGui::PushItemWidth(80);
						ImGui::InputFloat("", &m.Time);
						ImGui::PopItemWidth();
						ImGui::SameLine();
						ImGui::PushItemWidth(150);
						ImGui::InputFloat3("", &m.Position[0]);
						ImGui::PopItemWidth();
						ImGui::SameLine();
						ImGui::PushItemWidth(150);
						ImGui::InputFloat3("", &m.Direction[0]);
						ImGui::PopItemWidth();
					}
					//save marks
					static char filename[50] = "";
					ImGui::InputText("File Name", filename, 50);
					if (ImGui::Button("Save"))
						SaveMarks("..\\WorkSpace\\Patterns\\"+string(filename), marks);
				}
			}
			else matrixSource = VR;

			static bool show_pattern_window = false;
			if (ImGui::Button("Show Play Pattern Window")) show_pattern_window = !show_pattern_window;
			if (show_pattern_window)
			{
				static double timer = 0;
				matrixSource = CAMERA;
				LoadMarks("..\\WorkSpace\\Patterns\\new_default.txt", marks);
				//display
				for (auto &m : marks)
				{
					ImGui::PushItemWidth(80);
					ImGui::InputFloat("", &m.Time);
					ImGui::PopItemWidth();
					ImGui::SameLine();
					ImGui::PushItemWidth(150);
					ImGui::InputFloat3("", &m.Position[0]);
					ImGui::PopItemWidth();
					ImGui::SameLine();
					ImGui::PushItemWidth(150);
					ImGui::InputFloat3("", &m.Direction[0]);
					ImGui::PopItemWidth();
				}
				//load marks
				static char filename[50] = "";
				ImGui::InputText("File Name", filename, 50);
				if (ImGui::Button("Load"))
				{
					LoadMarks("..\\WorkSpace\\Patterns\\" + string(filename), marks);
					pattern.setMarks(marks);
				}
				ImGui::SameLine();
				static bool pause = true;
				if (ImGui::Button("Resume/Pause")) pause = !pause;
				if (!pause)
				{
					timer += duration;
					camera->setPosition(pattern.getPosition(timer));
					camera->setDirection(pattern.getDirection(timer));
				}
				ImGui::SameLine();
				if (ImGui::Button("Reset"))
					timer = 0;
			}
			else matrixSource = VR;

			ImGui::End();
		}

	}
	void CreateScene() {
		scene = new OctreeSceneManager("scene1", render);
		SceneContainer::getInstance().add(scene);
		SceneNode* pRoot = scene->GetSceneRoot();
		camera = scene->CreateCamera("main");
		camera->lookAt(Vector3(-1.8, 0.6, 0.4), Vector3(-2, 0, -2), Vector3(0, 1, 0));

		camera->setPerspective(c.fov, float(w) / float(h), 0.01, 200);
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

	//***************************************************************************************************************************************
	//***************************************************************************************************************************************
	//***************************************************************************************************************************************
	//***************************************************************************************************************************************

	void VRInit()
	{
		//******************************************************************************
		// openvr init
		//******************************************************************************
		//Loading the SteamVR Runtime
		bool success = true;
		vr::EVRInitError eError = vr::VRInitError_None;
		m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);

		if (eError != vr::VRInitError_None)
		{
			m_pHMD = NULL;
			char buf[1024];
			sprintf_s(buf, sizeof(buf), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
			success = false;
		}

		m_pRenderModels = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &eError);
		if (!m_pRenderModels)
		{
			m_pHMD = NULL;
			vr::VR_Shutdown();

			char buf[1024];
			sprintf_s(buf, sizeof(buf), "Unable to get render model interface: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
			success = false;
		}

		//init compositor
		vr::EVRInitError peError = vr::VRInitError_None;

		if (!vr::VRCompositor())
		{
			printf("Compositor initialization failed. See log file for details\n");
			success = false;
		}

		nearClip = 0.1f;
		farClip = 10000.0f;
		m_pHMD->GetRecommendedRenderTargetSize(&renderWidth, &renderHeight);

		//init not success, shut down
		if (!success)
			if (m_pHMD)
			{
				vr::VR_Shutdown();
				m_pHMD = NULL;
			}

		////init render
		//render = new EGLRenderSystem;
		//render->SetWandH(renderWidth, renderHeight);
		//this->w = renderWidth;
		//this->h = renderHeight;
		//render->Initialize();

		//init projection mat and transition matrix between eyes and HMD
		SetupCameras();
	}

	void SetupCameras()
	{
		projLeft = GetHMDMatrixProjectionEye(vr::Eye_Left);
		projRight = GetHMDMatrixProjectionEye(vr::Eye_Right);
		eyePosLeft = GetHMDMatrixPoseEye(vr::Eye_Left);
		eyePosRight = GetHMDMatrixPoseEye(vr::Eye_Right);
	}

	Matrix4 GetHMDMatrixProjectionEye(vr::Hmd_Eye nEye)
	{
		if (!m_pHMD)
			return Matrix4();

		vr::HmdMatrix44_t mat = m_pHMD->GetProjectionMatrix(nEye, nearClip, farClip, vr::API_OpenGL);

		return Matrix4(
			mat.m[0][0], mat.m[0][1], mat.m[0][2], mat.m[0][3],
			mat.m[1][0], mat.m[1][1], mat.m[1][2], mat.m[1][3],
			mat.m[2][0], mat.m[2][1], mat.m[2][2], mat.m[2][3],
			mat.m[3][0], mat.m[3][1], mat.m[3][2], mat.m[3][3]
		);
	}

	Matrix4 GetHMDMatrixPoseEye(vr::Hmd_Eye nEye)
	{
		if (!m_pHMD)
			return Matrix4();

		vr::HmdMatrix34_t matEyeRight = m_pHMD->GetEyeToHeadTransform(nEye);
		Matrix4 matrixObj(
			matEyeRight.m[0][0], matEyeRight.m[0][1], matEyeRight.m[0][2], matEyeRight.m[0][3],
			matEyeRight.m[1][0], matEyeRight.m[1][1], matEyeRight.m[1][2], matEyeRight.m[1][3],
			matEyeRight.m[2][0], matEyeRight.m[2][1], matEyeRight.m[2][2], matEyeRight.m[2][3],
			0, 0, 0, 1.0f
		);

		return matrixObj.inverse();
	}

	Matrix4 ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose)
	{
		Matrix4 matrixObj(
			matPose.m[0][0], matPose.m[0][1], matPose.m[0][2], matPose.m[0][3],
			matPose.m[1][0], matPose.m[1][1], matPose.m[1][2], matPose.m[1][3],
			matPose.m[2][0], matPose.m[2][1], matPose.m[2][2], matPose.m[2][3],
			0, 0, 0, 1.0f
		);
		return matrixObj;
	}

	void UpdatePorjection()
	{
		auto p = static_cast<EdushiVRPipeline*>(this->sysentry->m_pipeline);
		switch (matrixSource) {
			case VR: {
				p->projLeft = projLeft;
				p->projRight = projRight;
				p->viewLeft = eyePosLeft * HMDPos;
				p->viewRight = eyePosRight * HMDPos;
				break;
			}
			case CAMERA: {
				p->projLeft = camera->getProjectionMatrixDXRH();
				p->projRight = camera->getProjectionMatrixDXRH();
				p->viewLeft = camera->getViewMatrix();
				p->viewRight = camera->getViewMatrix();
				break;
			}
			default:break;
		}
	}

	void VRSubmitTexture()
	{
		double beforeSubmit = glfwGetTime();
		//submit texture
		GLuint leftID, rightID;

		((EdushiVRPipeline*)(this->sysentry->m_pipeline))->GetStereoTex(leftID, rightID);
		vr::Texture_t leftEyeTexture = { (void*)leftID, vr::API_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);

		vr::Texture_t rightEyeTexture = { (void*)rightID, vr::API_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);

		double afterSubmit = glfwGetTime();

		glFlush();
		//glFinish();

		double beforeUpdate = glfwGetTime();
		//UpdateHMDMatrixPose
		vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);

		for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
		{
			if (m_rTrackedDevicePose[nDevice].bPoseIsValid)
			{
				devicePose[nDevice] = ConvertSteamVRMatrixToMatrix4(m_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking);
			}
		}

		if (m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
		{
			//if (free)
				HMDPos = devicePose[vr::k_unTrackedDeviceIndex_Hmd].inverse();
			//else
			//{
			//	speed += acceleration;
			//	t += speed;
			//	Vector3 pos = interp.GetInterpolation(t);
			//	Vector3 dir = interp.GetDerivative(t);
			//	//Vector3 pos = circleRoute.GetInterpolation(t);
			//	//Vector3 dir = circleRoute.GetDerivative(t);
			//	dir.normalize();
			//	dir = -dir;
			//	//cout << t << " pos " << pos.x << "," << pos.y << "," << pos.z << endl;
			//	//cout << t << " dir " << dir.x << "," << dir.y << "," << dir.z << endl;

			//	//camera->lookAt(pos, pos + dir, Vector3(0, 1, 0));
			//	Vector3 xAxis = dir.crossProduct(Vector3(0, 1, 0));
			//	xAxis.normalize();
			//	Vector3 yAxis = xAxis.crossProduct(dir);
			//	Matrix4 model
			//	(
			//		xAxis[0], yAxis[0], dir[0], pos[0],
			//		xAxis[1], yAxis[1], dir[1], pos[1],
			//		xAxis[2], yAxis[2], dir[2], pos[2],
			//		0, 0, 0, 1
			//	);
			//	/*Matrix4 model
			//	(
			//		1, 0, 0, 0,
			//		0, 1, 0, 2,
			//		0, 0, 1, 2,
			//		0, 0, 0, 1
			//	);*/
			//	HMDPos = model.inverse();
			//	/*devicePose[vr::k_unTrackedDeviceIndex_Hmd] = model;
			//	HMDPos = devicePose[vr::k_unTrackedDeviceIndex_Hmd].inverse();*/
			//}
		}

		double afterUpdate = glfwGetTime();
	}

	private :
		//VR config
		vr::IVRSystem *m_pHMD;
		vr::IVRRenderModels *m_pRenderModels;//model of devices
		vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
		Matrix4 devicePose[vr::k_unMaxTrackedDeviceCount];
		uint32_t renderWidth;
		uint32_t renderHeight;
		Matrix4 projLeft;
		Matrix4 projRight;
		Matrix4 eyePosLeft;
		Matrix4 eyePosRight;
		Matrix4 HMDPos;
		float nearClip;
		float farClip;
		string vrScene;
};
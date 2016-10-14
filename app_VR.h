#pragma once
#include "Engine/AppFrameWork.h"
#include "TextureManager.h"
#include "ResourceManager.h"
#include "pass.h"
#include "PBRPipeline.h"
#include "EngineUtil.h"
#include "Image.h"
#include "RenderSystem.h"
#include "Config.h"
#include "AppFrameWork2.h"
#include "ForwardPipeline.h"
#include "VRPipeline.h"
#include "FrameListener.h"
#include "EGLTexture.h"
#include <sstream>
#include <openvr.h>

class App_VR :public App {

public:
	bool shutdown = false;
	GlobalVariablePool * gp;
	RenderSystem * render;
	SceneManager * scene;
	MeshManager * meshMgr;
	
	RenderTarget * renderwindow;
	std::vector<FrameListener *> framelistener_list;

	void Init() {
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
		farClip = 30.0f;
		m_pHMD->GetRecommendedRenderTargetSize(&renderWidth, &renderHeight);

		//init not success, shut down
		if (!success)
		if (m_pHMD)
		{
			vr::VR_Shutdown();
			m_pHMD = NULL;
		}

		//init render
		render = new EGLRenderSystem;
		render->SetWandH(renderWidth, renderHeight);
		this->w = renderWidth;
		this->h = renderHeight;
		render->Initialize();
		InitDevice();
		InitPipeline();
		CreateScene();
		
		//init projection mat and transition matrix between eyes and HMD
		SetupCameras();
	}
	void InitPipeline()
	{
		auto pipeline = new VRPipeline;

		pipeline->Init();
		sysentry->setPipeline(pipeline);
	}

	void Render() {
		ImGui_ImplGlfwGL3_NewFrame();

		//render to buffer
		((VRPipeline*)(this->sysentry->m_pipeline))->projLeft = projLeft;
		((VRPipeline*)(this->sysentry->m_pipeline))->projRight = projRight;

		((VRPipeline*)(this->sysentry->m_pipeline))->viewLeft = eyePosLeft * HMDPos;
		((VRPipeline*)(this->sysentry->m_pipeline))->viewRight = eyePosRight * HMDPos;

		sysentry->updateMainView();

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
			HMDPos = devicePose[vr::k_unTrackedDeviceIndex_Hmd].inverse();
		}

		//submit texture
		GLuint leftID, rightID;
		
		((VRPipeline*)(this->sysentry->m_pipeline))->GetStereoTex(leftID, rightID);
		vr::Texture_t leftEyeTexture = { (void*)leftID, vr::API_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);

		vr::Texture_t rightEyeTexture = { (void*)rightID, vr::API_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);

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
		camera->lookAt(Vector3(0, 0, 2.5), Vector3(0, 0, 0), Vector3(0, 1, 0));
		float fovy = 45;
		camera->setPerspective(fovy, float(w) / float(h), 0.01, 100);

		loadModels();
	}
	void loadModels() {
		SceneManager* scene = SceneContainer::getInstance().get("scene1");
		SceneNode* root = scene->GetSceneRoot();
		SceneNode* s;

		//s = LoadMeshtoSceneNode("model/mitsuba/mitsuba.obj", "mitsuba");
		//s->scale(0.2, 0.2, 0.2);
		//root->attachNode(s);
		for (int i = 0; i < 8; i++) {
			stringstream ss;
			ss << "model/pbr_test/sphere" << i << ".obj";
	
			s = LoadMeshtoSceneNode(ss.str(), "sphere" + to_string(i));
			s->scale(0.15, 0.15, 0.15);
			s->translate(i*0.33 - 1.1, -0.2, 0);
			root->attachNode(s);
		}
		for (int i = 0; i < 8; i++) {
		
			stringstream ss;
			ss << "model/pbr_test/sphere" << i << "d.obj";

			s = LoadMeshtoSceneNode(ss.str(), "sphere" + to_string(i)+"d");
			s->scale(0.15, 0.15, 0.15);
			s->translate(i*0.33 - 1.1, 0.2, 0);
			root->attachNode(s);
		}

	}
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
		FileUtil::addSceneFilePath("sdcard/OpenGLES/shader/");
		FileUtil::addSceneFilePath("sdcard/OpenGLES/default_asserts/");
		FileUtil::addSceneFilePath("sdcard/OpenGLES/");
#else
		FileUtil::addSceneFilePath("shader/");
		FileUtil::addSceneFilePath("default_asserts/");
#endif 

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

	private :
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

	public :
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

		void SetupCameras() 
		{
			projLeft = GetHMDMatrixProjectionEye(vr::Eye_Left);
			projRight = GetHMDMatrixProjectionEye(vr::Eye_Right);
			eyePosLeft = GetHMDMatrixPoseEye(vr::Eye_Left);
			eyePosRight = GetHMDMatrixPoseEye(vr::Eye_Right);
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
};
#pragma once
#include "ForwardPipeline.h"
#include "AppFrameWork2.h"
#include "Interpolation.h"

#include <fstream>
class App_VR_Experience :public App {

public:
	SceneNode* root;
	SceneNode* route;
	Interpolation interp;
	float t = 0;
	bool free = true;
	float speed = 0.002;
	float acceleration = 0;
	vector<Vector3> marks;

	void Init() {
		render = new EGLRenderSystem;
		render->SetWandH(w, h);
		render->Initialize();
		InitDevice();
		InitPipeline();
		CreateScene();
		LoadMarks("default.txt");
		SetInterp();
	}
	void InitPipeline()
	{
		pipeline = new ForwardPipeline;
		pipeline->Init();
	}

	void Render() {
		UpdateGUI();

		if (!free)
			FollowRoute();
		pipeline->Render();
	}
	void UpdateGUI() {
		GUI::NewFrame();
		ImGui::Begin("Params");
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		if (ImGui::TreeNode("Setting"))
		{
			if (ImGui::Button("Reset"))
				t = 0;
			ImGui::SameLine();
			if (ImGui::Button("Start/Stop"))
				free = !free;

			if (!free)
			{
				ImGui::DragFloat("Speed", &speed, 0.001);
				ImGui::DragFloat("Acceleration", &acceleration, 0.001);
			}
			
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Camera"))
		{
			auto p = static_cast<ForwardPipeline*>(pipeline);
			static Vector3 pos, dir;
			pos = camera->getPosition();
			dir = camera->getDirection();
			ImGui::DragFloat3("CameraPos", &pos[0], 0.05);
			ImGui::DragFloat3("CameraDir", &dir[0], 0.05);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Marks"))
		{
			if (ImGui::Button("Clear"))
				marks.clear();
			ImGui::SameLine();
			if (ImGui::Button("Push"))
				marks.push_back(camera->getPosition());
			ImGui::SameLine();
			if (ImGui::Button("Pop") && !marks.empty())
				marks.pop_back();
			for (auto&pos : marks)
				ImGui::InputFloat3("", &pos[0]);
			static char name[50]="";
			ImGui::InputText("File Name", name, 50);
			if (ImGui::Button("Load"))
			{
				LoadMarks(name);
				SetInterp();
				t = 0;
			}
			ImGui::SameLine();
			if (ImGui::Button("Save"))
				SaveMarks(name);
			ImGui::TreePop();
		}
		ImGui::End();
	}
	void CreateScene() {
		scene = new OctreeSceneManager("scene1", render);
		SceneContainer::getInstance().add(scene);
		SceneNode* pRoot = scene->GetSceneRoot();
		camera = scene->CreateCamera("main");
		camera->lookAt(Vector3(-7, 1, 0), Vector3(0, 0, 0), Vector3(0, 1, 0));
		float fovy = 45;
		camera->setPerspective(fovy, float(w) / float(h), 0.01, 100);
		loadModels();

	}
	void loadModels() {
		root = scene->GetSceneRoot();
		SceneNode* s = LoadMeshtoSceneNode("model/16.10/sponza/sponza.obj", "sponza");
		root->attachNode(s);
		//circleRoute.SetData(Vector3(0, 5, 0), 1, 1, 0);
		//route = new SceneNode;
		//UpdateRoute();
		//root->attachNode(route);
		//interpolation test
		//vector<double> x = { 0.1,2,4,6,8 };
		//vector<Vector3> y = { Vector3(-9,4,-1),Vector3(-4,2,1),Vector3(0,3.5,2),Vector3(5.5,5,-1),Vector3(8,7.5,-2) };

		/*s = LoadMeshtoSceneNode("model/test/cube.obj", "s1");
		s->setScale(Vector3(0.1));
		s->setTranslation(y[0]);
		root->attachNode(s);*/
	}

	/*void UpdateRoute() {
		route->detachAllNodes();
		vector<Vector3> tags = circleRoute.GenTags(10);
		SceneNode *s;
		int cnt = 0;
		for (auto &t : tags) {
			string name = "tag" + to_string(cnt);
			s = LoadMeshtoSceneNode("model/test/cube.obj", name);
			s->setScale(Vector3(0.1));
			s->setTranslation(t);
			route->attachNode(s);
			cnt++;
		}
	}*/
	void FollowRoute() {
		speed += acceleration;
		t += speed;
		Vector3 pos = interp.GetInterpolation(t);
		Vector3 dir = interp.GetDerivative(t);
		//Vector3 pos = circleRoute.GetInterpolation(t);
		//Vector3 dir = circleRoute.GetDerivative(t);
		dir.normalize();
		//cout << t << " pos " << pos.x << "," << pos.y << "," << pos.z << endl;
		//cout << t << " dir " << dir.x << "," << dir.y << "," << dir.z << endl;

		camera->lookAt(pos, pos + dir, Vector3(0, 1, 0));
	}

	void LoadMarks(string filename)
	{
		ifstream input("..\\Workspace\\VRExperience\\" + filename);
		if (input.fail())
		{
			cout << "\"" << filename << "\"" << "doesn't exist" << endl;
			return;
		}
		marks.clear();
		Vector3 pos;
		while (input >> pos.x >> pos.y >> pos.z)
			marks.push_back(pos);
		input.close();
	}

	void SaveMarks(string filename)
	{
		ofstream output("..\\Workspace\\VRExperience\\" + filename);
		output.clear();
		for (auto& pos : marks)
			output << pos.x << "\t" << pos.y << "\t" << pos.z << endl;
		output.close();
	}

	void SetInterp()
	{
		vector<double> x;
		double t = 0.001;
		for (auto &pos : marks)
		{
			x.push_back(t);
			t += 2;
		}
		interp.SetData(x, marks);
	}
};
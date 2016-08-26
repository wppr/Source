#pragma once
#include "AppFrameWork2.h"
#include <fstream>


class CarTrackingData {
public:
	Vector3 pos=Vector3(0,0,0);
	Vector3 LastPos = Vector3(0, 0, 0);
	Vector3 d;
	Quaternion q=Quaternion::IDENTITY;
	int frame = 0;
	SceneNode* Node=NULL;
	void Track(Vector3 newpos) {
		frame++;
		LastPos = pos;
		pos = newpos;
		Vector3 tmpd = newpos - LastPos;
		if (tmpd.length() < 1e-5)
			return;
		d = newpos - LastPos;
		d.normalize();

	}
	void UpdateNode() {
		Node->setTranslation(GetPos());
		Node->setOrientation(GetOrientation());
	}
	Vector3 GetPos() {
		return pos;
	}

	Quaternion GetOrientation() {
		Vector3 d0 = Vector3(0, 0, 1);
		if ((d - d0).length() < 1e-5) {
			q.fromAngleAxis(0, Vector3(0, 1, 0));
		}
		else 
		{
			float c = d.dotProduct(d0);
			float angle = acos(c);
			float clockwise = 1;
			if (d.x < 0) 
				clockwise = -1;
			q.fromAngleAxis(clockwise*angle, Vector3(0, 1, 0));
		}
		return q;
	}
};


class CarSimulation {
public:
	App* app=NULL;
	vector<MeshPtr> CarList;
	fstream f;
	map<string,CarTrackingData> carmap;
	int MaxCarNum=100;
	int CarTypeNum = 11;
	int RunningCarNum = 1;
	void Init(App* _app) {
		this->app = _app;
		f = fstream("CarSim/CarData.txt");
		LoadCarMeshes();
		for (int i = 0; i < RunningCarNum;i++) {
			AddCar(to_string(i), i%CarTypeNum);
		}

	}
	void AddCar(string name,int type) {
		CarTrackingData cd;
		InitCarTrack(cd, name, type);
		carmap[name] = cd;
	}
	void removeCar(string name) {

	}
	void LoadCarMeshes() {
		for (int i = 1; i <= CarTypeNum; i++) {
			string prefix = i < 10 ? "car0" : "car";
			string name = prefix + to_string(i);
			MeshPtr mesh = app->meshMgr->loadMesh_assimp_check(name,"model/edushi/car/" + name + ".obj");
			CarList.push_back(mesh);
		}
	}
	void InitCarTrack(CarTrackingData& cardata, string name, int meshId) {
		SceneNode* carNode = app->scene->CreateSceneNode("CarNode"+ name);
		auto mesh = CarList[meshId];
		Entity* entity = app->scene->CreateEntity(name);
		entity->setMesh(mesh);
		carNode->attachMovable(entity);
		carNode->setScale(0.01, 0.01, 0.01);
		app->scene->GetSceneRoot()->attachNode(carNode);
		cardata.Node = carNode;
	}
	void UpdateCars() {
		Vector3 pos;
		f  >> pos.x >> pos.y >> pos.z;
		//test
		pos.y = 0.05;
		if (f.eof()) {
			f.clear();
			f.seekg(0, ios::beg);
		}

		for (int i = 0; i < RunningCarNum; i++) {
			Vector3 offset(0.04*i, 0, -0.04*i);
			carmap[to_string(i)].Track(pos+offset);
			carmap[to_string(i)].UpdateNode();
		}

	}

	void Update() {
		int step = 1;
		static int i = 0;
		i++;
		if (i == step) {
			i = 0;
			UpdateCars();
		}
	}
};
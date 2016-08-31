#pragma once

#include "AppFrameWork2.h"
#include "OctreeSceneManager.h"

#include "ssMath/ssMathSerialize.h"
using namespace HW;

#define PI 3.141592654
struct BlockDef
{
	string name;
	string type;
	string size;
	int orientation;
	vector<int> meshID;
	vector<Vector3> scale;
	vector<float> orientations;
	vector<Vector3> translate;
	
	META(N(name), N(size),N(type),N(orientation),N( meshID), N(scale),N(translate),N(orientations));
};

class BlockHelper {
public:
	map<string, BlockDef> blockPresets;
	vector<MeshPtr> meshs;
	SceneManager* scene=NULL;
	MeshManager* meshMgr=NULL;
	void FillBlockData() {
		BlockDef b;

		for (int i = 1; i <= 15; i++) {
			serializeLoad("model/uniform/data/block" + to_string(i) + ".txt", b);
			blockPresets[b.name] = b;

		}
		vector<string> names = { "xcross","tcross","lcross","xcross_greenlight",
			"tcross_greenlight","lcross_greenlight","street", };
		for (auto x : names) {
			serializeLoad("model/uniform/data/" + x + ".txt", b);
			blockPresets[b.name] = b;
		}
		//{
		//	BlockDef b[10];

		//	b[0].name = "block1";
		//	b[0].type = "BLOCK";
		//	b[0].meshID = { 0,1,4 };
		//	b[0].translate = { Vector3(0,0,0),Vector3(3,0,0),Vector3(1,0,2) };
		//	b[0].scale = { Vector3(0.5),Vector3(0.5),Vector3(0.5) };
		//	b[0].size = "5x3";

		//	b[1].name = "block2";
		//	b[1].type = "BLOCK";
		//	b[1].size = "4x5";
		//	b[1].meshID = { 8,3,7 };
		//	b[1].translate = { Vector3(0,0,1),Vector3(3,0,0),Vector3(3,0,3) };
		//	b[1].scale = { Vector3(0.5),Vector3(0.5),Vector3(0.5) };

		//	b[2].name = "xcross";
		//	b[2].type = "XCROSS";
		//	b[2].size = "1x1";
		//	b[2].meshID = { 16,15,15,15,15 };
		//	//center up left down right
		//	b[2].translate = { Vector3(1 / 3.0,0,1 / 3.0), Vector3(2 / 3.0,0, 1 / 3.0), Vector3(1 / 3.0,0,1 / 3.0), Vector3(1 / 3.0,0,2 / 3.0) , Vector3(2 / 3.0,0,2 / 3.0) };
		//	b[2].scale = { Vector3(1 / 3.0),Vector3(1 / 3.0),Vector3(1 / 3.0),Vector3(1 / 3.0),Vector3(1 / 3.0) };
		//	Vector3 axis(0.0, 1.0, 0.0);
		//	b[2].orientations = { 0.0f, float(PI),  float(1.5*PI), 0.0f, float(0.5*PI) };

		//	b[3].name = "lcross";
		//	b[3].type = "LCROSS";
		//	b[3].size = "1x1";
		//	b[3].meshID = { 16, 15, 15 };
		//	//center down right
		//	b[3].translate = { Vector3(1 / 3.0,0,1 / 3.0), Vector3(1 / 3.0,0,2 / 3.0), Vector3(2 / 3.0,0,2 / 3.0) };
		//	b[3].scale = { Vector3(1 / 3.0),Vector3(1 / 3.0),Vector3(1 / 3.0) };
		//	b[3].orientations = { 0.0f, 0.0f, float(0.5*PI) };

		//	b[4].name = "tcross";
		//	b[4].type = "TCROSS";
		//	b[4].size = "1x1";
		//	b[4].meshID = { 16, 15, 15, 15 };
		//	//center up right down
		//	b[4].translate = { Vector3(1 / 3.0,0,1 / 3.0), Vector3(2 / 3.0,0,1 / 3.0), Vector3(2 / 3.0,0,2 / 3.0),  Vector3(1 / 3.0,0,2 / 3.0) };
		//	b[4].scale = { Vector3(1 / 3.0),Vector3(1 / 3.0),Vector3(1 / 3.0), Vector3(1 / 3.0) };
		//	b[4].orientations = { 0.0f, float(PI), float(0.5*PI), 0.0f };

		//	b[5].name = "street";
		//	b[5].type = "STREET";
		//	b[5].size = "1x1";
		//	b[5].orientation = 1;
		//	b[5].meshID = { 16,16,16 };
		//	b[5].translate = { Vector3(0, 0, 1 / 3.0), Vector3(1 / 3.0, 0, 1 / 3.0) , Vector3(2 / 3.0, 0, 1 / 3.0) };
		//	b[5].scale = { Vector3(1 / 3.0) , Vector3(1 / 3.0) , Vector3(1 / 3.0) };
		//	b[5].orientations = { 0, 0, 0};
		//	for (int i = 0; i < 10; i++) {
		//		blockPresets[b[i].name] = b[i];
		//	}
		//}
	}
	void SaveBlock(string fname,string bname,string bsize,string btype,int borien) {
		auto nodes = scene->GetSceneRoot()->getAllChildNodes();
		vector<SceneNode*> bnodes;
		for (auto& x : nodes) {
			if (x->getName().find("node_b") != string::npos) {
				bnodes.push_back(x);
			}
		}
		BlockDef b;
		b.name = bname;
		b.size = bsize;
		b.type = btype;
		b.orientation = borien;
		for (auto& node : bnodes) {
			auto t = node->getLocalTranslation();
			auto s = node->getLocalScale();
			auto r = node->getOrientation();
			float rad=0;
			Vector3 axis;
			r.toAngleAxis(rad, axis);
			int id = -1;
			for (int i = 0; i < meshs.size();i++) {
				OMesh* meshptr = meshs[i].InterPtr();
				if (scene->getEntity(node->getName())->getMesh().InterPtr() == meshptr)
				{
					id = i; 
					b.meshID.push_back(id);
					b.scale.push_back(s);
					b.translate.push_back(t);
					b.orientations.push_back(rad);
					break;;
				}
			}
		}
		b.orientation = 1;
		b.type = "block";
		serializeSave(fname, b);
	}
	string LoadBlockPreset(string fname) {
		BlockDef b;
		serializeLoad(fname, b);
		blockPresets[b.name] = b;
		return b.name;
	}
	void Init(SceneManager* _scene,MeshManager* _meshmgr ) {
		scene = _scene;
		meshMgr = _meshmgr;
		map<int, string> modellist;

		for (int i = 1; i <= 9; i++) {
			modellist[i] = "building0" + to_string(i) + ".obj";
		}
		for (int i = 10; i <= 15; i++) {
			modellist[i] = "building" + to_string(i) + ".obj";
		}
		modellist[16] = "road1.obj";
		modellist[17] = "road2.obj";
		modellist[18] = "lightGreen2.obj";
		modellist[19] = "lightRed2.obj";
		modellist[20] = "lightYellow2.obj";
		modellist[21] = "roadpark.obj";
		modellist[22] = "busStation.obj";

		int end = 23;
		for (int i = 16; i <= 25; i++) {
			modellist[end] = "building" + to_string(i) + ".obj";
			end++;
		}

		for (int i = 1; i < end; i++) {
			MeshPtr mesh = meshMgr->loadMesh_assimp_check("", "model/uniform/" + modellist[i]);
			meshs.push_back(mesh);
		}

		FillBlockData();
	}
	void AttachBlock(string blockname, string nodename, Vector3 pos) {
		SceneNode* root=scene->GetSceneRoot();
		auto blockit = blockPresets.find(blockname);
		if (blockit == blockPresets.end())
			return;
		auto block = blockit->second;
		//avoid repeat node
		for (int i = 0; i < block.meshID.size(); i++) {
			string name = nodename + to_string(i);
			auto snode = scene->getSceneNode(name);
			if (snode != NULL) {
				removeNode(name);
			}
			snode = scene->getSceneNode(name);
				auto entity = scene->CreateEntity(name);
				entity->setMesh(meshs[block.meshID[i]]);
				snode = scene->CreateSceneNode(name);
				snode->attachMovable(entity);
				root->attachNode(snode);
			//}
			snode->setTranslation(block.translate[i] + pos);
			snode->setScale(block.scale[i]);
			Quaternion q;
			q.fromAngleAxis(block.orientations[i], Vector3(0, 1, 0));
			snode->setOrientation(q);
		}
	}
	void removeNode(string nodename) {
		auto node = scene->getSceneNode(nodename);
		auto entity = scene->getEntity(nodename);
		if (node != NULL) {
			node->getParent()->detachNode(node);
			scene->destroy(node);
		}
		if (entity != NULL)
			scene->destroy(entity);
	}
	void PutblockTest() {

		for (int i = 1; i <= 10; i++) {
			AttachBlock(to_string(i), "block_demo" + to_string(i), Vector3((i%4)*4, 0, (i /4)*4));
		}

	}
};
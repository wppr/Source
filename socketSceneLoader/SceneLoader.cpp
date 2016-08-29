#include "SceneLoader.h"
#include <rapidjson\document.h>
#include <rapidjson\writer.h>
#include <rapidjson\stringbuffer.h>
#include <fstream>
#include <sstream>

using namespace Block;
using namespace rapidjson;
using std::pair;
using std::cout;
using std::endl;
using std::pair;
using std::ifstream;
using std::stringstream;

#define PI 3.141592654


SceneLoader::SceneLoader(SceneManager * scene, MeshManager * meshMgr, int width, int height) : width(width), height(height)
{
	this->scene = scene;
	this->meshMgr = meshMgr;
	this->layoutMatrix = new Entry[width * height];

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			this->layoutMatrix[i * width + j].SetX(j);
			this->layoutMatrix[i * width + j].SetY(i);
		}
	}

	InitSceneNode();
	InitFloor();

	bh.Init(scene, meshMgr);
}

void SceneLoader::ParseScene(string json)
{
	//cout << json << endl;
	//debug
	vector<pair<int, int>> cords;

	//parse json into sceneMatrix
	Document root;
	root.Parse(json.c_str());
	if (!root.IsObject())
	{
		cout << "json size: " << json.size() << endl;
		cout << "json str: " << json << endl;
		return;
	}
	assert(root.IsObject());
	Value &blockSize = root["blockSize"];
	width = blockSize["width"].GetInt();
	height = blockSize["height"].GetInt();
	
	//set all to NULL
	for (int i = 0; i < 32 * 16; ++i)
	{
		layoutMatrix[i].IsEmpty() = true;
		layoutMatrix[i].IsDrawable() = false;

		layoutMatrix[i].GetBlock() = BlockDef();
	}
	if (!crosses.empty())
	{
		//cout << crosses.size() << std::endl;
		crosses.clear();
	}

	Value &entry = root["entry"];
	assert(entry.IsArray());
	//cout << "num " << entry.Size() << endl;
	for (SizeType i = 0; i < entry.Size(); ++i)
	{
		int x = entry[i]["x"].GetInt();
		int y = entry[i]["y"].GetInt();
		if (!(x >= 0 && x < 32 && y >= 0 && y < 16)) continue;
		string type = entry[i]["type"].GetString();
		
		this->layoutMatrix[y * width + x].IsEmpty() = false;
		this->layoutMatrix[y * width + x].IsDrawable() = true;

		if("BLOCK" == type)
		{
			Entry::EntryType type;
			string name = entry[i]["id"].GetString();
			BlockDef block = bh.blockPresets[name];
			if (name.substr(1, 5) == "cross")
			{
				crosses.push_back(y * width + x);
				if (name == "xcross" || name == "xcross_greenlight")
				{
					type = Entry::EntryType::XCROSS;
				}
				else if(name == "tcross" || name == "tcross_greenlight")
					type = Entry::EntryType::TCROSS;
				else if(name == "lcross" || name == "lcross_greenlight")
					type = Entry::EntryType::LCROSS;
			}
			else
				type = Entry::EntryType::BLOCK;

			//span
			string size = block.size;
			int x_pos = size.find_first_of("x");
			string spanX = size.substr(0, x_pos);
			string spanY = size.substr(x_pos + 1, size.size());

			//modify matrix
			this->layoutMatrix[y * width + x].GetBlock() = block;
			this->layoutMatrix[y * width + x].SetEntryType(type);
			int orientation = entry[i]["orientation"].GetInt();
			this->layoutMatrix[y * width + x].GetBlock().orientation = orientation;

			int rotated_spanX, rotated_spanY;
			if (orientation % 2 == 1)
			{
				rotated_spanX = atoi(spanX.c_str());
				rotated_spanY = atoi(spanY.c_str());
			}
			else
			{
				rotated_spanX = atoi(spanY.c_str());
				rotated_spanY = atoi(spanX.c_str());
			}
			for (int i = 0; i < rotated_spanX; ++i)
				for (int j = 0; j < rotated_spanY; ++j)
				{
					this->layoutMatrix[(y + j) * width + x + i].IsEmpty() = false;
					this->layoutMatrix[(y + j) * width + x + i].SetEntryType(type);
				}
		}
	}

	//generate street
	 
	pair<int, int> dirs[4];
	dirs[0] = pair<int, int>(0, 1); 
	dirs[1] = pair<int, int>(1, 0);
	dirs[2] = pair<int, int>(0, -1);
	dirs[3] = pair<int, int>(-1, 0);

	for (int i = 0; i < crosses.size(); ++i)
	{
		int fork = layoutMatrix[crosses[i]].GetEntryType();
		for (int k = 0; k < fork; ++k)//expand the cross 
		{
			int crossOrien = layoutMatrix[crosses[i]].GetBlock().orientation;
			pair<int, int> dir = dirs[(k - 1 + crossOrien) % 4];

			for (int x = layoutMatrix[crosses[i]].GetX() + dir.first, y = layoutMatrix[crosses[i]].GetY() + dir.second; x >= 0 && x < width && y >= 0 && y < height;
				x += dir.first, y += dir.second)
			{
				if (this->layoutMatrix[y * width + x].IsEmpty())//NULL
				{
					this->layoutMatrix[y * width + x].IsEmpty() = false;
					this->layoutMatrix[y * width + x].IsDrawable() = true;
					string name = "street";
					BlockDef bd = bh.blockPresets[name];
					bd.orientation = (k + crossOrien) % 2 + 1;
					this->layoutMatrix[y * width + x].GetBlock() = bd;
					//this->layoutMatrix[y * width + x].SetObject(new Street(k + 1));
					this->layoutMatrix[y * width + x].SetEntryType(Entry::EntryType::STREET);
				}

				else if (this->layoutMatrix[y * width + x].GetEntryType() == Entry::EntryType::BLOCK)//BLOCK
					break;

				else
				{
					Entry::EntryType type = this->layoutMatrix[y * width + x].GetEntryType();
					BlockDef bd = this->layoutMatrix[y * width + x].GetBlock();
					//TCROSS
					if (type == Entry::EntryType::TCROSS && bd.orientation % 4 == (k + 1 + crossOrien) % 4)
						break;
					//LCROSS
					else if (type == Entry::EntryType::LCROSS
						&& bd.orientation % 4 == (k + 1 + crossOrien) % 4 || bd.orientation % 4 == (k + 2 + crossOrien) % 4)
						break;
					else
						continue;
				}
			}
		}
	}
}

void SceneLoader::PrintLayout()
{
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
			if (!this->layoutMatrix[i * width + j].IsEmpty())
				printf("1 ");
			else
				printf("0 ");
		printf("\n");
	}
}

void SceneLoader::InitServer(string ip, string port)
{
	server.SetAddr(ip, port);
}

void SceneLoader::UpdateScene()
{
	string json;
	mutex m;

	if (show_json)
	{
		ParseScene(staticJson);
		UpdateSceneNodes();
	}

	else if (!server.jsonQueue.empty())
	{
		//m.lock();
		int size = server.jsonQueue.size();
		for (int i = 0; i < size - 1; ++i)
		{
			server.jsonQueue.pop();
		}
		json = server.jsonQueue.front();
		server.jsonQueue.pop();
		cout << "json queue size " << server.jsonQueue.size() << endl;
		//m.unlock();
		ParseScene(json);
		UpdateSceneNodes();
	}
}

//Init scene nodes and entities
void SceneLoader::InitSceneNode()
{
	//sceneNodes
	sceneNodes = new SceneNode*[width * height];
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			stringstream name;
			name.str("");
			name << "node" << i << " " << j;
			sceneNodes[i * width + j] = this->scene->CreateSceneNode(name.str());
		}
	}

}

//Init floar nodes and entities
void SceneLoader::InitFloor()
{
	//floarNodes
	floarNodes = new SceneNode*[16 * 32];
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 32; ++j) {
			stringstream name;
			name.str("");
			name << "floar" << i << " " << j;
			floarNodes[i * 32 + j] = this->scene->CreateSceneNode(name.str());
		}
	}

	//floarEntities
	floarEntities = new Entity*[16 * 32];
	for (int i = 0; i < 16; ++i)
	{
		for (int j = 0; j < 32; ++j)
		{
			stringstream name;
			name << "floar" << i << " " << j;
			floarEntities[i * 32 + j] = this->scene->CreateEntity(name.str());
			floarNodes[i * 32 + j]->attachMovable(floarEntities[i * 32 + j]);
		}
	}

}

void SceneLoader::AttachFloar()
{
	SceneManager* scene_instance = SceneContainer::getInstance().get("scene1");
	SceneNode* root = scene_instance->GetSceneRoot();

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; ++j) {
			floarEntities[i * width + j]->setMesh(floarFragment);
			floarNodes[i * width + j]->setTranslation(j, 0, i);
			root->attachNode(floarNodes[i * width + j]);
		}
	}
}

void SceneLoader::UpdateSceneNodes()
{
	SceneManager* scene = SceneContainer::getInstance().get("scene1");
	SceneNode* root = scene->GetSceneRoot();
	root->detachAllNodes();
	AttachFloar();

	for (int i = 0; i < height; i++) {

		for (int j = 0; j < width; ++j)
		{
			if (layoutMatrix[i * width + j].IsDrawable())
			{
			
				BlockDef &block = layoutMatrix[i * width + j].GetBlock();
				string id = block.name;

				int orien = block.orientation;
				string span = block.size;
				int xPos = span.find_first_of("x");
				int spanX = atoi(span.substr(0, xPos).c_str());
				int spanY = atoi(span.substr(xPos + 1, span.size()).c_str());
				int xCalibration = 0, yCalibration = 0;
				switch (orien)
				{
				case 2:
					yCalibration = spanX;
					break;
				case 3:
					xCalibration = spanX;
					yCalibration = spanY;
					break;
				case 4:
					xCalibration = spanY;
					break;
				}

				Quaternion quaternion((orien - 1) * 0.5f * PI, Vector3(0, 1, 0));
				sceneNodes[i * width + j]->setOrientation(quaternion);
				sceneNodes[i * width + j]->setTranslation(j + xCalibration, 0, i + yCalibration);
				
				sceneNodes[i * width + j]->detachAllNodes();

				for (int k = 0; k < block.meshID.size(); ++k)
				{
					string name = "entry" + to_string(i) + " " + to_string(j) + "mesh"+to_string(k);
					SceneNode* node = scene->getSceneNode(name);
					if (NULL != node)
						scene->destroy(node);
					node = scene->CreateSceneNode(name);

					Entity* entity = scene->getEntity(name);
					if (NULL != entity)
						scene->destroy(entity);
					entity = scene->CreateEntity(name);

					entity->setMesh(bh.meshs[block.meshID[k]]);
					node->attachMovable(entity);
					node->setScale(block.scale[k]);
					//rotation
					if (block.orientations.size() > k)
					{
						Quaternion q(block.orientations[k], Vector3(0, 1, 0));
						node->setOrientation(q);
					}
					node->setTranslation(block.translate[k]);
					sceneNodes[i * width + j]->attachNode(node);
				}

				root->attachNode(sceneNodes[i * width + j]);

			}
		}
	}

}

void SceneLoader::loadMesh() {

	//BUILDING
	for (int i = 0; i < 5; ++i)
	{
		stringstream ss, ss1;
		ss << "model/uniform/building0" << i + 1 << ".obj";
		ss1 << "building0" << i + 1;
		auto meshPtr = meshMgr->loadMesh_assimp_check(ss1.str(), ss.str());
		cout << ss1.str() << endl;
		buildings.insert(std::pair<string, MeshPtr>(ss1.str(), meshPtr));
	}

	//street
	stringstream ss, ss1;
	ss << "model/uniform/road2.obj";
	ss1 << "street";
	roadFragment = meshMgr->loadMesh_assimp_check(ss1.str(), ss.str());

	//cross
	ss.str("");
	ss1.str("");
	ss << "model/uniform/road1.obj";
	ss1 << "cross";
	crossFragment = meshMgr->loadMesh_assimp_check(ss1.str(), ss.str());

	//floor
	ss.str("");
	ss1.str("");
	ss << "model/uniform/floor.obj";
	ss1 << "floor";
	floarFragment = meshMgr->loadMesh_assimp_check(ss1.str(), ss.str());

	AttachFloar();
}

void SceneLoader::LoadJson()
{
	ifstream stream, stream1;
	stringstream ss;

	ss.str("");
	stream1.open("model/uniform/data/block.json");
	if (stream1.is_open())
	{
		ss << stream1.rdbuf();
		staticJson = ss.str();
	}
}


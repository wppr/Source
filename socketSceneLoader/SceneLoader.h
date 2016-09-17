#pragma once
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <mutex>
#include "connection.h"

#include <sstream>
#include <map>
#include "AppFrameWork2.h"
#include "Block.h"
using std::string;
using std::stringstream;
using std::vector;
using std::list;
using std::cout;
using std::mutex;
using std::map;
using std::pair;

namespace Block
{
	
	class Entry
	{
	public :
		enum EntryType
		{
			STREET = 0,
			XCROSS = 4,
			LCROSS = 2,
			TCROSS = 3,
			BLOCK = 5,
			AFFL = 6
		};

	protected :
		EntryType entryType;
		int x, y;
		bool drawable;//true if marker exist
		bool empty;//true if object cover
		vector<BlockDef> blocks;

	public :
		Entry()
		{
			empty = true;
			drawable = false;
		}

		~Entry()
		{
			
		}

		void SetEntryType(EntryType entryType)
		{
			this->entryType = entryType;
		}

		EntryType GetEntryType()
		{
			return this->entryType;
		}

		void SetX(int x)
		{
			this->x = x;
		}

		void SetY(int y)
		{
			this->y = y;
		}

		int GetX()
		{
			return this->x;
		}

		int GetY()
		{
			return this->y;
		}

		bool& IsEmpty()
		{
			return this->empty;
		}

		bool& IsDrawable()
		{
			return this->drawable;
		}

		vector<BlockDef>& GetBlock()
		{
			return this->blocks;
		}

	private:
		Entry(const Entry&);
		Entry& operator= (const Entry&);
	};

	class Car
	{
	public:
		Car() {}
		Car(Vector3 position, Vector3 direction, float speed, SceneNode* sceneNode)
			: position(position), direction(direction), speed(speed), carNode(carNode)
		{
			carNode->setTranslation(position);

		}

		void Move(float deltaTime);
		bool IsOutOfBound(int l, int r, int t, int b);

	private:
		Vector3 position;
		Vector3 direction;
		float speed;
		SceneNode* carNode;
	};

	class SceneLoader
	{

	public:
		bool show_json = false;
		bool rotateFlag;
		//default 32 * 16
		SceneLoader(SceneManager * scene, MeshManager * meshMgr, int width, int height);

		~SceneLoader()
		{
			printf("delete SceneLoader\n");
			delete [] layoutMatrix;
			delete sceneNodes;
			delete floarNodes;
			delete floarEntities;
		}

		//Scene Management
		void ParseScene(string json);//from json to layoutMatrix
		void LoadJson();//static load json from file
		void UpdateScene();
		void loadMesh();
		void InitSceneNode();
		void InitFloor();
		void AttachFloar();
		void UpdateSceneNodes();
		void GetRandomCars(vector<Vector3>& roadPos, vector<Quaternion>& roadOrien);
		int GetCarName();
		void MoveCars();

		//Getters $ Setters
		int GetWidth()
		{
			return this->width;
		}

		int GetHeight()
		{
			return this->height;
		}

		bool IsEmpty(int x, int y)
		{
			return layoutMatrix[y * width + x].IsEmpty();
		}

		Entry* GetMatrix()
		{
			return layoutMatrix;
		}

		//helper functions
		void PrintLayout();
		
		//Server
		void InitClient(string ip, string port);

		//cars
		void PushCar(Vector3 position, Vector3 direction, float speed)
		{
			int name = GetCarName();
			SceneNode* carNode = this->scene->CreateSceneNode("car" + to_string(name));
			Car car(position, direction, speed, carNode);
			this->cars.push_back(car);
		}

		//block
		BlockHelper bh;
	private :
		int width, height;//width & height of the layoutMatrix
		Entry* layoutMatrix;
		vector<int> crosses;
		vector<Vector3> streetPos;
		vector<Quaternion> streetOrien;
		list<Car> cars;
		bool carNames[200];

		SceneManager * scene;
		MeshManager * meshMgr;
		map<string, MeshPtr> buildings;
		MeshPtr roadFragment;
		MeshPtr crossFragment;
		MeshPtr floarFragment;
		SceneNode** sceneNodes;
		SceneNode** floarNodes;
		Entity** floarEntities;

		string staticJson;

		//network
		Client client;

		//invalid invoke
		SceneLoader(const SceneLoader&);
		SceneLoader& operator= (const SceneLoader&);
	};

}


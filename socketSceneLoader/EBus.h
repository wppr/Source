#pragma once
#include <string>
#include <vector>
using std::string;
using std::pair;
using namespace std;

class EBusTrack;

class EBus
{
public :
	enum Status
	{
		NEW,
		RUNNING,
		STOP,
		REMOVED,
		CHARGING   // RUNING STOP CHARGING defined
	};

	enum VType
	{
		EBUS,
		TAXI,
		PRIVATECAR
	};
	//void GetEBusInfo(vector<EBusTrack>& eBusTrack, EBus& ebus);
public :
	int id;
	string time;
	Status status;
	VType vtype;
	pair<float, float> location;
	pair<float, float> lastLocation;
	pair<int, int> direction;// ¡ý(0,1) 1  ¡ú(1,0) 2  ¡ü(0,-1) 3  ¡û(-1,0) 4
	float speed;
	bool isShowEnergy;
	int energyLevel;
	double startStopTime;
	int runningStage;
	static void run(EBus& ebus,double timeStamp);
	int meet(vector<EBusTrack>& stations);
};


class EBusTrack
{
public :
	int x;
	int y;
	EBusTrack(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
	EBusTrack()
	{

	}
};
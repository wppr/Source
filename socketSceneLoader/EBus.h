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

	static const float ENERGYREDUCESPEED;
	static const float CHARGINGSPEED;
	static const float BATTERYCAPACITY;

	inline EBus() {}
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
	float energyLevel;//0->5
	double startStopTime;
	double lastRunTime;
	//double lastChargingTime;
	//double startChargineTime;
	int runningStage;
	static void run(EBus& ebus,double timeStamp);
	int meet(vector<EBusTrack>& stations);
	inline EBus(double timeStamp)
	{
		lastRunTime = timeStamp;
		//lastChargingTime = timeStamp;
		energyLevel = 1.0;
		status = EBus::RUNNING;
		speed = 0.0;
		runningStage = 1;
		vtype = EBus::EBUS;
		lastLocation.first = 3.0 + 5 / 12.0 + 1/24.0;      // Location
		lastLocation.second = 2 + 1 / 6.0;
		location = lastLocation;
		direction.first = 0;                       // Direction
		direction.second = 1;
	}
	inline void reset(double timeStamp)
	{
		lastRunTime = timeStamp;
		//lastChargingTime = timeStamp;
		energyLevel = 1.0;
		status = EBus::RUNNING;
		speed = 0.0;
		runningStage = 1;
		vtype = EBus::EBUS;
		lastLocation.first = 3.0 + 5 / 12.0 +1/24.0;      // Location
		lastLocation.second = 2 + 1 / 6.0;
		location = lastLocation;
		direction.first = 0;                       // Direction
		direction.second = 1;
	}
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
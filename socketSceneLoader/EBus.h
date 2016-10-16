#pragma once
#include <string>
using std::string;
using std::pair;


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
	pair<int, int> direction;// ��(0,1) 1  ��(1,0) 2  ��(0,-1) 3  ��(-1,0) 4
	float speed;
	bool isShowEnergy;
	int energyLevel;
};


class EBusTrack
{
public :
	int x;
	int y;
};
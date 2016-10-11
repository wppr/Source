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
		REMOVED
	};

	enum VType
	{
		BUS,
		TAXI,
		PRIVATECAR
	};

private :
	int id;
	string time;
	Status status;
	VType vtype;
	pair<int, int> location;
	pair<int, int> direction;//(1,0) (0,1) (-1,0) (0,-1)
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
#include "EBus.h"
#include "SceneLoader.h"
using namespace Block;

vector<EBusTrack> SceneLoader::GetEBusTrack()
{
	int index_LT = 100;   // sum of Left-Top index
	int index_L = 100;    // x
	int index_T = 100;    // y

	int index_RB=0;       // sum of Right-Bottom index
	int index_R = 100;    // x
	int index_B = 100;    // y

	// The vector of EBus line
	vector <EBusTrack> eBusTrack;

	//GetMatrix();
	// Get the starting point, and ending point
	for (int i = 0; i< (TOP - BOTTOM); i++)
	{
		for (int j = 0; j < (RIGHT - LEFT); j++)
		{
			string BlockName=layoutMatrix[i*width + j + LEFT].GetBlockName().substr(0, 6); //  xcross, tcross, lcross, xcross_greenlight, tcross_greenlight, lcross_greenlight, վ̨�Ǹ�ɶ
			if ("xcross" == BlockName || "tcross" == BlockName || "lcross" == BlockName || "xcross_greenlight" == BlockName || "tcross_greenlight" == BlockName || "lcross_greenlight" == BlockName)
			{
				if ((i+j)<index_LT)
				{
					index_LT = i + j;
					index_L = j + LEFT;
					index_T = i;
				}

				if ((i+j)>index_RB)
				{
					index_RB = i + j;
					index_R = j + LEFT;
					index_B = i;
				}
			}
		}
	}
	// to check whether the EBusLine is empty, if no eBusLine exist, return empty vector
	if (100==index_L || 100==index_R)
	{
		return eBusTrack;
	}
	EBusTrack start, end;
	start.x = index_L;
	start.y = index_T;
	end.x = index_R;
	end.y = index_B;

	middlePoint.x = index_L;
	middlePoint.y = index_B;

	// Add the starting point to the EBus line
	eBusTrack.push_back(start);

	// check the existence of the middle point
	string BlockName = layoutMatrix[middlePoint.y*width + middlePoint.x].GetBlockName().substr(0, 6);

	if ("xcross" == BlockName || "tcross" == BlockName || "lcross" == BlockName)
	{
		// Join the way from the starting point to ending point
		for (int i = start.y + 1; i < end.y; i++)
		{
			EBusTrack temp;
			temp.x = start.x;
			temp.y = i;
			eBusTrack.push_back(temp);
		}
		for (int j = start.x; j < end.x + 1; j++)// +1 to include the ending point
		{
			EBusTrack temp;
			temp.x = j;
			temp.y = end.y;
			eBusTrack.push_back(temp);
		}
	}

	return eBusTrack;

}

void SceneLoader::ShowBusLine(vector<EBusTrack>& eBusTrack)
{
	// By Leong
	/*for (vector<EBusTrack>::iterator it = eBusTrack.begin(); it != eBusTrack.end(); ++it)
	{
		int x = it->x; 
		int y = it->y;
		vector<BlockDef>& blocks = layoutMatrix[y * width + x].GetBlock();
		for (int i = 0; i < blocks.size(); ++i)
		{
			vector<int>& meshIDs = blocks[i].meshID;
			for (int j = 0; j < meshIDs.size(); ++j)
			{ 
				if (meshIDs[j] == bh.meshMap["road1"])
					meshIDs[j] = bh.meshMap["road1red"];
				if (meshIDs[j] == bh.meshMap["road2"])//center
					meshIDs[j] = bh.meshMap["road2red"];
			}
		}
	}*/

	for (int i = 0; i < eBusTrack.size(); ++i)
	{
		int x = eBusTrack[i].x;
		int y = eBusTrack[i].y;
		vector<BlockDef>& blocks = layoutMatrix[y * width + x].GetBlock();
		int orien;
		for (int i = 0; i < blocks.size(); ++i)
		{
			if (blocks[i].name == "street" || blocks[i].name.substr(1, 5) == "cross")
				orien = blocks[i].orientation;
		}

		if (i > 0) //pre exist
		{
			int inOrien;
			int deltaX = eBusTrack[i].x - eBusTrack[i - 1].x;
			int deltaY = eBusTrack[i].y - eBusTrack[i - 1].y;
			if (deltaX == 0 && deltaY == 1)
				inOrien = 1;
			if (deltaX == 1 && deltaY == 0)
				inOrien = 2;
			if (deltaX == 0 && deltaY == -1)
				inOrien = 3;
			if (deltaX == -1 && deltaY == 0)
				inOrien = 4;

			int redIndex;
			if (blocks[i].name.substr(0, 6) == "xcross")
			{
				redIndex = (5 - orien + inOrien - 1) % 4;
			}
			else if (blocks[i].name.substr(0, 6) == "tcross")
			{
				redIndex = (5 - orien + inOrien - 1) % 4;
			}
		}
	}
}

// all global variables in GetBusInfo
double start_time = 0.0;
double time_prior=0.0;   // global time (last call) time_prior
float last_position_x=0.0;
float last_position_y=0.0;
int last_start_x=-1;
int last_start_y=-1;
int last_end_x=-1;
int last_end_y=-1;

int station_flag = 0;   // flag of station
double stop_time = 0.0;  // the start time when entering the station 

// the bus line is empty, return 1
// the bus line changes, return 2
// the bus is in starting point, return 3
// usual case, return 4
// the bus is in station. return 5
int SceneLoader::GetEBusInfo(vector<EBusTrack>& eBusTrack, EBus& ebus, double timeStamp)
{	
	
	vector<EBusTrack> stations;         // vector of all stations
	eBusTrack = GetEBusTrack();  // Get the EBus Line
	if (eBusTrack.empty())  //if the Bus Line exist or not
	{
		start_time = 0.0;    // initialize the global variables
		time_prior = 0.0;   
		last_position_x = 0.0;
		last_position_y = 0.0;
		last_start_x = -1;
		last_start_y = -1;
		last_end_x = -1;
		last_end_y = -1;

		station_flag = 0;   
		stop_time = 0.0;  
		return 1;
	}
	else		// get the position of all bus stations in the BusTrack vector, and store them in a new vector ( vector<EBusTrack> stations ).
	{
		EBusTrack station_tmp;                  // temporary station
		for (vector<EBusTrack>::iterator iter = eBusTrack.begin(); iter != eBusTrack.end(); iter++)
		{
			int row = (*iter).y;
			int column = (*iter).x;
			string BlockName = layoutMatrix[row*width + column].GetBlockName();
			if ("station" == BlockName)
			{
				station_tmp.x = column;
				station_tmp.y = row;
				stations.push_back(station_tmp);
			}
		}
	}
	// stop when the bus is located in the station
	if (station_flag==1 && (timeStamp<(stop_time+3.0)) )      // in station and moving to the right, the stopping time is defined as 3 seconds
	{
		ebus.location.first = last_position_x;
		ebus.location.second = last_position_y;
		ebus.direction.first = 1;                       // Direction
		ebus.direction.second = 0;
		ebus.status = EBus::STOP;  // status
		ebus.speed = 0.0;              // Charging speed
		ebus.vtype = EBus::EBUS;       // vehicle type
		ebus.isShowEnergy = true;      // show energy

		last_position_x = ebus.location.first;
		last_position_y = ebus.location.second;
		time_prior = timeStamp;       // Save the timeStamp of last loop
		return 5;
	}
	else if (station_flag == 2 && (timeStamp<(stop_time + 3.0)))      // in station and moving to the bottom, the stopping time is defined as 3 seconds
	{
		ebus.location.first = last_position_x;
		ebus.location.second = last_position_y;
		ebus.direction.first = 0;                       // Direction
		ebus.direction.second = 1;
		ebus.status = EBus::STOP;  // status
		ebus.speed = 0.0;              // Charging speed
		ebus.vtype = EBus::EBUS;       // vehicle type
		ebus.isShowEnergy = true;      // show energy

		last_position_x = ebus.location.first;
		last_position_y = ebus.location.second;
		time_prior = timeStamp;       // Save the timeStamp of last loop
		return 5;
	}


	if (last_start_x==-1 && last_start_y==-1)  // save the starting and ending point
	{
		last_start_x = eBusTrack.front().x;
		last_start_y = eBusTrack.front().y;
		last_end_x = eBusTrack.back().x;
		last_end_y = eBusTrack.back().y;
	}
	if ((last_start_x == eBusTrack.front().x) && (last_start_y == eBusTrack.front().y) && (last_end_x == eBusTrack.back().x) && (last_end_y == eBusTrack.back().y)) //if the starting and ending point do not change.
	{
		// main part
		if ((time_prior - 0.0) < 1e-8)      // when time_prior=0.0, the bus is located in starting point
		{
			ebus.location.first = eBusTrack.front().x + 12/5;      // Location
			ebus.location.second = eBusTrack.front().y + 1/6;
			ebus.direction.first = 0;                       // Direction
			ebus.direction.second = 1;
			ebus.status = EBus::CHARGING;  // status
			ebus.speed = 0.0;              // Charging speed
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy

			last_position_x = ebus.location.first;
			last_position_y = ebus.location.second;
			start_time = timeStamp;
			time_prior = timeStamp;       // Save the timeStamp of last loop
			return 3;
		}

		if ( timeStamp < (start_time + 5.0) )   // Charging for 5.0 seconds
		{
			ebus.location.first = eBusTrack.front().x + 12 / 5;      // Location
			ebus.location.second = eBusTrack.front().y + 1 / 6;
			ebus.direction.first = 0;                       // Direction
			ebus.direction.second = 1;
			ebus.status = EBus::CHARGING;  // Status
			ebus.speed = 0.0;              // Charging speed
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy

			last_position_x = ebus.location.first;
			last_position_y = ebus.location.second;
			time_prior = timeStamp;       // Save the timeStamp of last loop
			return 3;
		}
		// determine the orientation and move the bus
		if ( (last_position_x - (eBusTrack.front().x+5/12) ) < 1e-8)   // ��
		{
			ebus.direction.first = 0;   //direction
			ebus.direction.second = 1;

			double timeShift = timeStamp - time_prior;
			time_prior = timeStamp;           // Save the timeStamp of last loop
			ebus.speed = 3.0;              // speed
			ebus.location.first = last_position_x;   // location
			ebus.location.second = last_position_y + timeShift*ebus.speed;
			last_position_x = ebus.location.first;
			last_position_y = ebus.location.second;
			ebus.status = EBus::RUNNING;  // Status
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy
			if (ebus.location.second > (eBusTrack.back().y + 1 / 2))  // reach the middle point and change the direction
			{
				ebus.location.first = eBusTrack.front().x + 1 / 2;   // location
				ebus.location.second = eBusTrack.back().y + 7 / 12;
				last_position_x = ebus.location.first;
				last_position_y = ebus.location.second;
				ebus.direction.first = 1;      // direction
				ebus.direction.second = 0;
				ebus.status = EBus::RUNNING;   // Status
				ebus.speed = 3.0;              // speed
				ebus.vtype = EBus::EBUS;       // vehicle type
				ebus.isShowEnergy = true;      // show energy
			}
		}
		if ((last_position_y - (eBusTrack.back().y + 7 / 12)) < 1e-8)   // ��
		{
			ebus.direction.first = 1;   //direction
			ebus.direction.second = 0;

			double timeShift = timeStamp - time_prior;
			time_prior = timeStamp;           // Save the timeStamp of last loop
			ebus.speed = 3.0;              // speed
			ebus.location.first = last_position_x + timeShift*ebus.speed;   // location
			ebus.location.second = last_position_y;
			last_position_x = ebus.location.first;
			last_position_y = ebus.location.second;
			ebus.status = EBus::RUNNING;  // Status
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy
			if ( ebus.location.first > (eBusTrack.back().x + 1/2) )  // reach the ending point and disappear, then restart from the starting point
			{
				ebus.location.first = eBusTrack.back().x + 1 / 2;   // location
				ebus.location.second = eBusTrack.back().y + 7 / 12;
				last_position_x = ebus.location.first;
				last_position_y = ebus.location.second;
				ebus.direction.first = 1;      // direction
				ebus.direction.second = 0;
				ebus.status = EBus::RUNNING;  // Status
				ebus.speed = 3.0;              // speed
				ebus.vtype = EBus::EBUS;       // vehicle type
				ebus.isShowEnergy = true;      // show energy

				// initialize parameters
				start_time = 0.0;
				time_prior = 0.0;   // global time (last call) time_prior
				last_position_x = 0.0;
				last_position_y = 0.0;
				last_start_x = -1;
				last_start_y = -1;
				last_end_x = -1;
				last_end_y = -1;

				station_flag = 0;
				stop_time = 0.0;
			}
		}

	}
	else  //the bus line changes
	{
		// initialize parameters
		start_time = 0.0;
		time_prior = 0.0;   // global time (last call) time_prior
		last_position_x = 0.0;
		last_position_y = 0.0;
		last_start_x = -1;
		last_start_y = -1;
		last_end_x = -1;
		last_end_y = -1;

		station_flag = 0;
		stop_time = 0.0;
		return 2;
	}


	//   Check whether the EBus reach the Station (the position is already obtained)
	for (vector<EBusTrack>::iterator iter_station = stations.begin(); iter_station != stations.end(); iter_station++)
	{
		if (abs(ebus.location.first - ((*iter_station).x + 1 / 2))<0.01)  // �����˶���ʱ�� !!!!!! �������0.01��Ҫ����ʵ��������е�����ֱ������������ֹͣn��Ϊֹ��
		{
			ebus.location.first = (*iter_station).x + 1 / 2;
			ebus.location.second = (*iter_station).y + 7 / 12;
			last_position_x = ebus.location.first;
			last_position_y = ebus.location.second;
			ebus.direction.first = 1;      // direction
			ebus.direction.second = 0;
			ebus.status = EBus::STOP;  // Status
			ebus.speed = 0.0;              // speed
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy

			station_flag = 1;
			start_time = timeStamp;

		}
		else if (abs(ebus.location.second - ((*iter_station).y + 1 / 2))<0.01)  // �����˶���ʱ�� !!!!!! �������0.01��Ҫ����ʵ��������е�����ֱ������������ֹͣn��Ϊֹ��
		{
			ebus.location.first = (*iter_station).x + 5 / 12;
			ebus.location.second = (*iter_station).y + 1 / 2;
			last_position_x = ebus.location.first;
			last_position_y = ebus.location.second;
			ebus.direction.first = 0;      // direction
			ebus.direction.second = 1;
			ebus.status = EBus::STOP;  // Status
			ebus.speed = 0.0;              // speed
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy

			station_flag = 2;
			start_time = timeStamp;
		}
		else  // not in the station
		{
			station_flag = 0;
		}
	}
	return 4;
}

void SceneLoader::GenerateEBus(EBus& ebus)   // Generate Ebus and dynamic energy level
{
	printf("ebus %.2f %.2f\n", ebus.location.first, ebus.location.second);

	// By Leong
	Vector3 trans = Vector3(ebus.location.first, 0, ebus.location.second);

	int orien;
	if (ebus.direction == pair<int, int>(0, 1))
		orien = 1;
	else if (ebus.direction == pair<int, int>(1, 0))
		orien = 2;
	else if (ebus.direction == pair<int, int>(0, -1))
		orien = 3;
	else if (ebus.direction == pair<int, int>(-1, 0))
		orien = 4;
	Quaternion quaternion((orien - 1) * 0.5f * PI, Vector3(0, 1, 0));

	eBusNode->setTranslation(trans);
	eBusNode->setOrientation(quaternion);
}

void SceneLoader::InitEbus()
{
	this->eBusNode = scene->CreateSceneNode("ebusNode");
	this->carRoot->attachNode(eBusNode);
	int size = carMeshes.size();
	MeshPtr mesh = this->carMeshes[size - 1];//default

	eBusNode->setScale(0.01, 0.01, 0.01);
	Entity* entity = this->scene->CreateEntity("ebusEntity");
	eBusNode->attachMovable(entity);
	entity->setMesh(mesh);

}


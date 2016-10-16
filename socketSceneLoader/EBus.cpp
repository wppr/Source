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
			string BlockName=layoutMatrix[i*width + j].GetBlockName(); //  xcross, tcross, lcross, xcross_greenlight, tcross_greenlight, lcross_greenlight, 站台是个啥
			if ("xcross" == BlockName || "tcross" == BlockName || "lcross" == BlockName || "xcross_greenlight" == BlockName || "tcross_greenlight" == BlockName || "lcross_greenlight" == BlockName)
			{
				if ((i+j)<index_LT)
				{
					index_LT = i + j;
					index_L = j;
					index_T = i;
				}

				if ((i+j)>index_RB)
				{
					index_RB = i + j;
					index_R = j;
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
	string BlockName = layoutMatrix[middlePoint.y*(RIGHT - LEFT) + middlePoint.x].GetBlockName();

	if ("xcross" == BlockName || "tcross" == BlockName || "lcross" == BlockName || "xcross_greenlight" == BlockName || "tcross_greenlight" == BlockName || "lcross_greenlight" == BlockName)
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
	// By Wu pengpeng
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

// the bus line is empty, return 1
// the bus line changes, return 2
// the bus is in starting point, return 3
// usual case, return 4
//
int SceneLoader::GetEBusInfo(vector<EBusTrack>& eBusTrack, EBus& ebus, double timeStamp)
{	
	
	vector<EBusTrack> stations;         // vector of all stations
	eBusTrack = GetEBusTrack();  // Get the EBus Line
	if (eBusTrack.empty())  //if the Bus Line exist
	{
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
		if ( (last_position_x - (eBusTrack.front().x+5/12) ) < 1e-8)   // ↓
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
			if (ebus.location.second > (eBusTrack.back().y + 5 / 12))  // reach the middle point and change the direction
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
		if ((last_position_y - (eBusTrack.back().y + 7 / 12)) < 1e-8)   // →
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
			}
		}

	}
	else  //the bus line changes
	{
		last_start_x = -1;    // initialize the busInfo
		last_start_y = -1;
		last_end_x = -1;
		last_end_y = -1;
		time_prior = 0.0;
		return 2;
	}


	// Check whether the EBus reach the Station (the position is already obtained)
	//for (vector<EBusTrack>::iterator iter_station = stations.begin(); iter_station != stations.end(); iter_station++)
	//{
	//	if (abs(ebus.location.first - ((*iter_station).x + 1 / 2))<0.01)  // 向左运动的时候 !!!!!! 这里这个0.01需要根据实际情况进行调整，直到调整到正常停止3s为止。
	//	{
	//		ebus.location.first = (*iter_station).x + 1 / 2;
	//		ebus.location.second = (*iter_station).y + 7 / 12;
	//
	//	}
	//	if (abs(ebus.location.second - ((*iter_station).y + 1 / 2))<0.01)  // 向下运动的时候 !!!!!! 这里这个0.01需要根据实际情况进行调整，直到调整到正常停止3s为止。
	//	{
	//	}
	//	else
	//	{
	//		ebus.status = EBus::RUNNING;
	//		ebus.speed = 3.0;             // Running speed
	//	}
	//}
	return 4;
}

void SceneLoader::GenerateEBus(vector<EBusTrack>& eBusTrack, EBus& ebus)   // Generate Ebus and dynamic energy level
{
	// By Liang yuzhi
}


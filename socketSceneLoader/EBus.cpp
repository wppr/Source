#include "EBus.h"
#include "SceneLoader.h"
using namespace Block;

const float EBus::ENERGYREDUCESPEED = 1/8.0;
const float EBus::CHARGINGSPEED = 1.0;
const float EBus::BATTERYCAPACITY = 5.0;

vector<EBusTrack> SceneLoader::GetEBusTrack_Fixed()
{
	vector <EBusTrack> eBusTrack;
	EBusTrack track1(3, 0);
	EBusTrack track2(3,1);
	EBusTrack track3(3, 2);
	EBusTrack track4(3, 3);
	EBusTrack track5(4, 3);
	EBusTrack track6(5, 3);
	EBusTrack track7(5, 4);
	EBusTrack track8(5, 5);
	EBusTrack track9(6, 5);
	EBusTrack track10(7, 5);
	EBusTrack track11(8, 5);
	EBusTrack track12(9, 5);
	EBusTrack track13(9, 4);
	EBusTrack track14(9, 3);
	EBusTrack track15(9, 2);
	EBusTrack track16(9, 1);
	EBusTrack track17(9, 0);
	EBusTrack track18(10, 0);
	EBusTrack track19(11, 0);
	EBusTrack track20(12, 0);
	EBusTrack track21(12, 1);
	EBusTrack track22(12, 2);
	EBusTrack track23(12, 3);
	EBusTrack track24(12, 4);
	EBusTrack track25(12, 5);
	EBusTrack track26(12, 6);
	EBusTrack track27(12, 7);

	eBusTrack.push_back(track1);
	eBusTrack.push_back(track2);
	eBusTrack.push_back(track3);
	eBusTrack.push_back(track4);
	eBusTrack.push_back(track5);
	eBusTrack.push_back(track6);
	eBusTrack.push_back(track7);
	eBusTrack.push_back(track8);
	eBusTrack.push_back(track9);
	eBusTrack.push_back(track10);
	eBusTrack.push_back(track11);
	eBusTrack.push_back(track12);
	eBusTrack.push_back(track13);
	eBusTrack.push_back(track14);
	eBusTrack.push_back(track15);
	eBusTrack.push_back(track16);
	eBusTrack.push_back(track17);
	eBusTrack.push_back(track18);
	eBusTrack.push_back(track19);
	eBusTrack.push_back(track20);
	eBusTrack.push_back(track21);
	eBusTrack.push_back(track22);
	eBusTrack.push_back(track23);
	eBusTrack.push_back(track24);
	eBusTrack.push_back(track25);
	eBusTrack.push_back(track26);
	eBusTrack.push_back(track27);

	return eBusTrack;

}
vector<EBusTrack> SceneLoader::GetEBusTrack_Unfixed()
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
			string BlockName=layoutMatrix[i*width + j + LEFT].GetBlockName().substr(0, 6); //  xcross, tcross, lcross, xcross_greenlight, tcross_greenlight, lcross_greenlight, 站台是个啥
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

	if (!wayGenFlag) return;

	for (int i = 0; i < eBusTrack.size(); ++i)
	{
		int x = eBusTrack[i].x;
		int y = eBusTrack[i].y;
		vector<BlockDef>& blocks = layoutMatrix[y * width + x].GetBlock();
		int orien;
		string name;
		for (int j = 0; j < blocks.size(); ++j)
		{
			if (blocks[j].name == "street" || blocks[j].name.substr(1, 5) == "cross")
			{
				orien = blocks[j].orientation;
				name = blocks[j].name;
			}
		}

		//in
		if (i > 0) //pre exist
		{
			int inOrien;
			int deltaX = eBusTrack[i].x - eBusTrack[i - 1].x;
			int deltaY = eBusTrack[i].y - eBusTrack[i - 1].y;
			assert(deltaX != 0 || deltaY != 0);
			if (deltaX == 0 && deltaY == 1)
				inOrien = 1;
			if (deltaX == 1 && deltaY == 0)
				inOrien = 2;
			if (deltaX == 0 && deltaY == -1)
				inOrien = 3;
			if (deltaX == -1 && deltaY == 0)
				inOrien = 4;

			int redIndex;
			if (name.substr(0, 6) == "xcross")
			{
				redIndex = (5 - orien + inOrien - 1) % 4 + 1;
			}
			else if (name.substr(0, 6) == "tcross")
			{
				redIndex = (5 - inOrien + orien - 1) % 4 + 1;
			}
			else if (name.substr(0, 6) == "lcross")
			{
				redIndex = (3 + inOrien - orien) % 4;
			}

			for (int j = 0; j < blocks.size(); ++j)
			{
				vector<int>& meshIDs = blocks[j].meshID;
				if (blocks[j].name.substr(1, 5) == "cross")
				{
					if (redIndex < meshIDs.size() && meshIDs[redIndex] == bh.meshMap["road1"])
						meshIDs[redIndex] = bh.meshMap["road1red"];
					if (redIndex < meshIDs.size() && meshIDs[redIndex] == bh.meshMap["road2"])//center
						meshIDs[redIndex] = bh.meshMap["road2red"];
				}
				else if (blocks[j].name == "street")
					meshIDs[0] = bh.meshMap["road2red"];
			}
		}

		//center
		for (int j = 0; j < blocks.size(); ++j)
		{
			vector<int>& meshIDs = blocks[j].meshID;
			if (blocks[j].name.substr(1, 5) == "cross")
			{
				if (meshIDs[0] == bh.meshMap["road1"])
					meshIDs[0] = bh.meshMap["road1red"];
				if (meshIDs[0] == bh.meshMap["road2"])//center
					meshIDs[0] = bh.meshMap["road2red"];
			}
			else if(blocks[j].name == "street")
				meshIDs[1] = bh.meshMap["road2red"];
		}

		//out
		if (i < eBusTrack.size() - 1) //post exist
		{
			int inOrien;
			int deltaX = eBusTrack[i + 1].x - eBusTrack[i].x;
			int deltaY = eBusTrack[i + 1].y - eBusTrack[i].y;
			if (deltaX == 0 && deltaY == 1)
				inOrien = 3;
			if (deltaX == 1 && deltaY == 0)
				inOrien = 4;
			if (deltaX == 0 && deltaY == -1)
				inOrien = 1;
			if (deltaX == -1 && deltaY == 0)
				inOrien = 2;

			int redIndex;
			if (name.substr(0, 6) == "xcross")
			{
				redIndex = (5 - orien + inOrien - 1) % 4 + 1;
			}
			else if (name.substr(0, 6) == "tcross")
			{
				redIndex = (5 - inOrien + orien - 1) % 4 + 1;
			}
			else if (name.substr(0, 6) == "lcross")
			{
				redIndex = (3 + inOrien - orien) % 4;
			}

			for (int j = 0; j < blocks.size(); ++j)
			{
				vector<int>& meshIDs = blocks[j].meshID;
				if (blocks[j].name.substr(1, 5) == "cross")
				{
					vector<int>& meshIDs = blocks[j].meshID;
					if (redIndex < meshIDs.size() && meshIDs[redIndex] == bh.meshMap["road1"])
						meshIDs[redIndex] = bh.meshMap["road1red"];
					if (redIndex < meshIDs.size() &&  meshIDs[redIndex] == bh.meshMap["road2"])//center
						meshIDs[redIndex] = bh.meshMap["road2red"];
				}
				else if (blocks[j].name == "street")
					meshIDs[2] = bh.meshMap["road2red"];
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

int stage = 0;
double timeShift = 0.0;


void EBus::run(EBus& ebus,double timeStamp)
{
	ebus.lastLocation = ebus.location;
	switch (ebus.runningStage)
	{
	case 1:   // stage 1

		ebus.direction.first = 0;   //direction
		ebus.direction.second = 1;

		timeShift = timeStamp - ebus.lastRunTime;
		ebus.lastRunTime = timeStamp;           // Save the timeStamp of last loop
	//	if (timeShift > 0.1) return;
		ebus.speed = 1.0;              // speed

		ebus.location.first = ebus.lastLocation.first;   // location
		ebus.location.second = ebus.lastLocation.second + timeShift*ebus.speed;

		ebus.status = EBus::RUNNING;  // Status
		ebus.vtype = EBus::EBUS;       // vehicle type
		ebus.isShowEnergy = true;      // show energy
		if (ebus.location.second > (3 + 1 / 2.0))
		{
			ebus.direction.first = 1;   //direction
			ebus.direction.second = 0;
			ebus.location.first = 3 + 1 / 2.0;
			ebus.location.second = 3 + 7 / 12.0;

			ebus.speed = 3.0;              // speed
			ebus.status = EBus::RUNNING;  // Status
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy
			ebus.runningStage = 2;
		}
		break;
	case 2:  // stage 2
		ebus.direction.first = 1;   //direction
		ebus.direction.second = 0;
		timeShift = timeStamp - ebus.lastRunTime;
		ebus.lastRunTime = timeStamp;           // Save the timeStamp of last loop
		ebus.speed = 1.0;              // speed

		ebus.location.first = ebus.lastLocation.first + timeShift*ebus.speed;   // location
		ebus.location.second = ebus.lastLocation.second;

		ebus.status = EBus::RUNNING;  // Status
		ebus.vtype = EBus::EBUS;       // vehicle type
		ebus.isShowEnergy = true;      // show energy
		if (ebus.location.first > (5 + 1 / 2.0 ))
		{
			ebus.direction.first = 0;   //direction
			ebus.direction.second = 1;
			ebus.location.first = 5 + 5 / 12.0 +1/24.0;
			ebus.location.second = 3 + 1 / 2.0;

			ebus.speed = 3.0;              // speed

			ebus.status = EBus::RUNNING;  // Status
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy
			ebus.runningStage = 3;
		}
		break;
	case 3:  // stage 3
		ebus.direction.first = 0;   //direction
		ebus.direction.second = 1;
		timeShift = timeStamp - ebus.lastRunTime;
		ebus.lastRunTime = timeStamp;           // Save the timeStamp of last loop
		ebus.speed = 1.0;              // speed

		ebus.location.first = ebus.lastLocation.first;   // location
		ebus.location.second = ebus.lastLocation.second + timeShift*ebus.speed;

		ebus.status = EBus::RUNNING;  // Status
		ebus.vtype = EBus::EBUS;       // vehicle type
		ebus.isShowEnergy = true;      // show energy
		if (ebus.location.second > (5 + 1 / 2.0))
		{
			ebus.direction.first = 1;   //direction
			ebus.direction.second = 0;
			ebus.location.first = 5 + 1 / 2.0 ;
			ebus.location.second = 5 + 7 / 12.0;

			ebus.speed = 1.0;              // speed

			ebus.status = EBus::RUNNING;  // Status
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy
			ebus.runningStage = 4;
		}
		break;
	case 4:  // stage 4
		ebus.direction.first = 1;   //direction
		ebus.direction.second = 0;
		timeShift = timeStamp - ebus.lastRunTime;
		ebus.lastRunTime = timeStamp;           // Save the timeStamp of last loop
		ebus.speed = 1.0;              // speed
		ebus.location.first = ebus.lastLocation.first + timeShift*ebus.speed;   // location
		ebus.location.second = ebus.lastLocation.second;

		ebus.status = EBus::RUNNING;  // Status
		ebus.vtype = EBus::EBUS;       // vehicle type
		ebus.isShowEnergy = true;      // show energy
		if (ebus.location.first > (9 + 1 / 2.0 ))
		{
			ebus.direction.first = 0;   //direction
			ebus.direction.second = -1;
			ebus.location.first = 9 + 7 / 12.0 -1/24.0;
			ebus.location.second = 5 + 1 / 2.0;

			ebus.speed = 1.0;              // speed

			ebus.status = EBus::RUNNING;  // Status
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy
			ebus.runningStage = 5;
		}
		break;
	case 5:  // stage 5
		ebus.direction.first = 0;   //direction
		ebus.direction.second = -1;
		timeShift = timeStamp - ebus.lastRunTime;
		ebus.lastRunTime = timeStamp;           // Save the timeStamp of last loop
		ebus.speed = 1.0;              // speed

		ebus.location.first = ebus.lastLocation.first;   // location
		ebus.location.second = ebus.lastLocation.second - timeShift*ebus.speed;

		ebus.status = EBus::RUNNING;  // Status
		ebus.vtype = EBus::EBUS;       // vehicle type
		ebus.isShowEnergy = true;      // show energy

		if (ebus.location.second < (0 + 1 / 2.0))
		{
			ebus.direction.first = 1;   //direction
			ebus.direction.second = 0;
			ebus.location.first = 9 + 1 / 2.0 ;
			ebus.location.second = 0 + 7 / 12.0;

			ebus.speed = 1.0;              // speed

			ebus.status = EBus::RUNNING;  // Status
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy
			ebus.runningStage = 6;
		}
		break;
	case 6:  // stage 6
		ebus.direction.first = 1;   //direction
		ebus.direction.second = 0;
		timeShift = timeStamp - ebus.lastRunTime;
		ebus.lastRunTime = timeStamp;           // Save the timeStamp of last loop
		ebus.speed = 1.0;              // speed
		ebus.location.first = ebus.lastLocation.first + timeShift*ebus.speed;   // location
		ebus.location.second = ebus.lastLocation.second;

		ebus.status = EBus::RUNNING;  // Status
		ebus.vtype = EBus::EBUS;       // vehicle type
		ebus.isShowEnergy = true;      // show energy
		if (ebus.location.first> (12 + 1 / 2.0 ))
		{
			ebus.direction.first = 0;   //direction
			ebus.direction.second = 1;
			ebus.location.first = 12 + 5 / 12.0 +1/24.0;
			ebus.location.second = 0 + 1 / 2.0;

			ebus.speed = 1.0;              // speed
			ebus.status = EBus::RUNNING;  // Status
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy
			ebus.runningStage = 7;
		}
		break;
	case 7:  // stage 7
		ebus.direction.first = 0;   //direction
		ebus.direction.second = 1;
		timeShift = timeStamp - ebus.lastRunTime;
		ebus.lastRunTime = timeStamp;           // Save the timeStamp of last loop
		ebus.speed = 1.0;              // speed

		ebus.location.first = ebus.lastLocation.first;   // location
		ebus.location.second = ebus.lastLocation.second + timeShift*ebus.speed;

		ebus.status = EBus::RUNNING;  // Status
		ebus.vtype = EBus::EBUS;       // vehicle type
		ebus.isShowEnergy = true;      // show energy
		if (ebus.location.second > (7 + 1 / 2.0))
		{
			ebus.direction.first = 0;   //direction
			ebus.direction.second = -1;
			ebus.location.first = 12 + 5 / 12.0 +1/6.0 -1/24.0;
			ebus.location.second = 7 + 5 / 6.0;

			ebus.speed = 1.0;              // speed

			ebus.status = EBus::RUNNING;  // Status
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy
			ebus.runningStage = 8;
			//time_prior = 0.0;
			//ebus.runningStage = 1;
			//ebus.reset(timeStamp);
		}
		break;
	case 8:  // stage 8: reverse stage 1
		ebus.direction.first = 0;   //direction
		ebus.direction.second = -1;
		timeShift = timeStamp - ebus.lastRunTime;
		ebus.lastRunTime = timeStamp;           // Save the timeStamp of last loop
		ebus.speed = 1.0;              // speed

		ebus.location.first = ebus.lastLocation.first;   // location
		ebus.location.second = ebus.lastLocation.second - timeShift*ebus.speed;

		ebus.status = EBus::RUNNING;  // Status
		ebus.vtype = EBus::EBUS;       // vehicle type
		ebus.isShowEnergy = true;      // show energy
		if (ebus.location.second < (0 + 1 / 2.0))
		{
			ebus.direction.first = -1;   //direction
			ebus.direction.second = 0;
			ebus.location.first = 12 + 1/2.0;
			ebus.location.second = 0+5/12.0+1/24.0;

			ebus.speed = 1.0;              // speed

			ebus.status = EBus::RUNNING;  // Status
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy

										   //time_prior = 0.0;
										   //ebus.runningStage = 1;
			ebus.runningStage = 9;
			//ebus.reset(timeStamp);
		}
		break;
	case 9:  // stage 9: reverse stage 2
		ebus.direction.first = -1;   //direction
		ebus.direction.second = 0;
		timeShift = timeStamp - ebus.lastRunTime;
		ebus.lastRunTime = timeStamp;           // Save the timeStamp of last loop
		ebus.speed = 1.0;              // speed

		ebus.location.first = ebus.lastLocation.first - timeShift*ebus.speed;   // location
		ebus.location.second = ebus.lastLocation.second;

		ebus.status = EBus::RUNNING;  // Status
		ebus.vtype = EBus::EBUS;       // vehicle type
		ebus.isShowEnergy = true;      // show energy
		if (ebus.location.first < (9 + 1 / 2.0))
		{
			ebus.direction.first = 0;   //direction
			ebus.direction.second = 1;
			ebus.location.first = 9 + 5 / 12.0+1/24.0;
			ebus.location.second = 0 + 1/2.0;

			ebus.speed = 1.0;              // speed

			ebus.status = EBus::RUNNING;  // Status
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy

										   //time_prior = 0.0;
										   //ebus.runningStage = 1;
			ebus.runningStage = 10;
			//ebus.reset(timeStamp);
		}
		break;
	case 10:  // stage 10: reverse stage 3
		ebus.direction.first = 0;   //direction
		ebus.direction.second = 1;
		timeShift = timeStamp - ebus.lastRunTime;
		ebus.lastRunTime = timeStamp;           // Save the timeStamp of last loop
		ebus.speed = 1.0;              // speed

		ebus.location.first = ebus.lastLocation.first ;   // location
		ebus.location.second = ebus.lastLocation.second + timeShift*ebus.speed;

		ebus.status = EBus::RUNNING;  // Status
		ebus.vtype = EBus::EBUS;       // vehicle type
		ebus.isShowEnergy = true;      // show energy
		if (ebus.location.second > (5 + 1 / 2.0))
		{
			ebus.direction.first = -1;   //direction
			ebus.direction.second = 0;
			ebus.location.first = 9+1/2.0;
			ebus.location.second = 5+5/12.0;

			ebus.speed = 1.0;              // speed

			ebus.status = EBus::RUNNING;  // Status
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy

										   //time_prior = 0.0;
										   //ebus.runningStage = 1;
			ebus.runningStage = 11;
			//ebus.reset(timeStamp);
		}
		break;
	case 11:  // stage 11: reverse stage 4
		ebus.direction.first = -1;   //direction
		ebus.direction.second = 0;
		timeShift = timeStamp - ebus.lastRunTime;
		ebus.lastRunTime = timeStamp;           // Save the timeStamp of last loop
		ebus.speed = 1.0;              // speed

		ebus.location.first = ebus.lastLocation.first - timeShift*ebus.speed;   // location
		ebus.location.second = ebus.lastLocation.second ;

		ebus.status = EBus::RUNNING;  // Status
		ebus.vtype = EBus::EBUS;       // vehicle type
		ebus.isShowEnergy = true;      // show energy
		if (ebus.location.first < (5 + 1 / 2.0))
		{
			ebus.direction.first = 0;   //direction
			ebus.direction.second = -1;
			ebus.location.first = 5 + 7 / 12.0 -1/24.0;
			ebus.location.second = 5 + 1 / 2.0;

			ebus.speed = 1.0;              // speed

			ebus.status = EBus::RUNNING;  // Status
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy

										   //time_prior = 0.0;
										   //ebus.runningStage = 1;
			ebus.runningStage = 12;
			//ebus.reset(timeStamp);
		}
		break;
	case 12:  // stage 12: reverse stage 5
		ebus.direction.first = 0;   //direction
		ebus.direction.second = -1;
		timeShift = timeStamp - ebus.lastRunTime;
		ebus.lastRunTime = timeStamp;           // Save the timeStamp of last loop
		ebus.speed = 1.0;              // speed

		ebus.location.first = ebus.lastLocation.first ;   // location
		ebus.location.second = ebus.lastLocation.second - timeShift*ebus.speed;

		ebus.status = EBus::RUNNING;  // Status
		ebus.vtype = EBus::EBUS;       // vehicle type
		ebus.isShowEnergy = true;      // show energy
		if (ebus.location.second < (3 + 1 / 2.0))
		{
			ebus.direction.first = -1;   //direction
			ebus.direction.second = 0;
			ebus.location.first = 5 + 1/2.0;
			ebus.location.second = 3+ 5/12.0;

			ebus.speed = 1.0;              // speed

			ebus.status = EBus::RUNNING;  // Status
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy

										   //time_prior = 0.0;
										   //ebus.runningStage = 1;
			ebus.runningStage = 13;
			//ebus.reset(timeStamp);
		}
		break;
	case 13:  // stage 13: reverse stage 6
		ebus.direction.first = -1;   //direction
		ebus.direction.second = 0;
		timeShift = timeStamp - ebus.lastRunTime;
		ebus.lastRunTime = timeStamp;           // Save the timeStamp of last loop
		ebus.speed = 1.0;              // speed

		ebus.location.first = ebus.lastLocation.first - timeShift*ebus.speed;   // location
		ebus.location.second = ebus.lastLocation.second ;

		ebus.status = EBus::RUNNING;  // Status
		ebus.vtype = EBus::EBUS;       // vehicle type
		ebus.isShowEnergy = true;      // show energy
		if (ebus.location.first < (3 + 1 / 2.0))
		{
			ebus.direction.first = 0;   //direction
			ebus.direction.second = -1;
			ebus.location.first = 3+ 7/12.0-1/24.0;
			ebus.location.second = 3 + 1/2.0;

			ebus.speed = 1.0;              // speed

			ebus.status = EBus::RUNNING;  // Status
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy

										   //time_prior = 0.0;
										   //ebus.runningStage = 1;
			ebus.runningStage = 14;
			//ebus.reset(timeStamp);
		}
		break;
	case 14:  // stage 14: reverse stage 7
		ebus.direction.first = 0;   //direction
		ebus.direction.second = -1;
		timeShift = timeStamp - ebus.lastRunTime;
		ebus.lastRunTime = timeStamp;           // Save the timeStamp of last loop
		ebus.speed = 1.0;              // speed

		ebus.location.first = ebus.lastLocation.first ;   // location
		ebus.location.second = ebus.lastLocation.second - timeShift*ebus.speed;

		ebus.status = EBus::RUNNING;  // Status
		ebus.vtype = EBus::EBUS;       // vehicle type
		ebus.isShowEnergy = true;      // show energy
		if (ebus.location.second < (0 + 1 / 6.0))
		{
			ebus.direction.first = 0;   //direction
			ebus.direction.second = 1;
			//ebus.location.first = 3 + 7 / 12.0 - 1 / 24.0;
			//ebus.location.second = 3 + 1 / 2.0;

			ebus.speed = 1.0;              // speed

			ebus.status = EBus::RUNNING;  // Status
			ebus.vtype = EBus::EBUS;       // vehicle type
			ebus.isShowEnergy = true;      // show energy

										   //time_prior = 0.0;
										   //ebus.runningStage = 1;
			//ebus.runningStage = 13;
			ebus.reset(timeStamp);
		}
		break;
	}

}

int meetTheLine(float startPoint, float endPoint, float point)
{
	point += 0.5;
	if (startPoint <= point && point <= endPoint) return 1;
	if (endPoint <= point && point <= startPoint) return 1;
	return 0;
}

int EBus::meet(vector<EBusTrack>& stations)
{
	static float delta = 1 / 11.0;
	for (vector<EBusTrack>::iterator iter = stations.begin(); iter != stations.end(); iter++)
	{
		if (fabs(lastLocation.first-iter->x-0.5) <= delta && meetTheLine(lastLocation.second, location.second, iter->y)) return 1;
		if (fabs(lastLocation.second-iter->y-0.5) <=delta && meetTheLine(lastLocation.first, location.first, iter->x)) return 1;
	}
	return 0;
}

int SceneLoader::GetEBusInfo_Fixed(vector<EBusTrack>& eBusTrack, EBus& ebus, double timeStamp)
{
	vector<EBusTrack> stations; // vector of all stations
	vector<EBusTrack> chargingStations;
	eBusTrack = GetEBusTrack_Fixed();  // Get the EBus Line
    // get the position of all bus stations in the BusTrack vector, and store them in a new vector ( vector<EBusTrack> stations ).
	for (vector<EBusTrack>::iterator iter = eBusTrack.begin(); iter != eBusTrack.end(); iter++)
	{
		EBusTrack station_tmp;                  // temporary station
		int row = (*iter).y;
		int column = (*iter).x;
		station_tmp.x = column;
		station_tmp.y = row;
		string BlockName = layoutMatrix[row*width + column].GetBlockName();
		if ("station" == BlockName)
			stations.push_back(station_tmp);
		 else if("newChargingStation" == BlockName)
			chargingStations.push_back(station_tmp);
	}
	
	//printf("energy level = %f\n",ebus.energyLevel);
	switch (ebus.status)
	{
	case  EBus::CHARGING :
		if (ebus.energyLevel < 5)
		{
			ebus.energyLevel += (timeStamp - ebus.lastRunTime)*EBus::CHARGINGSPEED;
			if (ebus.energyLevel > 5) ebus.energyLevel = 5.0;
			//ebus.location = ebus.lastLocation;
			//ebus.speed = 0.0;
		}
		else
		{
			ebus.status = EBus::RUNNING;
			ebus.speed = 1.0;
			EBus::run(ebus, timeStamp);
		}
		break;
	case EBus::RUNNING :
		ebus.energyLevel -= (timeStamp - ebus.lastRunTime)*EBus::ENERGYREDUCESPEED;
		EBus::run(ebus,timeStamp);
		if (ebus.meet(stations))
		{
			ebus.status = EBus::STOP;
			ebus.speed = 0.0;
			ebus.startStopTime = timeStamp;
		}
		else if(ebus.meet(chargingStations)){
			ebus.status = EBus::CHARGING;
			ebus.speed = 0.0;
//			ebus.startStopTime = timeStamp;
		}
		break;
	case EBus::STOP:
		ebus.energyLevel -= (timeStamp - ebus.lastRunTime)*EBus::ENERGYREDUCESPEED;
		if (timeStamp <= ebus.startStopTime + 1.0)
		{
			ebus.location = ebus.lastLocation;
			ebus.speed = 0.0;
		}
		else {
			ebus.status = EBus::RUNNING;
			ebus.speed = 1.0;
			EBus::run(ebus, timeStamp);
		}
		break;
	default:
		break;
	}
	ebus.lastRunTime = timeStamp;
	if (ebus.energyLevel < 0) ebus.energyLevel = 0;
	return 0;
}



// the bus line is empty, return 1
// the bus line changes, return 2
// the bus is in starting point, return 3
// usual case, return 4
// the bus is in station. return 5
int SceneLoader::GetEBusInfo_Unfixed(vector<EBusTrack>& eBusTrack, EBus& ebus, double timeStamp)
{	
	
	vector<EBusTrack> stations;         // vector of all stations
	eBusTrack = GetEBusTrack_Fixed();  // Get the EBus Line
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
		if (abs(ebus.location.first - ((*iter_station).x + 1 / 2))<0.01)  // 向右运动的时候 !!!!!! 这里这个0.01需要根据实际情况进行调整，直到调整到正常停止n秒为止。
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
		else if (abs(ebus.location.second - ((*iter_station).y + 1 / 2))<0.01)  // 向下运动的时候 !!!!!! 这里这个0.01需要根据实际情况进行调整，直到调整到正常停止n秒为止。
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
	if (!busGenFlag)
		carRoot->detachNode(eBusNode);
	else
		carRoot->attachNode(eBusNode);
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
	busNode->setOrientation(quaternion);

	float energyPercentage = ebus.energyLevel / 5.0f;
	int energyLevel = energyPercentage * 7.0f;
	if (energyLevel >= 7) energyLevel = 6;//max 6
	if (energyLevel < 0) energyLevel = 0;//least 0
	batteryEntity->setMesh(batteries[energyLevel]);
	//printf("bat %s\n", batteries[energyLevel]->getName());
}

void SceneLoader::InitEbus(double timeStamp)
{
	//printf("ebus init\n");
	this->eBusNode = scene->CreateSceneNode("ebusNode");
	this->carRoot->attachNode(eBusNode);
	int size = carMeshes.size();
	MeshPtr mesh = this->carMeshes[size - 1];//default

	busNode = scene->CreateSceneNode("busNode");
	Entity* entity = this->scene->CreateEntity("ebusEntity");
	busNode->attachMovable(entity);
	entity->setMesh(mesh);
	busNode->setScale(0.00005, 0.00005, 0.00005);

	eBusNode->attachNode(busNode);

	eBatteryNode = scene->CreateSceneNode("battery");
	batteryEntity = this->scene->CreateEntity("batteryEntity");
	eBatteryNode->attachMovable(batteryEntity);
	batteryEntity->setMesh(batteries[6]);
	eBatteryNode->setScale(0.4, 0.4, 0.4);
	eBatteryNode->setTranslation(0, 0.3, 0);
	Quaternion quaternion(0.5f * PI, Vector3(0, 1, 0));
	eBatteryNode->setOrientation(quaternion);

	eBusNode->attachNode(eBatteryNode);

	//this->ebus = EBus(timeStamp);
}


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

	GetMatrix();
	// Get the starting point, and ending point
	for (int i = 0; i< (TOP - BOTTOM); i++)
	{
		for (int j = 0; j < (RIGHT - LEFT); j++)
		{
			string BlockName=layoutMatrix[i*(RIGHT - LEFT) + j].GetBlockName(); //  xcross, tcross, lcross, xcross_greenlight, tcross_greenlight, lcross_greenlight, վ̨�Ǹ�ɶ
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
	EBusTrack start, end, middle;
	start.x = index_L;
	start.y = index_T;
	end.x = index_R;
	end.y = index_B;

	middle.x = index_L;
	middle.y = index_B;

	// Add the starting point to the EBus line
	eBusTrack.push_back(start);

	// check the existence of the middle point
	string BlockName = layoutMatrix[middle.y*(RIGHT - LEFT) + middle.x].GetBlockName();

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

void SceneLoader::GetEBusInfo(vector<EBusTrack>& eBusTrack, EBus& ebus)
{

}

void SceneLoader::GenerateEBus(vector<EBusTrack>& eBusTrack, EBus& ebus)   // Generate Ebus and dynamic energy level
{

}


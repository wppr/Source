#pragma once
#include <vector>
using namespace std;

struct  InputLayout_Desc
{
	string semantic;
	string name;
	int index;
	string type;
};

class as_InputLayout{
public:

	vector<InputLayout_Desc*> getElements(){
		return mAttributes;
	}
	void clear(){
		mAttributes.clear();
	}
	void add(InputLayout_Desc* desc){
		mAttributes.push_back(desc);
	}

	void getAttributeDataType(InputLayout_Desc* att, string& datatype, int& size)
	{
		if (att->type == "vec2"){
			size = 2;
			datatype = "float";
			return;
		}
		if (att->type == "vec3"){
			size = 3;
			datatype = "float";
			return;
		}
		if (att->type == "vec4"){
			size = 4;
			datatype = "float";
			return;
		}
		if (att->type == "ivec4"){
			size = 4;
			datatype = "int";
			return;
		}
	}
private:
	vector<InputLayout_Desc*> mAttributes;
};
#pragma once
#include "PreDefine.h"
#include <map>
#include "MyString.h"
using namespace std;
using namespace HW;

class GLProgramDataSetter{
public:
	void SetProgram(GLGpuProgram* prog){ program = prog; }
	 
	//¸üÐÂprogramÖÐuniform
	void SetProgramTexture(const string &name, Texture* texture, int index);
	void UpdateGlobalVariable(const map<unsigned int, string> &uniformMap);
	void updateProgramData(const map<string, ProgramData> & program_data);
	void UpdateConstantVariable(const String &name, string datatype,unsigned int size, void* val);

	GLGpuProgram* program;
};
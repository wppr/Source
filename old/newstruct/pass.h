#pragma once
#include "../RenderState.h"
#include "../rapidjson/document.h"
#include "gl_program.h"
#include <map>
#include <set>
#include "engine_struct.h"
#include "as_InputLayout.h"
#include "ResourceManager.h"
#include "PreDefine.h"
using namespace rapidjson;
using namespace HW;
using namespace std;



class as_Pass
{
public:
	as_Pass()
	{
		mProgram = (NULL);
		mRasterState = RasterState::getDefault();
		mBlendState = BlendState::getDefault();
		mDepthStencilState = DepthStencilState::getDefault();
		mClearState = ClearState::getDefault();
		mInputLayout = new as_InputLayout;
	}


	~as_Pass(){}

	RasterState 			mRasterState;
	BlendState  			mBlendState;
	DepthStencilState 		mDepthStencilState;
	ClearState				mClearState;
	GpuProgram*			mProgram;

	string					name;

	//input uniform
	map<unsigned int, string> mUniformMap;
	set<string>					mManualData;
	as_InputLayout*				mInputLayout;
private:
	void validateUniformMap();
};



void ParseAttributeFromJson(rapidjson::Value& attributes, as_InputLayout* mInputLayout);

void ParseUniformInputFromJson(rapidjson::Value& uniformmap, as_Pass* pass);

as_Pass* ParsePassFromJsonFile(string filename);



class PassManager :public as_ResourceManager<as_Pass*>{
public:
	static PassManager& getInstance()
	{
		static PassManager    instance;
		return instance;
	}
	
	as_Pass* LoadPass(string path);
	
	map<string, unsigned int> m_StringSemanticMap;
private:

	PassManager(){
		initStringSemanticMap();
	}
	void initStringSemanticMap();
	PassManager(PassManager const&);              // Don't Implement.
	void operator=(PassManager const&); // Don't implement
};
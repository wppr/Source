#define  _WINSOCKAPI_
//#include "App_SSS.h"
//#include "app_PBR.h"
#include "app_edushi_VR.h"
#include "app_VR.h"
#include "GLFWFramework.h"
//#include "edushiKey.h"
//#include "connection.h"
//#pragma comment(lib,"LibConnection.lib")
//#include "socketSceneLoader/SceneLoader.h"

std::function< void (int,int)> f;

void main() {
	GLFWFrameWork fw;

	auto app4 = new App_edushi_VR;
	//auto app4 = new App_VR;
	fw.app = app4;
	
	fw.Init();
	//edushi_key ek;
	//ek.app = app4;
	//f = std::bind(&edushi_key::SwitchLogCam, ek, std::placeholders::_1, std::placeholders::_2);
	//InteractionControler::KeyCallBacks.push_back(f);

	fw.Loop();

	return;
}
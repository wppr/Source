//#pragma once
//
//#include "Engine/FrameListener.h"
//#include "Engine/SceneNode.h"
//#include "Engine/AppFrameWork.h"
////#include "Engine/DynamicWeather.h"
////#include "UI/UISceneManager.h"
//#ifdef _WIN32
//#include <windows.h>
//#include <conio.h>
//#endif
//#include "SSSPipeline.h"
////#include "PCFramework.h"
//#include "Logger.h"
//namespace HW
//{
//	class FrameListenerDebug : public FrameListener
//	{
//	private:
//		Camera * _cam;
////		AndroidTouch * _save;
//		SceneNode * _root;
//		float scale_factor;
//		unsigned int _width;
//		unsigned int _height;
//		SceneManager* _msceneMgr;
//		bool cam_log_state;
//		Pipeline* p_pipeline;
//		float speed1 = 0.03;
//		float speed2 = 0.1;
//		float speed3 = 1;
//		//UISceneManager* _uisceneMgr;
//		//AppFrameWork* m_pInstance;
//		//bool _rotate_by_up ;
//	public:
//		FrameListenerDebug(Camera * cam, SceneNode * root,SceneManager* msceneMgr, unsigned int height, unsigned int width, void* ins)
//		{
//			_cam = cam;
//			//_save = save;
//			_width = width;
//			_height = height;
//			_root = root;
//			cam_log_state = true;
//			_msceneMgr=msceneMgr;
//			p_pipeline = (Pipeline*)ins;
//			//_uisceneMgr=UISceneManager::GetInstance();			
//			//	_rotate_by_up = true;
//			//m_pInstance = ins;
//		}
//		bool frameStarted()
//		{
//			if (_root == NULL)
//				return false;
//#ifdef ANDROID
//			if (_save == NULL)
//				return false;
//			TouchActionParam & touch = _save->getActionParam();
//			switch (touch.gesture)
//			{
//			case GS_None:
//				//LOGE("Gs_None");
//				break;
//			case GS_Pan:
//			{
//						   //LOGE("Gs_Pan");
//						   float move_x = touch.move_vec.x / _width;
//						   float move_y = touch.move_vec.y / _height;
//
//						   Vector3 dir = _cam->getDirection();
//						   Vector3 up = _cam->getUp();
//						   dir.normalize();
//						   Vector3 right = dir.crossProduct(up);
//						   Quaternion q(move_x * M_PI_2, up);
//						   dir = q * dir;
//						   q = Quaternion(move_y * M_PI_2, right);
//						   dir = q * dir;
//						   _cam->setDirection(dir);
//
//			}
//
//				break;
//			case GS_Tap:
//				//LOGE("Gs_Tap");
//				break;
//			case GS_Press:
//				//LOGE("Gs_Press");
//				break;
//			case GS_Zoom:
//			{
//
//							float zoom_factor = touch.zoom_factor;
//
//							float old_len = touch.old_length;
//							float new_len = touch.new_length;
//
//							Vector3 pos = _cam->getPosition();
//							Vector3 dir = _cam->getDirection();
//							dir.normalize();
//							pos += (new_len - old_len) * dir;
//							//LOGE("Gs_Zoom %f - %f = %f",new_len,old_len,new_len- old_len);
//						//	if (pos.length() >= 10000.0)
//								_cam->setPosition(pos);
//							
//
//			}
//
//				break;
//				//	case GS_Flick:
//				//		LOGE("Gs_Flick");
//				//		break;
//			case GS_Rotate:
//			{
//							  //LOGE("Gs_Rotate angle = %f",touch.rotate_angle);
//							  Vector3 up = _cam->getUp();
//							  Vector3 dir = _cam->getDirection();
//
//							  float angle = touch.rotate_angle;
//							  Quaternion q(angle, dir);
//							  up = q * up;
//							  _cam->setUp(up);
//			}
//
//				break;
//			default:
//				//LOGE("Gs_default");
//				break;
//			}
//			//TouchActionParam & touch = _save->getActionParam();
//			//Event e=_uisceneMgr->GenAndroidEvent(touch);
//			//_uisceneMgr->addEvent(e);
//			//_uisceneMgr->MainLoop();
//
//#else
//			if (GetKeyState ('C') & 0x8000) {
//				//m_pInstance->acceleration.y += 0.05f;
//			}
//			else if (GetKeyState('V') & 0x8000)
//			{
//				//m_pInstance->acceleration.y -= 0.05f;
//			}
//			else {
//				//m_pInstance->acceleration.y = 0.0f;
//
//				if (GetKeyState('P') & 0x8000)
//				{
//					Logger::WriteLog("CAMERA POSITION: <%.2f, %.2f, %.2f>", _cam->getPosition().x, _cam->getPosition().y, _cam->getPosition().z);
//				}
//
//				if (GetKeyState('O') & 0x8000)
//				{
//					Logger::WriteLog("CAMERA DIRECTION: <%.2f, %.2f, %.2f>", _cam->getDirection().x, _cam->getDirection().y, _cam->getDirection().z);
//				}
//
//
//
//				if ((GetKeyState(VK_RIGHT) & 0x8000))
//				{
//					float move_x = -10.0 / _width;
//					Vector3 dir = _cam->getDirection();
//					Vector3 up = _cam->getUp();
//					dir.normalize();
//					Vector3 right = dir.crossProduct(up);
//					Quaternion q(move_x * M_PI_2, up);
//					dir = q * dir;
//					_cam->setDirection(dir);
//					_cam->setUp(Vector3(0, 1, 0));
//				}
//
//				if ((GetKeyState(VK_LEFT) & 0x8000))
//				{
//					float move_y = 10.0 / _width;
//					Vector3 dir = _cam->getDirection();
//					Vector3 up = _cam->getUp();
//					dir.normalize();
//					Vector3 right = dir.crossProduct(up);
//					Quaternion q = Quaternion(move_y * M_PI_2, up);
//					dir = q * dir;
//					_cam->setDirection(dir);
//					_cam->setUp(Vector3(0, 1, 0));
//				}
//
//
//				if (GetKeyState(VK_UP) & 0x80000)
//				{
//					float move_y = 10.0 / _height;
//					Vector3 dir = _cam->getDirection();
//					Vector3 up = _cam->getUp();
//					dir.normalize();
//					Vector3 right = dir.crossProduct(up);
//
//					Quaternion q = Quaternion(move_y * M_PI_2, right);
//					dir = q * dir;
//					_cam->setDirection(dir);
//					_cam->setUp(Vector3(0, 1, 0));
//				}
//
//				if (GetKeyState(VK_DOWN) & 0x80000)
//				{
//					float move_y = -10.0 / _height;
//					Vector3 dir = _cam->getDirection();
//					Vector3 up = _cam->getUp();
//					dir.normalize();
//					Vector3 right = dir.crossProduct(up);
//
//					Quaternion q = Quaternion(move_y * M_PI_2, right);
//					dir = q * dir;
//					_cam->setDirection(dir);
//					_cam->setUp(Vector3(0, 1, 0));
//				}
//
//				if (GetKeyState('W') & 0x8000)
//				{
//					Vector3 pos = _cam->getPosition();
//					Vector3 dir = _cam->getDirection();
//
//					dir.normalize();
//					if (GetKeyState(VK_SHIFT) & 0x8000)
//						pos += (speed3) * dir;
//					else
//						if (GetKeyState('F') & 0x8000)
//							pos += (speed2) * dir;
//						else
//							pos += (speed1) * dir;
//					//if (pos.length() >= 1000.0)
//					_cam->setPosition(pos);
//
//				}
//
//				if (GetKeyState('S') & 0x8000)
//				{
//					Vector3 pos = _cam->getPosition();
//					Vector3 dir = _cam->getDirection();
//					dir.normalize();
//					if (GetKeyState(VK_SHIFT) & 0x8000)
//						pos -= (speed3) * dir;
//					else
//						if (GetKeyState('F') & 0x8000)
//							pos -= (speed2) * dir;
//						else
//						pos -= (speed1) * dir;
//					//	if (pos.length() >= 1000.0)
//					_cam->setPosition(pos);
//				}
//
//				if (GetKeyState('Q') & 0x8000)
//				{
//					Vector3 up = _cam->getUp();
//					Vector3 dir = _cam->getDirection();
//
//					float angle = 5.0 / 360.0;
//					Quaternion q(angle, dir);
//					up = q * up;
//					_cam->setUp(up);
//				}
//
//				if (GetKeyState('E') & 0x8000)
//				{
//					Vector3 up = _cam->getUp();
//					Vector3 dir = _cam->getDirection();
//
//					float angle = -5.0 / 360.0;
//					Quaternion q(angle, dir);
//					up = q * up;
//					_cam->setUp(up);
//				}
//				if (GetKeyState('G') & 0x8000)
//				{
//					LogCameraPosition();
//					
//				}
//				if (GetKeyState('H') & 0x8000)
//				{
//					cam_log_state = true;
//				}
//				if (true)
//				{
//					static bool LastPressed = false;
//					bool keyPressed= GetKeyState('R') & 0x8000;
//					
//					if (LastPressed == false&&keyPressed) {
//						auto p = dynamic_cast<SSSPipeline*>(p_pipeline);
//						p->RefreshConfigs();
//					}
//					LastPressed = keyPressed;
//					
//				}
//				//if (GetKeyState('3') & 0x8000)
//				//{
//				//	POINT p;
//				//	tagMSG msg;
//				//	GetMessage(&msg, NULL, 0, 0);
//				//	/*if(msg.message==WM_LBUTTONDBLCLK) {*/
//				//	GetCursorPos(&p);
//				//	Event e;
//				//	e.type = Event::TAP;
//				//	e.pos2 = Vector2(p.x, p.y);
//				//	_uisceneMgr->addEvent(e);
//				//	_uisceneMgr->LoopEvent();
//				//	//}
//				//}
//				
//				//if (GetKeyState('5') & 0x80)
//				//{
//				//	auto p=dynamic_cast<TestPipeline*>(m_pInstance->sysentry->m_pipeline);
//				//	p->e_pbr.metalness *= 1.03;
//				//	Logger::WriteLog("metalness %f ", p->e_pbr.metalness);
//				//}
//				//if (GetKeyState('6') & 0x80)
//				//{
//				//	auto p = dynamic_cast<TestPipeline*>(m_pInstance->sysentry->m_pipeline);
//				//	p->e_pbr.metalness *= 0.97;
//				//	Logger::WriteLog("metalness %f ", p->e_pbr.metalness);
//				//}
//				//if (GetKeyState('7') & 0x80)
//				//{
//				//	auto p = dynamic_cast<TestPipeline*>(m_pInstance->sysentry->m_pipeline);
//				//	p->e_pbr.roughness *= 1.03;
//				//	Logger::WriteLog("roughness %f ", p->e_pbr.roughness);
//				//}
//				//if (GetKeyState('8') & 0x80)
//				//{
//				//	auto p = dynamic_cast<TestPipeline*>(m_pInstance->sysentry->m_pipeline);
//				//	p->e_pbr.roughness *= 0.97;
//				//	Logger::WriteLog("roughness %f ", p->e_pbr.roughness);
//				//}
//			}
//#endif // ANDROID
//			
//			return true;
//		}
//		void LogCameraPosition() {
//			
//			if (cam_log_state)
//			{
//				auto p = _cam->getPosition();
//				Logger::WriteLog("CAMERA POSITION: %.2f %.2f %.2f", p.x, p.y, p.z);
//				cam_log_state = false;
//			}
//		}
//		bool frameEnded()
//		{
//			//if (_save == NULL)
//			//{
//			//	return false;
//			//	//	LOGE("return false");
//			//}
//			return true;
//
//			//TouchActionParam & touch = _save->getActionParam();
//			//touch.gesture = GS_None;
//			//if (_save->event != NULL)
//			//{
//			//	LOGE(" event ");
//
//			//	if (AInputEvent_getType(_save->event) == AINPUT_EVENT_TYPE_MOTION)
//			//	{
//			//		LOGE(" type ");
//			//		_save->x = AMotionEvent_getX(_save->event, 0);
//			//		_save->y = AMotionEvent_getY(_save->event, 0);
//			//	}
//
//			//}
//			//_save->input_type = 0;
//			//_save->event = NULL;
//		}
//	};
//}
//

#ifndef ANDROID_INPUT_H
#define ANDROID_INPUT_H


#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <sys/time.h>
#include "Engine/mathlib.h"
#include "Engine/AppFrameWork.h"
#include "Engine/AndroidTouch.h"


using namespace HW;

class AndroidInput
{
public:
	static int32_t process_input(struct android_app * app, AInputEvent * event)
	{
		
		static int active_pointer_id = -1;
		static int now_x =0 ,now_y =0;
		int action = AMotionEvent_getAction(event);
		int pointer_index = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
			>> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
		HW::AppFrameWork * application = (HW::AppFrameWork *)app->userData;
		HW::AndroidTouch * android_touch = application->android_touch;
		unsigned int pointer_id = 0 ;
		if(android_touch == NULL)
			return 0;
		if (AINPUT_EVENT_TYPE_MOTION == AInputEvent_getType(event))
		{
			switch(action & AMOTION_EVENT_ACTION_MASK)
			{
			case AMOTION_EVENT_ACTION_DOWN:
			case AMOTION_EVENT_ACTION_POINTER_DOWN:
				{
					pointer_id = AMotionEvent_getPointerId(event,pointer_index);
					Vector2 point;
					point.x = AMotionEvent_getX(event,pointer_index);
					point.y = AMotionEvent_getY(event,pointer_index);
					//LOGE("action down %d %f,%f",pointer_id,point.x,point.y);
					android_touch->OnPointerDown(point,pointer_id);	
				}break;
			case  AMOTION_EVENT_ACTION_UP:
			case  AMOTION_EVENT_ACTION_POINTER_UP:
				{
					pointer_id = AMotionEvent_getPointerId(event,pointer_index);
					Vector2 point;
					point.x = AMotionEvent_getX(event,pointer_index);
					point.y = AMotionEvent_getY(event,pointer_index);
					//LOGE("action up %d,%f,%f",pointer_id,point.x,point.y);
					android_touch->OnPointerUp(point,pointer_id);
				}break;
			case AMOTION_EVENT_ACTION_MOVE:
				{
					Vector2 point;
					for(unsigned int  i = 0; i <  AMotionEvent_getPointerCount(event);i++)
					{
						pointer_id = AMotionEvent_getPointerId(event,i);
						point.x = AMotionEvent_getX(event,i);
						point.y = AMotionEvent_getY(event,i);
						android_touch->OnPointerMove(point,pointer_id);
					//	LOGE("action move %d %f,%f",pointer_id,point.x,point.y);
					}
				}break;
			}
				
			android_touch->updateInput();
			return 1;
		}
		else if(AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY )
		{
				LOGE("key down!!");		
				return 0;
		}
	}
};
#endif

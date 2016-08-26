#ifndef HW_INPUTTOUCH_H
#define HW_INPUTTOUCH_H
#include "mathlib.h"
//use a state machine like klayge

#ifdef ANDROID
#include <android/log.h>
#endif
namespace HW
{
	enum Gesture
	{
		GS_None =0 ,
		GS_Pan,
		GS_Tap,
		GS_Press,
		GS_Zoom,
		GS_Rotate,
		GS_Num
	};

	struct TouchActionParam
	{
		Gesture gesture;
		Vector2 touch_center;
		Vector2 move_vec;
		float zoom_factor;
		float old_length;
		float new_length;
		float rotate_angle;

	};

	class InputTouch
	{
	public:
		InputTouch();
		Gesture gesture()
		{
			return m_gesture;
		}

		TouchActionParam & getActionParam()
		{
			return m_touch_action_param;
		}

		void ClearAction()
		{
			m_system_message_processed = false;
			if(m_touch_action_param.gesture != GS_None)
			{
					switch (m_touch_action_param.gesture)
				{
				case GS_Tap:
					m_touch_action_param.gesture = m_gesture = GS_None;
					break;
				case GS_Pan:
					m_touch_action_param.gesture = m_gesture = GS_Pan;
					m_touch_action_param.move_vec = Vector2(0, 0);
					break;
				case GS_Press:
					m_touch_action_param.gesture = m_gesture = GS_Press;
					break;
				case GS_Zoom:
				case GS_Rotate:
					m_two_finger_start_vec = m_two_finger_vec;
					m_two_finger_start_length = m_two_finger_length;
					m_touch_action_param.zoom_factor = 1.0;
					m_touch_action_param.old_length = m_touch_action_param.new_length;
					m_touch_action_param.rotate_angle = 0;
					break;
				default:
					break;
				}
			//	m_touch_action_param.gesture = m_gesture = GS_None;
			//	m_two_finger_start_vec = m_two_finger_vec;
			//	m_two_finger_start_length = m_two_finger_length;
			}
			
			//try not give the lastest vec value
			//m_two_finger_length = 0.0;
			//m_two_finger_vec = Vector2(0,0);
		}

		virtual ~InputTouch()
		{

		}

		virtual void everyFrameProcessed();
	protected:
		float TAP_TIMEOUT,PRESS_TIMEOUT,Pan_dist;

		enum GestureState
		{
			State_None = 0,
			State_Tap,
			State_Pan,
			State_Press,
			State_TwoFinger,
			State_DontHandle,
			State_Num
		};
		

		void GestureStateNone(float elapsed_time);
		void GestureStatePan(float elapsed_time);
		void GestureStateTap(float elapsed_time);
		void GestureStatePress(float elapsed_time);
		void GestureStateTwoFinger(float elapsed_time);
		void GestureStateDontHandle(float elapsed_time);


		void CurrState(GestureState state);


		Gesture m_gesture;
		GestureState m_current_state;
		void (InputTouch::*m_gesture_func[State_Num])(float);
		void (InputTouch::*m_current_gesture_func)(float);
		bool m_index;
		int m_touch_count;

		TouchActionParam m_touch_action_param;

		Vector2 m_touch_positions[2][4] ;
		bool  m_touch_downs[2][4];	

		float m_one_finger_timer;
		Vector2 m_one_finger_start_pos;

		float m_two_finger_timer;
		float m_two_finger_start_length;
		float m_two_finger_length;
		Vector2 m_two_finger_start_vec;
		Vector2 m_two_finger_vec;


		bool m_system_message_processed;
		
	};
}

#endif
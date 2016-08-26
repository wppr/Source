#include "InputTouch.h"
#include "Logger.h"
namespace HW
{
	InputTouch::InputTouch() :m_system_message_processed(false)
	{
		m_index = true;
		m_gesture = GS_None;
		m_current_state = State_None;
		m_gesture_func[State_None] = &InputTouch::GestureStateNone;
		m_gesture_func[State_Pan] = &InputTouch::GestureStatePan;
		m_gesture_func[State_Tap] = &InputTouch::GestureStateTap;
		m_gesture_func[State_Press] = &InputTouch::GestureStatePress;
		m_gesture_func[State_DontHandle] = &InputTouch::GestureStateDontHandle;
		m_gesture_func[State_TwoFinger] = &InputTouch::GestureStateTwoFinger;
		
		m_current_gesture_func = &InputTouch::GestureStateNone;
		TAP_TIMEOUT = 180;
		PRESS_TIMEOUT = 250;
		Pan_dist = 25;
	}

	void InputTouch::CurrState(GestureState state)
	{
		m_current_gesture_func = m_gesture_func[state];
		m_current_state = state;
#ifdef ANDROID
		switch(state)
		{
		case State_None:
			LOGE("State_None");
			break;
		case State_Pan:
			LOGE("State_Pan");
			break;
		case  State_Tap:
			LOGE("State_Tap");
			break;
		case State_TwoFinger:
			LOGE("State_TwoFinger");
			break;
		case State_Press:
			Logger::WriteAndroidInfoLog("State_Press");
			break;
		case State_DontHandle:
			Logger::WriteAndroidInfoLog("State_DontHandle");
			break;
		//case State_Zoom:
		//	LOGE("State_Zoom");
		//	break;
		//case State_Rotate:
		//	LOGE("State_Rotate");
		//	break;
		 default:
			LOGE("State_Default");
			break;
		}
#endif
	}

	void InputTouch::GestureStateNone(float elapsed_time)
	{

		//Logger::WriteAndroidInfoLog("GestureStateNone num_touch = %d,elapsed_time = %f",(int)m_touch_count,elapsed_time);

	
		switch(m_touch_count)
		{
		case 1:
			{
				
				if(m_touch_downs[m_index][0])
				{
					this->CurrState(State_Tap);
					m_one_finger_start_pos = m_touch_positions[m_index][0];
					m_one_finger_timer = 0;
				}
				else if(m_touch_downs[m_index][1])
				{
					this->CurrState(State_Tap);
					m_one_finger_start_pos = m_touch_positions[m_index][1];
					m_one_finger_timer = 0;
				}
			}break;
		case 2:
			{
				if(m_touch_downs[m_index][0] && m_touch_downs[m_index][1])
				{
					// different with klay ,temporaily has no intention to support press and tap
					this->CurrState(State_TwoFinger);
					m_two_finger_timer= 0;
					m_two_finger_start_vec = m_touch_positions[m_index][0] - m_touch_positions[m_index][1];
					m_two_finger_start_length = m_two_finger_start_vec.normalize();

				}
			}break;
		default: break;
		}
	}

//	void InputTouch::GestureStatePan(float elapsed_time)
//	{
//#ifdef ANDROID
//		LOGE("GestureStatePan num_touch = %d ,elapsed_time = %f",m_touch_count,elapsed_time);
//#endif
//		switch(m_touch_count)
//		{
//		case 0:
//			this->CurrState(State_None);
//			break;
//		}
//		m_gesture = GS_Pan;
//		Vector2 move_vec;
//		if(m_touch_downs[m_index][0] && m_touch_downs[!m_index][0])
//		{
//			 move_vec = m_touch_positions[m_index][0] - m_touch_positions[!m_index][0];
//		}
//		else
//		{
//			move_vec = Vector2(0,0);
//		}
//#ifdef ANDROID
//		LOGE("Gs_Pan move_vec = (%f,%f)",move_vec.x,move_vec.y);
//#endif
//		m_touch_action_param.gesture = m_gesture;
//		m_touch_action_param.move_vec = move_vec;
//		m_touch_action_param.touch_center = m_touch_positions[m_index][0];
//	} 

	void InputTouch::GestureStateTap(float elapsed_time)
	{

		//Logger::WriteAndroidInfoLog("GestureStateTap num_touch = %d ,elapsed_time = %f",(int)m_touch_count,elapsed_time);

		//Logger::WriteAndroidInfoLog(" e time %f",elapsed_time);
		switch(m_touch_count)
		{
		case 0:
			{
			
				m_touch_action_param.gesture = m_gesture = GS_Tap;
				m_touch_action_param.touch_center = m_touch_positions[m_index][0];
				this->CurrState(State_None);
			}break;
		case 2:
			{
				if(m_touch_downs[m_index][0] && m_touch_downs[m_index][1])
				{
					// different with klay ,temporaily has no intention to support press and tap
					this->CurrState(State_TwoFinger);
					m_two_finger_timer= 0;
					m_two_finger_start_vec = m_touch_positions[m_index][0] - m_touch_positions[m_index][1];
					m_two_finger_start_length = m_two_finger_start_vec.normalize();
					
				}
			}break;
		case 1:
			{

				m_one_finger_timer += elapsed_time;
				float dist ;
				unsigned int down_index = 0;
				if(m_touch_downs[m_index][0])
					down_index =0;
				else
					down_index =1;
				dist = (m_touch_positions[m_index][down_index] -m_one_finger_start_pos).length();
				//Logger::WriteAndroidInfoLog("time %f ,dist %f",m_one_finger_timer,dist);
				if(m_one_finger_timer > PRESS_TIMEOUT && dist < Pan_dist )
				{
					m_gesture = GS_Press;
					m_touch_action_param.gesture = m_gesture;
					m_touch_action_param.touch_center = m_touch_positions[m_index][down_index];
					this->CurrState(State_Press);
				}
				else 
				{
				//	if(m_gesture == GS_Pan)
				//	{
				//		//m_touch_action_param.gesture = m_gesture = GS_Pan;
				//		m_touch_action_param.touch_center = m_touch_positions[m_index][down_index];
				//		m_touch_action_param.move_vec += m_touch_positions[m_index][down_index] - m_one_finger_start_pos;
				//		m_one_finger_start_pos = m_touch_positions[m_index][down_index];
				//	}
				//	else
					if (dist>Pan_dist&&m_one_finger_timer>TAP_TIMEOUT)
					{
						m_touch_action_param.gesture = m_gesture = GS_Pan;
						m_touch_action_param.touch_center = m_touch_positions[m_index][down_index];
						m_touch_action_param.move_vec = m_touch_positions[m_index][down_index] - m_one_finger_start_pos;
						m_one_finger_start_pos = m_touch_positions[m_index][down_index];
						this->CurrState(State_Pan);

					}
					else
					{
						m_touch_action_param.gesture = m_gesture = GS_None;
					}
				}

			}break;
		}
	/*	m_one_finger_timer += elapsed_time;
		float dist = (m_touch_positions[m_index][0] -m_one_finger_start_pos).length();
		if((m_touch_count == 1) &&(m_one_finger_timer > 200) && dist < 5)
		{
			m_gesture = GS_Press;
			m_touch_action_param.gesture= m_gesture;
			m_touch_action_param.touch_center = m_touch_positions[m_index][0];
		}
		else if(dist > 5)
		{
			this->CurrState(State_Pan);
		}*/
	}
	/*
	void InputTouch::GestureStateTwoFinger(float elapsed_time)
	{

		//Logger::WriteAndroidInfoLog("GestureStateTwoFinger num_touch = %d,elapsed_time = %f",(int)m_touch_count,elapsed_time);

		switch(m_touch_count)
		{
		case 0:
		case 1:
			{
				if(m_gesture != Gs_Rotate && m_gesture != GS_Zoom)
				{
					m_touch_action_param.gesture= m_gesture = GS_None;
					// maybe has some bug 
				}
				this->CurrState(State_None);
			}break;
		
			//means 0.2 second
		//	if(m_gesture != Gs_Rotate && m_gesture != GS_Zoom)
		//	{
		//		m_touch_action_param.gesture= m_gesture = GS_None;
				// maybe has some bug 
		//		m_touch_action_param.touch_center = m_touch_positions[m_index][0];
		//	}
		//	this->CurrState(State_Tap);
		//	break;
		case 2:
			{
				// klay distinguish between zoom and rotate ,we didnt process rotate for temporarily 
				m_two_finger_vec = m_touch_positions[m_index][0] - m_touch_positions[m_index][1];
				m_two_finger_length = m_two_finger_vec.normalize();
				//Vector2 old_vec = m_two_finger_vec;
			//	old_vec.normalize();
				float d = m_two_finger_start_vec.dotProduct(m_two_finger_vec);
#ifdef ANDROID
		//		LOGE("GestureStateTwoFinger new_vec(%f,%f) = %f,old_vec(%f,%f) = %f,%f",m_two_finger_vec.x,m_two_finger_vec.y,m_two_finger_length,m_two_finger_start_vec.x,m_two_finger_start_vec.y,m_two_finger_start_length,d);
#endif
				if(abs(d) > 0.995)
				{
					if(abs(m_two_finger_length - m_two_finger_start_length) > 10)
					{
#ifdef ANDROID
		//				LOGE("Gesturezoom length  = %f",m_two_finger_start_length - m_two_finger_start_length);
#endif
						m_touch_action_param.old_length = m_two_finger_start_length;
						m_touch_action_param.new_length = m_two_finger_length;
						m_touch_action_param.touch_center = (m_touch_positions[m_index][0] + m_touch_positions[m_index][1])/2.0;
					//	if(m_gesture == GS_Zoom || m_gesture == Gs_Rotate )
					//	{
							
					//		m_two_finger_start_length = vec_len;
					//		m_two_finger_vec = m_touch_positions[m_index][0] - m_touch_positions[m_index][1];
					//	}
						m_touch_action_param.gesture = m_gesture = GS_Zoom;
						
					}
					else
						m_touch_action_param.gesture = m_gesture = GS_None;
						
				}
				else
				{
					float rotate_angle = -asin(m_two_finger_start_vec.crossProduct(m_two_finger_vec));
#ifdef ANDROID
		//			LOGE("GestureStateRotate rotate_angle = %f",rotate_angle);
#endif
					if(abs(rotate_angle) > 0.01)
					{
						m_touch_action_param.touch_center = (m_touch_positions[m_index][0] +m_touch_positions[m_index][1])/2.0;
						m_touch_action_param.rotate_angle = rotate_angle;
					//	if(m_gesture != GS_Zoom && m_gesture != Gs_Rotate )
					//	{
					//		m_two_finger_vec = m_touch_positions[m_index][0] - m_touch_positions[m_index][1];
					//		m_two_finger_start_length = m_two_finger_vec.length();
					//	}
						m_touch_action_param.gesture = m_gesture = Gs_Rotate;
						
					}
					else
					{
						m_touch_action_param.gesture = m_gesture = GS_None;
					}

				}
			}break;
		}
		m_two_finger_timer += elapsed_time;
	}
	*/
	/*void InputTouch::GestureStateZoom(float elapsed_time)
	{
#ifdef ANDROID
		LOGE("GestureStateZoom num_touch = %d,elapsed_time = %f",m_touch_count,elapsed_time);
#endif
		switch(m_touch_count)
		{
		case 0:
		case 1:
			this->CurrState(State_None);
			break;
		case 2:
			{
				float vec_len = (m_touch_positions[m_index][0]- m_touch_positions[m_index][1]).length();
				m_gesture = GS_Zoom;
#ifdef ANDROID
				LOGE("Gs_Zoom,start_lenght = %f,new_length= %f",m_two_finger_start_length,vec_len);
#endif
				m_touch_action_param.gesture = m_gesture;
				m_touch_action_param.touch_center = (m_touch_positions[m_index][0] + m_touch_positions[m_index][1])/2.0;
				m_touch_action_param.zoom_factor = vec_len/m_two_finger_start_length;
				m_touch_action_param.old_length = m_two_finger_start_length;
				m_touch_action_param.new_length = vec_len;
				m_two_finger_start_length =vec_len;
				
			}
			break;
		}

		m_two_finger_timer += elapsed_time;
				
	}*/

	
	void InputTouch::GestureStateTwoFinger(float elaped_time)
	{

		switch(m_touch_count)
		{

		case 0:
			{
				m_touch_action_param.gesture = m_gesture = GS_None;
				this->CurrState(State_None);
			}
			break;
		case 1:
			{
				m_touch_action_param.gesture = m_gesture = GS_None;
				this->CurrState(State_DontHandle);
			}
			break;
		case 2:
			{
				m_two_finger_vec = m_touch_positions[m_index][0] - m_touch_positions[m_index][1];
				m_two_finger_length = m_two_finger_vec.normalize();
				float d = m_two_finger_start_vec.dotProduct(m_two_finger_vec);

				if(abs(d) > 0.995)
				{
					
					{
						m_touch_action_param.old_length = m_two_finger_start_length;
						m_touch_action_param.new_length = m_two_finger_length;
						m_touch_action_param.touch_center = (m_touch_positions[m_index][0] + m_touch_positions[m_index][1])/2.0;
						m_touch_action_param.gesture = m_gesture = GS_Zoom;
					}

				}
				else
				{
					float rotate_angle = -asin(m_two_finger_start_vec.crossProduct(m_two_finger_vec));
					m_touch_action_param.touch_center = (m_touch_positions[m_index][0] +m_touch_positions[m_index][1])/2.0;
					m_touch_action_param.rotate_angle = rotate_angle;
					m_touch_action_param.gesture = m_gesture = GS_Rotate;
				}

			}break;
			
		

		}
	}

	void InputTouch::GestureStatePan(float elaped_time)
	{
		//Logger::WriteAndroidInfoLog("GestureStatePan num_touch = %d,elapsed_time = %f ",(int)m_touch_count,elaped_time);
		switch (m_touch_count)
		{
		case 0:
		{
				   m_touch_action_param.gesture = m_gesture = GS_None;
				   this->CurrState(State_None);
		}
			break;
		case 1:
		{

				  unsigned int down_index = 0;
				  if (m_touch_downs[m_index][0])
					  down_index = 0;
				  else
					  down_index = 1;
				  if (m_gesture == GS_Pan)
				  {
					  m_touch_action_param.touch_center = m_touch_positions[m_index][down_index];
					  m_touch_action_param.move_vec += m_touch_positions[m_index][down_index] - m_one_finger_start_pos;
					  m_one_finger_start_pos = m_touch_positions[m_index][down_index];
				  }
				  else
				  {
					  m_touch_action_param.gesture = m_gesture = GS_Pan;
					  m_touch_action_param.touch_center = m_touch_positions[m_index][down_index];
					  m_touch_action_param.move_vec = m_touch_positions[m_index][down_index] - m_one_finger_start_pos;
					  m_one_finger_start_pos = m_touch_positions[m_index][down_index];
				  }

		}
			break;
		case 2:
		{
				  m_touch_action_param.gesture = m_gesture = GS_None;
				  if(m_touch_downs[m_index][0] && m_touch_downs[m_index][1])
				  {
					  // different with klay ,temporaily has no intention to support press and tap
					  this->CurrState(State_TwoFinger);
					  m_two_finger_timer= 0;
					  m_two_finger_start_vec = m_touch_positions[m_index][0] - m_touch_positions[m_index][1];
					  m_two_finger_start_length = m_two_finger_start_vec.normalize();

				  }
		}
			break;
		
		}
	}

	void InputTouch::GestureStatePress(float elapsed_time)
	{
		//Logger::WriteAndroidInfoLog("GestureStatePress num_touch = %d,elapsed_time = %f ",(int)m_touch_count,elapsed_time);
		switch (m_touch_count)
		{
		case 0:
		{
				  m_touch_action_param.gesture = m_gesture = GS_None;
				  this->CurrState(State_None);
		}
			break;
		case 1:
		{
				  unsigned int down_index = 0;
				  if (m_touch_downs[m_index][0])
					  down_index = 0;
				  else
					  down_index = 1;
				  float dist = (m_touch_positions[m_index][down_index] - m_one_finger_start_pos).length();
				  if (dist < 15)
				  {
					  m_gesture = GS_Press;
					  m_touch_action_param.gesture = m_gesture;
					  m_touch_action_param.touch_center = m_touch_positions[m_index][down_index];
				  }
				  else
				  {
					  m_touch_action_param.gesture = m_gesture = GS_Pan;
					  m_touch_action_param.touch_center = m_touch_positions[m_index][down_index];
					  m_touch_action_param.move_vec = m_touch_positions[m_index][down_index] - m_one_finger_start_pos;
					  m_one_finger_start_pos = m_touch_positions[m_index][down_index];
					  this->CurrState(State_Pan);
				  }
		}
			break;
		case 2:
		{
				  m_touch_action_param.gesture = m_gesture = GS_None;
				  if(m_touch_downs[m_index][0] && m_touch_downs[m_index][1])
				  {
					  // different with klay ,temporaily has no intention to support press and tap
					  this->CurrState(State_TwoFinger);
					  m_two_finger_timer= 0;
					  m_two_finger_start_vec = m_touch_positions[m_index][0] - m_touch_positions[m_index][1];
					  m_two_finger_start_length = m_two_finger_start_vec.normalize();

				  }

		}
			break;
		}
	}

	void InputTouch::GestureStateDontHandle(float elapsed_time)
	{
		//Logger::WriteAndroidInfoLog("GestureStateDontHanle num_touch = %d,elapsed_time = %f ",(int)m_touch_count,elapsed_time);

		switch (m_touch_count)
		{
		case 0:
		{
				  m_touch_action_param.gesture = m_gesture = GS_None;
				  this->CurrState(State_None);
		}break;
		default:
			break;
		}
	}

	void InputTouch::everyFrameProcessed()
	{

	}

	/*void InputTouch::GestureStateRotate(float elapsed_time)
	{
		LOGE("GestureStateRotate num_touch = %d,elapsed_time = %f",m_touch_count,elapsed_time);
		switch(m_touch_count)
		{
		case 0:
		case 1:
			this->CurrState(State_None);
			break;
		case 2:
			{
				Vector2 new_vec= (m_touch_positions[m_index][0] -m_touch_positions[m_index][1]);
				new_vec.normalize();
				Vector2 old_vec = m_two_finger_vec;
				old_vec.normalize();
				float rotate_angle = atan2(old_vec.y,old_vec.x) - atan2(new_vec.y,new_vec.x);
				LOGE("GestureStateRotate rotate_angle = %f",rotate_angle);
				if(abs(rotate_angle) > 0.01)
				{
					m_two_finger_vec = m_touch_positions[m_index][0] - m_touch_positions[m_index][1];
					m_two_finger_start_length = m_two_finger_vec.length();
					m_touch_action_param.gesture = m_gesture = Gs_Rotate;
					m_touch_action_param.touch_center = (m_touch_positions[m_index][0] +m_touch_positions[m_index][1])/2.0;
					m_touch_action_param.rotate_angle = rotate_angle;
				}
				
				m_two_finger_timer += elapsed_time;
			}break;
		}
	}*/
}
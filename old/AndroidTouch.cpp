#include "AndroidTouch.h"
#include "Logger.h"
namespace HW
{
	AndroidTouch::AndroidTouch()
	{
		for(unsigned int  i = 0 ;i < 4;i++)
		{
			m_touch_pos[i] = Vector2(0,0);
			m_touch_down[i] = false;
		}
	}

	void AndroidTouch::OnPointerDown(Vector2 const & point,unsigned int pointer_id)
	{
		m_touch_pos[pointer_id] = point;
		m_touch_down[pointer_id] = true;
	}

	void AndroidTouch::OnPointerMove(Vector2 const & point,unsigned int pointer_id)
	{
		m_touch_pos[pointer_id] = point;
		m_touch_down[pointer_id] = true;
	}

	void AndroidTouch::OnPointerUp(Vector2 const & point,unsigned int pointer_id )
	{
		m_touch_pos[pointer_id] = point;
		m_touch_down[pointer_id] = false;
	}

	void AndroidTouch::updateInput()
	{
#ifdef ANDROID
		LOGE("update input");
#endif
		m_index = !m_index;
		m_touch_count = 0;
		for(unsigned int i = 0; i < 4 ;i++)
		{
			
			m_touch_positions[m_index][i] = m_touch_pos[i];
			m_touch_downs[m_index][i] = m_touch_down[i];
			if(m_touch_down[i])
				m_touch_count++;
			//m_touch_down[i] = false;
			//m_touch_pos[i] = Vector2(0,0);
		}
		m_system_message_processed = true;

		//m_touch_action_param.gesture = m_gesture = GS_None;
		(this->*m_current_gesture_func)(timer.elapsed2());
		timer.restart();
		
	}

	void AndroidTouch::everyFrameProcessed()
	{
		if (!m_system_message_processed)
		{
			(this->*m_current_gesture_func)(timer.elapsed2());
			timer.restart();
			m_system_message_processed = !m_system_message_processed;

		}
	}

}
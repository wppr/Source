#ifndef HW_ANDROIDTOUCH_H
#define HW_ANDROIDTOUCH_H

#include "InputTouch.h"
#include "Timer.h"
namespace HW
{
	class AndroidTouch :public InputTouch
	{
	public:
		AndroidTouch();
		~AndroidTouch()
		{

		}

		void OnPointerDown(Vector2 const & point,unsigned int pointer_id);
		void OnPointerUp(Vector2 const & point,unsigned int pointer_id );
		void OnPointerMove(Vector2 const & point,unsigned int pointer_id);
		void updateInput();

		void everyFrameProcessed();
	protected:
		
		
		Vector2 m_touch_pos[4];
		bool m_touch_down[4];
		Timer timer;


	};
}
#endif 
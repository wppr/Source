#ifndef HW_PASS_H
#define HW_PASS_H
#include "RenderState.h"
#include "GpuProgram.h"

namespace HW
{
	class GpuProgram;
	enum PassType
	{
		PT_SHADOW_MAP,			//
		PT_REFLECTION_MAP,
		PT_POST_PROCESS,
		PT_SHADOW_CASTING,
		PASS_TYPE_COUNT
	}; 
	class Pass
	{ 
	public:
		Pass(PassType type) : mProgram(0),mType(type){}
		Pass()
		{
			mProgram = (NULL);
			mRasterState = RasterState::getDefault();
			mBlendState = BlendState::getDefault();
			mDepthStencilState = DepthStencilState::getDefault();
			mClearState = ClearState::getDefault();
		}
		~Pass(){}

		void bindGpuProgram(GpuProgram * program) 
		{
			mProgram = program;
		}

		GpuProgram * getGpuProgram()
		{
			return mProgram;
		}

		/// »æÖÆ×´Ì¬
		RasterState 			mRasterState;
		BlendState  			mBlendState;
		DepthStencilState 		mDepthStencilState;
		ClearState				mClearState;
		GpuProgram *			mProgram;
		
		PassType mType;
	};
}

#endif

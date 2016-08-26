#ifndef _DX11_RENDERABLE_H_
#define _DX11_RENDERABLE_H_

#include "Renderable.h"
#include "d3d11.h"

namespace HW{
	class DX11RenderSystem;
	class DX11Renderable : public Renderable{
	public:
		DX11Renderable(RenderSystem* renderSystem);
		virtual ~DX11Renderable();

		virtual void createInternalRes();
		virtual void releaseInternalRes();
		virtual GpuVertexData* GetGpuAttrData(InputLayout* layout);

	private:
		// A quick access to render system
		DX11RenderSystem* m_pSystem;
	};
}

#endif
#include "DX11Renderable.h"
#include "DX11RenderSystem.h"
#include "DX11GpuVertexData.h"
#include "DX11IndexBuffer.h"
#include "ShaderManager.h"

namespace HW{
	DX11Renderable::DX11Renderable(RenderSystem* renderSystem){
		m_pSystem = dynamic_cast<DX11RenderSystem*>(renderSystem);
	}

	DX11Renderable::~DX11Renderable(){
		releaseInternalRes();
	}

	void DX11Renderable::createInternalRes(){
		if (!m_pGpuAttrData)
			m_pGpuAttrData = new DX11GpuVertexData();
		if (!m_IndexBuffer)
			m_IndexBuffer = new DX11IndexBuffer();
		if (!m_IndexBuffer->valid())
			m_IndexBuffer->CreateInternalRes(m_pSystem, this);
	}

	void DX11Renderable::releaseInternalRes(){
		if (m_pGpuAttrData){
			delete m_pGpuAttrData;
			m_pGpuAttrData = NULL;
		}
		if (m_IndexBuffer){
			delete m_IndexBuffer;
			m_IndexBuffer = NULL;
		}
	}

	GpuVertexData* DX11Renderable::GetGpuAttrData(InputLayout* layout){
		if (!m_pGpuAttrData)
			m_pGpuAttrData = new DX11GpuVertexData();
		m_pGpuAttrData->CreateInternalRes(m_pSystem, this, layout);
		return m_pGpuAttrData;
	}
}
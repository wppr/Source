#ifndef _DX11_INDEX_BUFFER_H_
#define _DX11_INDEX_BUFFER_H_

#include "IndexBuffer.h"
#include "d3d11.h"

namespace HW{
	class DX11IndexBuffer : public IndexBuffer{
	public:
		DX11IndexBuffer();
		virtual ~DX11IndexBuffer();

		virtual unsigned int GetIndexCount() const{ return m_uIndexCount; }
		virtual unsigned int GetBitsPerIndex() const{ return m_uBitsPerIndex; }

		virtual void CreateInternalRes(RenderSystem* renderSystem, Renderable* renderable);
		virtual void ReleaseInternalRes();
		DXGI_FORMAT GetIndexFormat() const;
		ID3D11Buffer* GetIndexBuffer(){
			return m_pIndexBuffer;
		}

	private:
		unsigned int m_uIndexCount;
		unsigned int m_uBitsPerIndex;
		ID3D11Buffer* m_pIndexBuffer;
	};
}

#endif
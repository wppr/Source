#ifndef _DX11_VERTEX_BUFFER_H_
#define _DX11_VERTEX_BUFFER_H_

#include "../VertexBuffer.h"
#include "d3d11.h"

namespace HW{
	class DX11InputLayout;
	class DX11VertexBuffer : public VertexBuffer{
	public:
		DX11VertexBuffer();
		virtual ~DX11VertexBuffer();
		
		virtual UINT GetNumVertices() const{ return m_uVertexCount; }
		virtual UINT GetPerVertexSize() const{ return m_uPerVertexSize; }
		virtual bool CreateInternalRes(RenderSystem* renderSystem, Renderable* renderable, InputLayout* layout);
		// not used now
		virtual bool CreateInternalRes(UINT pairID, int handle, void* data, UINT totalsize, UINT elementsize){ return false; };
		virtual bool UpdateInternalRes(Renderable* renderable);
		virtual void ReleaseInternalRes();

		ID3D11Buffer*& GetVertexBuffer(){ return m_pVertexBuffer; }

	private:
		// Copy all the vertex data from renderable to buf
		void CopyVertexData(unsigned char*buf, Renderable* renderable);

	private:
		ID3D11Buffer* m_pVertexBuffer;
		// For quick access
		ID3D11DeviceContext* m_pContext;
		DX11InputLayout* m_pInputLayout;
		UINT m_uVertexCount;
		UINT m_uPerVertexSize;
	};
}

#endif
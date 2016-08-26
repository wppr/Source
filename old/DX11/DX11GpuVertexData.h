#ifndef _DX11_GPU_ATTRIBUTE_DATA_H_
#define _DX11_GPU_ATTRIBUTE_DATA_H_

#include <map>

#include "d3d11.h"
#include "InputLayout.h"
#include "GpuVertexData.h"
namespace HW{

	class DX11RenderSystem;
	class DX11InputLayout;
	/**	This class is used to store Vertex Buffers created for GPU
			And maintain the vertex buffer order
	*/
	class DX11GpuVertexData : public GpuVertexData{
	public:
		DX11GpuVertexData();
		virtual ~DX11GpuVertexData();

		virtual bool CreateInternalRes(RenderSystem* renderSystem, as_Geometry* geometry, InputLayout* layout);
		virtual void ReleaseInternalRes();
		virtual bool UpdateInternalRes(RenderSystem* renderSystem, as_Geometry* geometry);

		virtual void Create(as_Geometry* geometry, vector<InputLayout_Desc*>& desc);
		virtual void Draw();
		virtual void Update(as_Geometry* geometry, vector<InputLayout_Desc*>& desc);

		bool m_bResourceValid;

		ID3D11Buffer** GetCurrentBuffers(){
			return m_CurrentKeyBuffers.second;
		}

		int GetCurrentBufferNum(){
			return m_CurrentKeyBuffers.first.Length;
		}

	private:
		// Store all the vertex data in different buffers
		// First is semantic key num   (key >> 28)
		std::map<unsigned int, ID3D11Buffer*> m_mSemBufferMap;
		std::map<IPLKey, ID3D11Buffer**> m_mKeyBuffersMap;
		// The Current Key and it's relative Buffers pointer
		std::pair<IPLKey, ID3D11Buffer**> m_CurrentKeyBuffers;
	};
}

#endif
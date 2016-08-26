#include "DX11RenderSystem.h"
#include "DX11IndexBuffer.h"
#include "dx11Utils.h"

namespace HW{
	DX11IndexBuffer::DX11IndexBuffer() : m_uBitsPerIndex(0), m_uIndexCount(0){
		m_pIndexBuffer = NULL;
	}

	DX11IndexBuffer::~DX11IndexBuffer(){
		ReleaseInternalRes();
	}

	// Create index Buffer into ID3D11Buffer*
	// Currently create buffer with D3D11_USAGE_IMMUTABLE
	void DX11IndexBuffer::CreateInternalRes(RenderSystem* renderSystem, Renderable* renderable){
		m_uIndexCount = renderable->m_Indices.size();
		m_uBitsPerIndex = 32;

		DX11RenderSystem* pSystem = dynamic_cast<DX11RenderSystem*>(renderSystem);
		ID3D11Device* pDevice = pSystem->GetDX11Device();
		
		D3D11_BUFFER_DESC bufDesc;
		bufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufDesc.ByteWidth = sizeof(UINT)* m_uIndexCount;
		bufDesc.CPUAccessFlags = 0;
		bufDesc.MiscFlags = 0;
		bufDesc.Usage = D3D11_USAGE_IMMUTABLE;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &renderable->m_Indices[0];
		HR(pDevice->CreateBuffer(&bufDesc, &initData, &m_pIndexBuffer));
		if (m_pIndexBuffer)
			m_valid = true;
	}

	void DX11IndexBuffer::ReleaseInternalRes(){
		if (m_valid){
			ReleaseCOM(m_pIndexBuffer);
			m_valid = false;
		}
	}

	DXGI_FORMAT DX11IndexBuffer::GetIndexFormat() const{
		switch (m_uBitsPerIndex){
		case 8:
			return DXGI_FORMAT_R8_UINT;
		case 16:
			return DXGI_FORMAT_R16_UINT;
		case 32:
		default:
			return DXGI_FORMAT_R32_UINT;
		}
	}
}
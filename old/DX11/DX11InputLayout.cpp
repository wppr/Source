#include "DX11InputLayout.h"
#include "DX11Utils.h"
#include "DX11RenderSystem.h"
#include "DX11Shader.h"
#include <xnamath.h>

namespace HW{
	DX11InputLayout::DX11InputLayout(){
		m_pInputLayout = NULL;
	}

	DX11InputLayout::~DX11InputLayout(){
		ReleaseCOM(m_pInputLayout);
	}

	std::vector<DX11InputDesc>& DX11InputLayout::GetElement(){
		return m_Element;
	}

	unsigned int DX11InputLayout::GetElementSize(int i) const{
		assert(i >= 0 && i < m_Element.size());
		return DX11DatatypeHelper::GetTypeSize(m_Element[i].eDataType);
	}

	bool DX11InputLayout::CreateInternalRes(RenderSystem* renderSystem, Shader* shader){
		//return false;
		assert(renderSystem);
		assert(shader);
		D3D11_INPUT_ELEMENT_DESC *pDescs = new D3D11_INPUT_ELEMENT_DESC[m_Element.size()];
		int offset = 0;

		for (int i = 0; i < m_Element.size(); i++){
			pDescs[i].SemanticName = m_Element[i].Semantic.c_str();
			pDescs[i].SemanticIndex = m_Element[i].uSemanticIndex;
			pDescs[i].InputSlot = i;			// Assign i
			pDescs[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			pDescs[i].InstanceDataStepRate = 0;
			pDescs[i].AlignedByteOffset = offset;

			switch (m_Element[i].eDataType){
			case DX11_DATA_TYPE::FLOAT2:
				pDescs[i].Format = DXGI_FORMAT_R32G32_FLOAT;	break;
			case DX11_DATA_TYPE::FLOAT3:
				pDescs[i].Format = DXGI_FORMAT_R32G32B32_FLOAT;	break;
			case DX11_DATA_TYPE::FLOAT4:
				pDescs[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;	break;
			}
			offset += DX11DatatypeHelper::GetTypeSize(m_Element[i].eDataType);
		}
		DX11RenderSystem* dx11RenderSystem = dynamic_cast<DX11RenderSystem*>(renderSystem);
		DX11Shader* dx11Shader = dynamic_cast<DX11Shader*>(shader);
		// To be continued
		// Return false if failed to create inputlayout
		HRESULT hr = dx11RenderSystem->GetDX11Device()->CreateInputLayout(pDescs, m_Element.size(),
			dx11Shader->GetShaderBuffer(), dx11Shader->GetShaderBufferSize(), &m_pInputLayout);
		if (FAILED(hr)){
			Logger::WriteErrorLog("CreateInputLayout Failed.");
			delete[] pDescs;
			return false;
		}
		m_uTotalSize = offset;
		delete[] pDescs;
		return true;
	}
}
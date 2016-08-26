#include "DX11GpuVertexData.h"
#include "DX11RenderSystem.h"
#include "Renderable.h"
#include "DX11InputLayout.h"
#include "DX11Utils.h"

namespace HW{
	DX11GpuVertexData::DX11GpuVertexData()
	{
		m_CurrentKeyBuffers.second = NULL;
	}

	DX11GpuVertexData::~DX11GpuVertexData(){
		ReleaseInternalRes();
	}

	bool DX11GpuVertexData::CreateInternalRes(RenderSystem* renderSystem, as_Geometry* geometry, InputLayout* layout){
		assert(renderSystem && geometry && layout);
		const IPLKey& key = layout->GetKey();
		std::map<IPLKey, ID3D11Buffer**>::iterator it = m_mKeyBuffersMap.find(key);
		if (it != m_mKeyBuffersMap.end())	{			// This input layout has been handled
			m_CurrentKeyBuffers = *it;
			return true;
		}

		// Some Initialization to make this func faster
		DX11RenderSystem* pSystem = dynamic_cast<DX11RenderSystem*>(renderSystem);
		DX11InputLayout* dx11Layout = dynamic_cast<DX11InputLayout*>(layout);
		std::vector<DX11InputDesc>& elements = dx11Layout->GetElement();

		int verticesNum = geometry->position.size();
		D3D11_BUFFER_DESC desc;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		D3D11_SUBRESOURCE_DATA initData;
		
		ID3D11Buffer** ppBuffers = new ID3D11Buffer*[elements.size()];
		// Make Vertex Buffer for every Component in layout Elements
		for (int i = 0; i < elements.size(); i++){
			unsigned int temp = (key.keys[i] & 0xF0000000) >> 28;
			std::map<unsigned int, ID3D11Buffer*>::iterator it = m_mSemBufferMap.find(temp);
			if (it != m_mSemBufferMap.end())	// if we have created the vertex buffer
				ppBuffers[i] = it->second;
			
			else {	// if we haven't create vertex buffer relative to the semantic key num
				ID3D11Buffer* vertBuffer = NULL;
#pragma region CreateVertexBuffers
				desc.ByteWidth = verticesNum * dx11Layout->GetElementSize(i);
			
				switch (temp){
				case 1:		// POSITION0
					initData.pSysMem = &geometry->position[0];	break;
				case 2:		// NORMAL0
					initData.pSysMem = &geometry->normal[0];	break;
				case 3:		// TEXCOORD0
					initData.pSysMem = &geometry->texcoord[0];	 break;
				//case 4:		// POSITION1
				//	initData.pSysMem = &renderable->m_PositionArray1[0];	 break;
				//case 5:		// NORMAL1
				//	initData.pSysMem = &renderable->m_NormalArray1[0];	 break;
				//case 6:		// TEXCOORD1
				//	initData.pSysMem = &renderable->m_TexCoord2DArray1[0];	 break;
				case 7:		// BONEINDEX
					initData.pSysMem = &geometry->boneID[0];	 break;
				case 8:		// BONEWEIGHT
					initData.pSysMem = &geometry->boneWeight[0];	 break;
				default:
					break;
				}

				HRESULT hr = pSystem->GetDX11Device()->CreateBuffer(&desc, &initData, &vertBuffer);
				if (FAILED(hr)){		// Create Buffer Failed.
					Logger::WriteErrorLog("DX11GpuAttributeData: CreateInternalRes Failed.\n");
					return false;
				}
#pragma endregion
				// Insert this buffer
				m_mSemBufferMap.insert(std::make_pair(temp, vertBuffer));
				ppBuffers[i] = vertBuffer;
			}
		}
		std::pair<IPLKey, ID3D11Buffer**> keyBufs = std::make_pair(key, ppBuffers);
		// Insert this key-Buffers, make it faster for another access to the same key
		m_mKeyBuffersMap.insert(keyBufs);
		m_CurrentKeyBuffers = keyBufs;
		m_bResourceValid = true;
		return true;
	}

	void DX11GpuVertexData::ReleaseInternalRes(){
		if (!m_mKeyBuffersMap.empty()){
			for (auto& it = m_mKeyBuffersMap.begin(); it != m_mKeyBuffersMap.end(); it++)
				delete[] it->second;
			m_mKeyBuffersMap.clear();
			m_CurrentKeyBuffers.second = NULL;
		}
		if (!m_mSemBufferMap.empty()){
			for (auto& it = m_mSemBufferMap.begin(); it != m_mSemBufferMap.end(); it++)
				ReleaseCOM(it->second);
			m_mSemBufferMap.clear();
		}
	}

	bool DX11GpuVertexData::UpdateInternalRes(RenderSystem* renderSystem, as_Geometry* geometry){
		if (!geometry || !renderSystem)
			return false;
		DX11RenderSystem* pSystem = dynamic_cast<DX11RenderSystem*>(renderSystem);
		ID3D11Device* pDevice = pSystem->GetDX11Device();
		ID3D11DeviceContext* pContext = pSystem->GetDX11Context();
		const IPLKey& key = m_CurrentKeyBuffers.first;

		for (int i = 0; i < key.Length; i++){
			D3D11_MAPPED_SUBRESOURCE mappedData;
			HRESULT hr = pContext->Map(m_CurrentKeyBuffers.second[i], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
			if (FAILED(hr)){
				Logger::WriteErrorLog("DX11GpuAttributeData: UpdateInternalRes Failed.\n");
				return false;
			}
			int temp = (key.keys[i] & 0xF0000000) >> 28;
			switch (temp){
			case 1:		// POSITION0
				mappedData.pData = &geometry->position[0];	break;
			case 2:		// NORMAL0
				mappedData.pData = &geometry->normal[0];	break;
			case 3:		// TEXCOORD0
				mappedData.pData = &geometry->texcoord[0];	 break;
			//case 4:		// POSITION1
			//	mappedData.pData = &renderable->m_PositionArray1[0];	 break;
			//case 5:		// NORMAL1
			//	mappedData.pData = &renderable->m_NormalArray1[0];	 break;
			//case 6:		// TEXCOORD1
			//	mappedData.pData = &renderable->m_TexCoord2DArray1[0];	 break;
			case 7:		// BONEINDEX
				mappedData.pData = &geometry->boneID[0];	 break;
			case 8:		// BONEWEIGHT
				mappedData.pData = &geometry->boneWeight[0];	 break;
			default:
				break;
			}
			pContext->Unmap(m_CurrentKeyBuffers.second[i], 0);
		}
		return true;
	}

	void DX11GpuVertexData::Draw(){
		/*
		assert(m_pRenderSystem);
		ID3D11DeviceContext* pContext = m_pRenderSystem->GetDX11Context();
		pContext->DrawIndexed(m_uIndexCount, 0, 0);*/
	}

	void DX11GpuVertexData::Create(as_Geometry* geo, vector<InputLayout_Desc*>& desc)
	{

	}



	void DX11GpuVertexData::Update(as_Geometry* geo, vector<InputLayout_Desc*>& desc)
	{

	}

}
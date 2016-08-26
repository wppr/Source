#include "DX11VertexBuffer.h"
#include "DX11Utils.h"
#include "DX11InputLayout.h"
#include "DX11DatatypeHelper.h"
#include "Renderable.h"
#include "DX11RenderSystem.h"

namespace HW{
	DX11VertexBuffer::DX11VertexBuffer() : m_uVertexCount(0), m_uPerVertexSize(0){
		m_pVertexBuffer = NULL;
		m_pContext = NULL;
		m_pInputLayout = NULL;
	}

	DX11VertexBuffer::~DX11VertexBuffer(){
		ReleaseInternalRes();
	}

	bool DX11VertexBuffer::CreateInternalRes(RenderSystem* renderSystem, Renderable* renderable, InputLayout* layout){
		DX11InputLayout* dx11Layout = dynamic_cast<DX11InputLayout*>(layout);
		m_uPerVertexSize = dx11Layout->GetTotalSize();
		m_uVertexCount = renderable->m_PositionArray.size();
		assert(m_uVertexCount);
		if (!m_uPerVertexSize){
			Logger::WriteErrorLog("Create Vertex Buffer Error! The InputLayout should CreateInternalResource before!\n");
			return false;
		}

		DX11RenderSystem* pSystem = dynamic_cast<DX11RenderSystem*>(renderSystem);
		ID3D11Device* pDevice = pSystem->GetDX11Device();

		D3D11_BUFFER_DESC bufDesc;
		bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufDesc.MiscFlags = 0;
		bufDesc.Usage = (D3D11_USAGE)DX11DatatypeHelper::Get(m_buffer_store);
		if (bufDesc.Usage == D3D11_USAGE_DYNAMIC)
			bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		else
			bufDesc.CPUAccessFlags = 0;
		bufDesc.ByteWidth = m_uPerVertexSize * m_uVertexCount;
		// Allocate the memory to store vertex data
		unsigned char* vertexBuf = new unsigned char[bufDesc.ByteWidth];
		// Copy vertex data from renderable to    vertexBuf
		CopyVertexData(vertexBuf, renderable);

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = vertexBuf;
		HRESULT hr = pDevice->CreateBuffer(&bufDesc, &initData, &m_pVertexBuffer);
		delete[] vertexBuf;
		HRRET(pDevice->CreateBuffer(&bufDesc, &initData, &m_pVertexBuffer));
		// Assign the context and inputlayout for quick access
		m_pContext = pSystem->GetDX11Context();
		m_pInputLayout = dx11Layout;
		return true;
	}

	bool DX11VertexBuffer::UpdateInternalRes(Renderable* renderable){
		if (!m_pVertexBuffer){
			Logger::WriteErrorLog("UpdateInternalRes Failed. You should create vertex buffer first.");
			return false;
		}
		assert(m_pContext);
		D3D11_MAPPED_SUBRESOURCE mappedData;
		// Map the vertex buffer
		HRRET(m_pContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
		// Copy vertex data to the mapped buf
		CopyVertexData(reinterpret_cast<unsigned char*>(mappedData.pData), renderable);
		// Unmap
		m_pContext->Unmap(m_pVertexBuffer, 0);
		return true;
	}

	void DX11VertexBuffer::CopyVertexData(unsigned char* vertexBuf, Renderable* renderable){
		assert(m_pInputLayout);
		const IPLKey& key = m_pInputLayout->GetKey();
		int baseOff = 0;
		// Copy Vertex Data to the buffer
		for (int i = 0; i < key.Length; i++){
			int temp = (key.keys[i] & 0xF0000000) >> 28;
			int pos = baseOff;
			int dataSize = 0;
			switch (temp){
			case 1:		// POSITION0
				dataSize = DX11DatatypeHelper::GetTypeSize(DX11_DATA_TYPE::FLOAT3);
				if (!renderable->m_PositionArray.empty()){
					for (int i = 0; i < renderable->m_PositionArray.size(); i++, pos += m_uPerVertexSize)
						memcpy(&vertexBuf[pos], &renderable->m_PositionArray[i], dataSize);
					baseOff += dataSize;
				}
				break;

			case 2:		// NORMAL0
				dataSize = DX11DatatypeHelper::GetTypeSize(DX11_DATA_TYPE::FLOAT3);
				if (!renderable->m_NormalArray.empty()){
					for (int i = 0; i < renderable->m_NormalArray.size(); i++, pos += m_uPerVertexSize)
						memcpy(&vertexBuf[pos], &renderable->m_NormalArray[i], dataSize);
					baseOff += dataSize;
				}
				break;

			case 3:		// TEXCOORD0
				dataSize = DX11DatatypeHelper::GetTypeSize(DX11_DATA_TYPE::FLOAT2);
				if (!renderable->m_TexCoord2DArray.empty()){
					for (int i = 0; i < renderable->m_TexCoord2DArray.size(); i++, pos += m_uPerVertexSize)
						memcpy(&vertexBuf[pos], &renderable->m_TexCoord2DArray[i], dataSize);
					baseOff += dataSize;
				}
				break;

			case 4:		// POSITION1
				dataSize = DX11DatatypeHelper::GetTypeSize(DX11_DATA_TYPE::FLOAT3);
				if (!renderable->m_PositionArray1.empty()){
					for (int i = 0; i < renderable->m_PositionArray1.size(); i++, pos += m_uPerVertexSize)
						memcpy(&vertexBuf[pos], &renderable->m_PositionArray1[i], dataSize);
					baseOff += dataSize;
				}
				break;

			case 5:		// NORMAL1
				dataSize = DX11DatatypeHelper::GetTypeSize(DX11_DATA_TYPE::FLOAT3);
				if (!renderable->m_NormalArray1.empty()){
					for (int i = 0; i < renderable->m_NormalArray1.size(); i++, pos += m_uPerVertexSize)
						memcpy(&vertexBuf[pos], &renderable->m_NormalArray1[i], dataSize);
					baseOff += dataSize;
				}
				break;

			case 6:		// TEXCOORD1
				dataSize = DX11DatatypeHelper::GetTypeSize(DX11_DATA_TYPE::FLOAT2);
				if (!renderable->m_TexCoord2DArray1.empty()){
					for (int i = 0; i < renderable->m_TexCoord2DArray1.size(); i++, pos += m_uPerVertexSize)
						memcpy(&vertexBuf[pos], &renderable->m_TexCoord2DArray1[i], dataSize);
					baseOff += dataSize;
				}
				break;

			case 7:		// BONEINDEX
				dataSize = DX11DatatypeHelper::GetTypeSize(DX11_DATA_TYPE::FLOAT4);
				if (!renderable->m_BoneIndex.empty()){
					for (int i = 0; i < renderable->m_BoneIndex.size(); i++, pos += m_uPerVertexSize)
						memcpy(&vertexBuf[pos], &renderable->m_BoneIndex[i], dataSize);
					baseOff += dataSize;
				}
				break;

			case 8:		// BONEWEIGHT
				dataSize = DX11DatatypeHelper::GetTypeSize(DX11_DATA_TYPE::FLOAT4);
				if (!renderable->m_BoneWeight.empty()){
					for (int i = 0; i < renderable->m_BoneWeight.size(); i++, pos += m_uPerVertexSize)
						memcpy(&vertexBuf[pos], &renderable->m_BoneWeight[i], dataSize);
					baseOff += dataSize;
				}
				break;

			default:
				break;
			}
		}
	}

	void DX11VertexBuffer::ReleaseInternalRes(){
		ReleaseCOM(m_pVertexBuffer);
	}
}
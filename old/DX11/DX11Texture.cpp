#include "DX11Texture.h"
#include "DX11Utils.h"
#include "DX11DatatypeHelper.h"
#include "DX11RenderSystem.h"

namespace HW{
	DX11Texture::DX11Texture() : m_uBindFlag(D3D11_BIND_SHADER_RESOURCE),
		m_uCPUAccessFlag(0), m_Usage(D3D11_USAGE_DEFAULT){
//		m_eFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_pTexture2D = NULL;
//		m_pSamplerState = NULL;
		m_pRenderSystem = 0;
		m_SamplerStateName = "";
	}

	DX11Texture::~DX11Texture(){
		releaseInternalRes();
	}

	// Set the texture Format with DXGI_FORMAT
	void DX11Texture::SetFormat(Texture::PixelFormat format){
//		m_eFormat = (DXGI_FORMAT)DX11DatatypeHelper::Get(format);
		this->pixel_format = format;
	}

	void DX11Texture::SetWAndH(UINT width, UINT height){
		ui_width = width;
		ui_height = height;
	}

	// To get a quick access to render system
	void DX11Texture::SetRenderSystem(RenderSystem* renderSystem){
		m_pRenderSystem = dynamic_cast<DX11RenderSystem*>(renderSystem);
	}

	// Set some parameters that describe the texture to be created
	void DX11Texture::SetTextureParams(UINT bindFlag, UINT cpuAccessFlag, D3D11_USAGE usage){
		m_uBindFlag = bindFlag;
		m_uCPUAccessFlag = cpuAccessFlag;
		m_Usage = usage;
	}

	// only Texture2D is supported now, D3D11_BIND_SHADER_RESOURCE now
	void DX11Texture::createInternalRes(){
		assert(m_pRenderSystem && "DX11Texture: createInternalRes() need the pointer to DX11RenderSystem.");
		if (texture_type != TextureType::Texture_2D)
			return;
		ID3D11Device*device = m_pRenderSystem->GetDX11Device();
		D3D11_TEXTURE2D_DESC texDesc;
		texDesc.ArraySize = 1;
		texDesc.BindFlags = m_uBindFlag;
		texDesc.CPUAccessFlags = m_uCPUAccessFlag;
		texDesc.Format = (DXGI_FORMAT)DX11DatatypeHelper::Get(pixel_format);
		texDesc.MipLevels = 1;
		texDesc.MiscFlags = 0;
		texDesc.Height = ui_height;
		texDesc.Width = ui_width;
		texDesc.Usage = m_Usage;
		if (m_pRenderSystem->Get4xMsaaEnable()){
			texDesc.SampleDesc.Count = 4;
			texDesc.SampleDesc.Quality = m_pRenderSystem->Get4xMsaaQuality() - 1;
		}
		else{
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
		}
		if (FAILED(device->CreateTexture2D(&texDesc, 0, &m_pTexture2D))){
			Logger::WriteErrorLog("DX11Texture: createInternalRes Failed.");
			return;
		}
		
		m_SamplerDesc.AddressU = (D3D11_TEXTURE_ADDRESS_MODE)DX11DatatypeHelper::Get(texture_warp_s);
		m_SamplerDesc.AddressV = (D3D11_TEXTURE_ADDRESS_MODE)DX11DatatypeHelper::Get(texture_warp_t);
		m_SamplerDesc.Filter = (D3D11_FILTER)DX11DatatypeHelper::Get(texture_min_filter, texture_mag_filter);
		m_SamplerDesc.MipLODBias = 0;
		
		resource_valid = true;
	}

	void DX11Texture::releaseInternalRes(){
		ReleaseCOM(m_pTexture2D);
	}

	ID3D11Resource* DX11Texture::GetResource(){
		if (!m_pTexture2D)
			createInternalRes();
		return m_pTexture2D;
	}

	const D3D11_SAMPLER_DESC& DX11Texture::GetSamplerDesc() const{
		return m_SamplerDesc;
	}
}
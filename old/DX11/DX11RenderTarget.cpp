#include "DX11RenderTarget.h"
#include "DX11RenderSystem.h"
#include "DX11Texture.h"
#include "DX11Utils.h"

namespace HW{
	DX11RenderTarget::DX11RenderTarget(const String& name, RenderSystem* system)
		: RenderTarget(name, system){
		m_valid = false;
	}

	DX11RenderTarget::~DX11RenderTarget(){
		releaseInternalRes();
	}

	void DX11RenderTarget::setDimension(unsigned int width, unsigned int height){
		m_uWidth = width;
		m_uHeight = height;
	}

	unsigned int DX11RenderTarget::getHeight() const{
		return m_uHeight;
	}

	unsigned int DX11RenderTarget::getWidth() const{
		return m_uWidth;
	}

	void DX11RenderTarget::setColorBufferFormat(Texture::PixelFormat format){

	}

	void DX11RenderTarget::setDepthBufferFormat(Texture::PixelFormat format){

	}

	void DX11RenderTarget::setColorBufferUsage(HardwareUsage usage){

	}

	void DX11RenderTarget::setDepthBufferUsage(HardwareUsage usage){

	}

	void DX11RenderTarget::createInternalRes(){

	}

	void DX11RenderTarget::releaseInternalRes(){
		if (m_valid){
			for (int i = 0; i < m_vTextures.size(); i++)
				delete m_vTextures[i];
			for (int i = 0; i < m_vSRVs.size(); i++){
				ReleaseCOM(m_vSRVs[i]);
				ReleaseCOM(m_vRTVs[i]);
			}
			ReleaseCOM(m_pDepthStencilView);
			ReleaseCOM(m_pDepthStencilSRV);
		}
	}

	std::vector<ID3D11RenderTargetView*>& DX11RenderTarget::GetRTVs(){
		return m_vRTVs;
	}

	ID3D11DepthStencilView* DX11RenderTarget::GetDepthStencilView(){
		return m_pDepthStencilView;
	}

	void DX11RenderTarget::bindTarget(int index){
		DX11RenderSystem* pSystem = dynamic_cast<DX11RenderSystem*>(m_RenderSystem);
		ID3D11DeviceContext* pContext = pSystem->GetDX11Context();
		pContext->OMSetRenderTargets(m_vRTVs.size(), &m_vRTVs[0], m_pDepthStencilView);
	}

	void DX11RenderTarget::init_as_gbuffer(){
		DX11RenderSystem* pSystem = dynamic_cast<DX11RenderSystem*>(m_RenderSystem);
		ID3D11Device* pDevice = pSystem->GetDX11Device();

		for (int i = 0; i < 4; i++){
			DX11Texture* texture = new DX11Texture();
			// Default format UCHAR_8_8_8_8
			texture->SetRenderSystem(m_RenderSystem);
			texture->SetTextureParams(D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE,
				0, D3D11_USAGE_DEFAULT);
			texture->SetWAndH(m_uWidth, m_uHeight);
			texture->texture_min_filter = Texture::TextureFilter_Nearest;
			texture->texture_mag_filter = Texture::TextureFilter_Nearest;
			// Push_back to the vector
			m_vTextures.push_back(texture);
			ID3D11Texture2D* pTexture2D = dynamic_cast<ID3D11Texture2D*>(texture->GetResource());
			ID3D11RenderTargetView* pRTV = NULL;
			ID3D11ShaderResourceView* pSRV = NULL; 
			// Make render target view and shader resource view
			assert(pTexture2D && "Create Texture Failed in init_as_gbuffer");
			HR(pDevice->CreateRenderTargetView(pTexture2D, 0, &pRTV));
			HR(pDevice->CreateShaderResourceView(pTexture2D, 0, &pSRV));
			m_vRTVs.push_back(pRTV);
			m_vSRVs.push_back(pSRV);
		}

		// Depth/stecil view
		DX11Texture* texture = new DX11Texture();
		texture->SetRenderSystem(m_RenderSystem);
		texture->SetFormat(Texture::PixelFormat::DEPTH_COMPONENT24);
		texture->SetWAndH(m_uWidth, m_uHeight);
		texture->SetTextureParams(D3D11_BIND_DEPTH_STENCIL/* | D3D11_BIND_SHADER_RESOURCE*/, 0, D3D11_USAGE_DEFAULT);
		texture->texture_min_filter = Texture::TextureFilter_Nearest;
		texture->texture_mag_filter = Texture::TextureFilter_Nearest;
		texture->texture_warp_s = Texture::TextureWarp_Clamp;
		texture->texture_warp_t = Texture::TextureWarp_Clamp;
		m_vTextures.push_back(texture);
		ID3D11Texture2D* pTexture2D = dynamic_cast<ID3D11Texture2D*>(texture->GetResource());
		assert(pTexture2D && "Create Texture Failed in init_as_gbuffer");
		HR(pDevice->CreateDepthStencilView(pTexture2D, 0, &m_pDepthStencilView));
		//HR(pDevice->CreateShaderResourceView(pTexture2D, 0, &m_pDepthStencilSRV));
		m_valid = true;
	}

	void DX11RenderTarget::init_as_depthbuffer(){
		DX11RenderSystem *pSystem = dynamic_cast<DX11RenderSystem*>(m_RenderSystem);
		assert(pSystem && "RenderSystem cannot be null");
		ID3D11Device* pDevice = pSystem->GetDX11Device();

		DX11Texture* texture = new DX11Texture();
		texture->SetRenderSystem(m_RenderSystem);
		texture->SetFormat(Texture::PixelFormat::DEPTH_COMPONENT24);
		texture->SetWAndH(m_uWidth, m_uHeight);
		// Used as shader resource when the render target is seemed as shadow map
		texture->SetTextureParams(D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, 0, D3D11_USAGE_DEFAULT);
		texture->texture_min_filter = Texture::TextureFilter_Nearest;
		texture->texture_mag_filter = Texture::TextureFilter_Nearest;
		texture->texture_warp_s = Texture::TextureWarp_Clamp;
		texture->texture_warp_t = Texture::TextureWarp_Clamp;
		m_vTextures.push_back(texture);
		ID3D11Texture2D* pTexture2D = dynamic_cast<ID3D11Texture2D*>(texture->GetResource());
		HR(pDevice->CreateDepthStencilView(pTexture2D, 0, &m_pDepthStencilView));
		HR(pDevice->CreateShaderResourceView(pTexture2D, 0, &m_pDepthStencilSRV));
		m_valid = true;
	}
}
#ifndef _DX11_TEXTURE_H_
#define _DX11_TEXTURE_H_

#include "d3d11.h"
#include "Texture.h"
#include "MyString.h"

namespace HW{
	class RenderSystem;
	class DX11RenderSystem;
	// The Texture Resource for DX11
	// Only support texture2D now
	// D3D11_USAGE_DEFAULT now
	class DX11Texture : public Texture{
	public:
		DX11Texture();
		virtual ~DX11Texture();
		virtual void releaseInternalRes();
		virtual void createInternalRes();
		virtual void useTexture(){};

		void SetTextureParams(UINT bindFlag, UINT cpuAccessFlag, D3D11_USAGE usage);
		void SetFormat(Texture::PixelFormat format);	// Set format
		void SetWAndH(UINT width, UINT height);	// Set width and height
		void SetRenderSystem(RenderSystem* renderSystem);
		void SetSamplerStateName(String& name);
		String GetSamplerStateName(){ return m_SamplerStateName; }
		ID3D11Resource* GetResource();
		const D3D11_SAMPLER_DESC& GetSamplerDesc() const;

	private:
		UINT m_uBindFlag;				// Default: D3D11_BIND_SHADER_RESOURCE
		UINT m_uCPUAccessFlag;	// Default: 0
		D3D11_USAGE m_Usage;	// Default: D3D11_USAGE_DEFAULT
		DX11RenderSystem* m_pRenderSystem;		// A quick access to render system
		//DXGI_FORMAT m_eFormat;
		ID3D11Texture2D* m_pTexture2D;		// Only support texture2D now
		D3D11_SAMPLER_DESC m_SamplerDesc;
		String m_SamplerStateName;					// The name of sampler state that this texture used in shader
	};

	class DX11TextureFactory : public TextureFactory{
	public:
		virtual Texture* createTexture(){
			return new DX11Texture();
		}
	};
}

#endif
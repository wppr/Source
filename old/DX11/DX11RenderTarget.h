#ifndef _DX11_RENDER_TARGET_H_
#define _DX11_RENDER_TARGET_H_

#include <vector>
#include "RenderTarget.h"
#include "d3d11.h"

namespace HW{
	class DX11Texture;
	class DX11RenderTarget : public RenderTarget{
	public:
		DX11RenderTarget(const String& name, RenderSystem* system);
		virtual ~DX11RenderTarget();

		virtual void setDimension(unsigned int width, unsigned int height);
		virtual unsigned int getWidth() const;
		virtual unsigned int getHeight() const;

		virtual void setColorBufferFormat(Texture::PixelFormat format);
		virtual void setDepthBufferFormat(Texture::PixelFormat format);
		// Set usage of hardware buffer, default is HARDWARE_USAGE_DEFAULT
		virtual void setColorBufferUsage(HardwareUsage usage);
		virtual void setDepthBufferUsage(HardwareUsage usage);
		// Create internal resource before it is used
		virtual void createInternalRes();
		virtual void releaseInternalRes();
		virtual void bindTarget(int index = 0);

		void init_as_gbuffer();
		void init_as_depthbuffer();

		std::vector<ID3D11RenderTargetView*>& GetRTVs();
		ID3D11DepthStencilView* GetDepthStencilView();

	private:
		unsigned int m_uWidth;
		unsigned int m_uHeight;
		std::vector<DX11Texture*> m_vTextures;
		std::vector<ID3D11RenderTargetView*> m_vRTVs;
		std::vector<ID3D11ShaderResourceView*> m_vSRVs;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11ShaderResourceView* m_pDepthStencilSRV;
	};
}

#endif

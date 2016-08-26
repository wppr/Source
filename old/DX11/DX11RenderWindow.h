#ifndef _DX11_RENDER_WINDOW_H_
#define _DX11_RENDER_WINDOW_H_

#include "RenderTarget.h"
#include "d3d11.h"

namespace HW{
	/** This class is used to store a render target as window
	   * A render window for dx11 has color and depth/stencil buf and their resourceView
	   * And the viewport
	   * @Remarks: This class just represent the main window target, no other use for the original framework
	   */
	class DX11RenderWindow : public RenderTarget{
	public:
		DX11RenderWindow();
		DX11RenderWindow(const String& name, RenderSystem* system);
		virtual ~DX11RenderWindow();

		virtual void setDimension(unsigned int width, unsigned int height);
		virtual unsigned int getWidth() const{ return m_uWidth; }
		virtual unsigned int getHeight() const{ return m_uHeight; }

		virtual void setColorBufferFormat(Texture::PixelFormat format);
		virtual void setDepthBufferFormat(Texture::PixelFormat format);

		virtual void setColorBufferUsage(HardwareUsage usage);
		virtual void setDepthBufferUsage(HardwareUsage usage);

		virtual void createInternalRes();
		virtual void releaseInternalRes();
		virtual void setWindowPos(unsigned int ix, unsigned int iy);
		virtual void bindTarget(int index = 0);

	private:
		// The width is just the width of viewport, not buffer
		unsigned int m_uWidth;
		unsigned int m_uHeight;
		unsigned int m_uWindowPosX;
		unsigned int m_uWindowPosY;

		DXGI_FORMAT m_ColorBufFormat;
		DXGI_FORMAT m_DepthBufFormat;
		DXGI_USAGE m_ColorBufUsage;
		DXGI_USAGE m_DepthBufUsage;

		D3D11_VIEWPORT m_WindowViewport;
		ID3D11RenderTargetView* m_pColorBufView;
		ID3D11DepthStencilView* m_pDepthBufView;
	};
}

#endif
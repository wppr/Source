#include "DX11RenderWindow.h"
#include "DX11DatatypeHelper.h"
#include "DX11RenderSystem.h"
#include "DX11Utils.h"

namespace HW{
	DX11RenderWindow::DX11RenderWindow() :m_uWidth(0), m_uHeight(0), 
		m_uWindowPosX(0), m_uWindowPosY(0){
		m_ColorBufFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_DepthBufFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		m_ColorBufUsage = D3D11_USAGE_DEFAULT;
		m_DepthBufUsage = D3D11_USAGE_DEFAULT;

		m_pColorBufView = NULL;
		m_pDepthBufView = NULL;
		m_Type = RTT_WINDOW;
	}

	DX11RenderWindow::DX11RenderWindow(const String& name, RenderSystem* system):
		RenderTarget(name, system) , m_uWidth(0), m_uHeight(0), m_uWindowPosX(0), m_uWindowPosY(0){
		m_ColorBufFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_DepthBufFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		m_ColorBufUsage = D3D11_USAGE_DEFAULT;
		m_DepthBufUsage = D3D11_USAGE_DEFAULT;

		m_pColorBufView = NULL;
		m_pDepthBufView = NULL;
		m_Type = RTT_WINDOW;
	}

	DX11RenderWindow::~DX11RenderWindow(){
		releaseInternalRes();
	}

	void DX11RenderWindow::setDimension(unsigned int width, unsigned int height){
		m_uWidth = width;
		m_uHeight = height;
	}

	void DX11RenderWindow::setColorBufferFormat(Texture::PixelFormat format){
		m_ColorBufFormat = (DXGI_FORMAT)DX11DatatypeHelper::Get(format);
	}

	void DX11RenderWindow::setDepthBufferFormat(Texture::PixelFormat format){
		m_DepthBufFormat = (DXGI_FORMAT)DX11DatatypeHelper::Get(format);
	}

	void DX11RenderWindow::setColorBufferUsage(HardwareUsage usage){
		m_ColorBufUsage = (DXGI_USAGE)DX11DatatypeHelper::Get(usage);
	}

	void DX11RenderWindow::setDepthBufferUsage(HardwareUsage usage){
		m_DepthBufUsage = (DXGI_USAGE)DX11DatatypeHelper::Get(usage);
	}

	void DX11RenderWindow::setWindowPos(unsigned int ix, unsigned int iy){
		m_uWindowPosX = ix;
		m_uWindowPosY = iy;
	}

	// To be continued
	void DX11RenderWindow::createInternalRes(){
		/*
		assert(m_RenderSystem);
		DX11RenderSystem* pRenderSystem = dynamic_cast<DX11RenderSystem*>(m_RenderSystem);

		D3D11_TEXTURE2D_DESC colorBufDesc;
		colorBufDesc.ArraySize = 1;
		colorBufDesc.BindFlags = D3D11_BIND_RENDER_TARGET;		// Create as render target
		colorBufDesc.CPUAccessFlags = 0;
		colorBufDesc.Format = m_ColorBufFormat;
		colorBufDesc.Height = pRenderSystem->GetClientHeight();
		colorBufDesc.Width = pRenderSystem->GetClientWidth();*/
		/*¡­¡­¡­¡­*/
	}

	void DX11RenderWindow::releaseInternalRes(){
		if (m_valid){
			ReleaseCOM(m_pColorBufView);
			ReleaseCOM(m_pDepthBufView);
			m_valid = false;
		}
	}

	void DX11RenderWindow::bindTarget(int index){
		DX11RenderSystem* pSystem = dynamic_cast<DX11RenderSystem*>(m_RenderSystem);
		pSystem->SetDefaultRenderTarget();
	}
}
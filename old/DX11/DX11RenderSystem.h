#ifndef _DX11_RENDER_SYSTEM_H_
#define _DX11_RENDER_SYSTEM_H_

#include "RenderSystem.h"
#include "d3d11.h"
#include "MyString.h"
#include <windows.h>

namespace  HW{
	class DX11RenderSystem;
	class DX11GpuProgram;
	extern DX11RenderSystem* g_pDX11RenderSystem;
	class GpuVertexData;
	class DX11RenderSystem : public RenderSystem{
	public:
		DX11RenderSystem();
		~DX11RenderSystem();

		// Set the width and height of client window
		void SetWandH(int w, int h);
		void GetWandH(int& w, int& h);

		// Initialize DX11 context and device
		bool Initialize();

		// Bind Input Layout to the IA stage
		void BindInputLayout(InputLayout* layout);

		// Set the primitives type of the input data
		void SetPrimType(PrimType geoType = PRIM_TYPE_TRIANGLES);

		// Bind the gpu program
		void BindGpuProgram(GpuProgram *program);

		// Set the fill mode as wire frame
		void EnableWireFrame(bool op = false);

		// Set the cull mode
		void SetCullMode(CullMode md = CULL_MODE_BACK);

		// Set which side of a face will be thought as the front face
		void SetFaceFront(bool clockwise = true);

		// Enable/disable the multi-sample
		/* The multi-sample not supported yet */
		void EnableMultiSample(bool ms = false){};

		// Enable Scissor Test
		void EnableScissorTest(bool teston = false);
		void SetScissorRect(int tplx, int tply, int btrx, int btry);

		// Set Raster state to default
		void RasterParamDefault();
		
		// Apply Raster Param set
		void ApplyRasterParamSet();
		
		// Set the viewport
		void SetViewport(float tplx, float tply, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

		// Enable/disable the alpha to coverage
		void EnableBlendAlphaCoverage(bool bac = false);
		// Enable Independent blend for every render target
		void EnableIndependentBlend(bool ib = false);
		// Set Blend parameters for every render target
		void SetBlendParam(unsigned int target_index, BlendOperand srcBlend,
			BlendOperand destBlend, BlendOperation blendOp, BlendOperand srcAlpha,
			BlendOperand destAlpha, BlendOperation alphaOp, unsigned char rtMask);
		// Enable/disable the blend for a specific render target
		void EnableBlend(unsigned int target_index, bool blend = false);
		// Set blend state to default
		void BlendParamDefault();
		// Set blend factor
		void SetBlendFactor(float const bf[4], unsigned int sampleMask = 0xffffffff);
		// Apply the set of Blend param
		void ApplyBlendParamSet();

		// Set depth state
		void SetDepthParameters(ComparisonFunc func, bool mask);
		// Enable/Disable Depth Test
		void EnableDepthTest(bool teston = true);
		// Set Stencil state
		void SetStencilParameters(unsigned char readMask, unsigned char writeMask, StencilOperation frontFailOp,
			StencilOperation frontDepthFailOp, StencilOperation frontPassOp, ComparisonFunc front,
			StencilOperation backFailOp, StencilOperation backDepthFailOp, StencilOperation backPassOp,
			ComparisonFunc back, unsigned refv);
		// Enable/Disable Stencil test
		void EnableStencil(bool teston = false);
		// Set depth/stencil state to default
		void DepthStencilDefault();
		// Apply the param set of depth/stencil state
		void ApplyDepthStencilParamSet();

		// Set All the parameters within this pipeline to default
		void AllParamDefault();

		// Set background color
		void SetBackground(const Vector4& background);

		// Set Depth clear flag
		void DepthClearParam(float depth = 1.0f, unsigned char stencil = 0);

		// Do some internal initialization before actual rendering
		bool PreRender(){ return true; };

		// Swap back buffer to front and present the frame buffer to the monitor
		void BufferSwap();

		// Main render function
		void RenderMetadata(GpuVertexData* attrData, IndexBuffer* indexData, const IPLKey& iplKey);

		virtual void RenderMetadata(VertexBuffer* vertData, IndexBuffer* indexData, const IPLKey & ipkey);
		// Clear frame buffer, input bitwise #FrameBuffer
		void ClearBuffer(unsigned int buffer);

		// Do some internal recoveray after rendering
		bool PostRender(){ return true; };

		// Get the name of this render system
		String GetName() const{ return String("DirectX 11"); }

		// push back a render target to a stack
		void PushRenderTarget(RenderTarget* rt);

		// Pop a render target from the stack
		void PopRenderTarget();

		// Get Current Render target
		RenderTarget* CurrentRenderTarget() const;

		// Factory function for Creating Vertex Buffer
		VertexBuffer* CreateVertexBuffer();

		// Factory function for creating index Buffer
		IndexBuffer* CreateIndexBuffer();

		void ClearColor(float r, float g, float b, float a);

		void setClearState(const ClearState &clearState);
		void setBlendState(const BlendState& blendState);
		void setRasterState(const RasterState& rasterState);
		void setDepthStencilState(const DepthStencilState& depthStencilState);

		void setPolygonOffset(bool enable, float offset_factor = 0.0f, float offset_unit = 0.0f);
		void term_display();

	public:
		LRESULT CALLBACK MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		ID3D11Device* GetDX11Device() const{ return m_pDevice; }
		ID3D11DeviceContext* GetDX11Context() const{ return m_pImmediateContext; }
		bool Get4xMsaaEnable() const{ return m_bEnable4xMsaa; }
		UINT Get4xMsaaQuality() const{ return m_u4xMsaaQuality; }

		UINT GetClientWidth() const{ return m_uClientWidth; }
		UINT GetClientHeight() const{ return m_uClientHeight; }

		// Use this function to set the default window as render target
		void SetDefaultRenderTarget();

	private:
		bool InitMainWindow();
		bool InitDirect3D();
		void OnResize();

	private:
		DX11GpuProgram* m_pGpuProgram;
		Vector4 m_Background;
		float m_fDepthClear;
		unsigned char m_StencilClear;
		std::stack<RenderTarget*> m_RTStack;
		bool m_bIsTerm;		// Whether this device is terminated

	private:
		ID3D11Device * m_pDevice;
		ID3D11DeviceContext * m_pImmediateContext;
		bool m_bEnable4xMsaa;
		UINT m_u4xMsaaQuality;
		IDXGISwapChain * m_pSwapChain;
		ID3D11RenderTargetView * m_pRenderTargetView;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11Texture2D* m_pDepthStencilBuffer;
		D3D11_VIEWPORT m_ScreenViewport;

		UINT m_uClientWidth;
		UINT m_uClientHeight;
		HWND m_hMainWnd;

		bool m_bResizing;

		D3D11_RASTERIZER_DESC m_RasterizerDesc;
		// Blend relatives
		D3D11_BLEND_DESC m_BlendDesc;
		float m_BlendFactor[4];
		unsigned int m_uSampleMask;

		D3D11_DEPTH_STENCIL_DESC m_DepthStencilDesc;
		unsigned int m_uStencilRef;
	};

	class DX11MsgProc{
	public:
		static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	};

}


#endif
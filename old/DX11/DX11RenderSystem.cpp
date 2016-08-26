#include "DX11RenderSystem.h"
#include "d3d11.h"
#include "DX11Utils.h"
#include "DX11InputLayout.h"
#include "DX11GpuProgram.h"
#include "DX11DatatypeHelper.h"
#include "DX11RenderTarget.h"
#include "DX11VertexBuffer.h"
#include "DX11IndexBuffer.h"
#include "DX11GpuVertexData.h"

namespace HW{
	DX11RenderSystem* g_pDX11RenderSystem = 0;
	LRESULT CALLBACK DX11MsgProc::MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
		return g_pDX11RenderSystem->MsgProc(hWnd, msg, wParam, lParam);
	}

	DX11RenderSystem::DX11RenderSystem()
		: m_hMainWnd(0), m_bResizing(false), m_pRenderTargetView(0), m_pGpuProgram(0), m_bIsTerm(false),
		m_pDepthStencilView(0), m_pDepthStencilBuffer(0), m_uClientWidth(480), m_uClientHeight(800),
		m_bEnable4xMsaa(false), m_Background(0.0f, 0.0f, 0.0f, 0.0f), m_fDepthClear(1.0f), m_StencilClear(0){
		
		g_pDX11RenderSystem = this;
	}

	DX11RenderSystem::~DX11RenderSystem(){
		term_display();
		g_pDX11RenderSystem = 0;
	}

	void DX11RenderSystem::term_display(){
		if (!m_bIsTerm){
			ReleaseCOM(m_pDevice);
			ReleaseCOM(m_pImmediateContext);
			ReleaseCOM(m_pSwapChain);
			ReleaseCOM(m_pRenderTargetView);
			ReleaseCOM(m_pDepthStencilView);
			ReleaseCOM(m_pDepthStencilBuffer);
			m_bIsTerm = true;
		}
	}
	
	void DX11RenderSystem::SetWandH(int w, int h){
		m_uClientWidth = w;
		m_uClientHeight = h;
	}

	void DX11RenderSystem::GetWandH(int& w, int& h){
		w = m_uClientWidth;
		h = m_uClientHeight;
	}

	// Set the default window as render target
	void DX11RenderSystem::SetDefaultRenderTarget(){
		m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	}

	// Bind a GpuProgram which has been compiled to this render system
	void DX11RenderSystem::BindGpuProgram(GpuProgram* program){
		if (!program)
			return;
		m_pGpuProgram = dynamic_cast<DX11GpuProgram*>(program);
		//m_pGpuProgram->NotifyRenderSystem(this);
		m_pGpuProgram->UseProgram();
	}

	// Bind the input layout to the IA stage
	void DX11RenderSystem::BindInputLayout(InputLayout* layout){
		DX11InputLayout* inputLayout = dynamic_cast<DX11InputLayout*>(layout);
		// input the vertex shader to get input signature
		if (!inputLayout->CreateInternalRes(this, m_pGpuProgram->GetShader(ShaderType::ST_VERTEX))){
			Logger::WriteErrorLog("DX11RenderSystem: BindInputLayout Failed.\n");
			return;
		}
		// Set the input layout
		m_pImmediateContext->IASetInputLayout(inputLayout->GetInputLayout());
	}

	// Set the primitives type of the input data
	void DX11RenderSystem::SetPrimType(PrimType geoType){
		D3D11_PRIMITIVE_TOPOLOGY primTopology = (D3D11_PRIMITIVE_TOPOLOGY)DX11DatatypeHelper::Get(geoType);
		if (primTopology == D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED){
			Logger::WriteErrorLog("DX11RenderSystem: SetPrimType Failed. Undefined PrimType.\n");
			return;
		}
		m_pImmediateContext->IASetPrimitiveTopology(primTopology);
	}

	// Enable/Disable the wireframe mode
	void DX11RenderSystem::EnableWireFrame(bool op){
		if (op)
			m_RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
		else
			m_RasterizerDesc.FillMode = D3D11_FILL_SOLID;
	}

	// Set the cull mode
	void DX11RenderSystem::SetCullMode(CullMode md){
		switch (md){
		case CullMode::CULL_MODE_NONE:
			m_RasterizerDesc.CullMode = D3D11_CULL_NONE;		break;
		case CullMode::CULL_MODE_BACK:
			m_RasterizerDesc.CullMode = D3D11_CULL_BACK;		break;
		case CullMode::CULL_MODE_FRONT:
			m_RasterizerDesc.CullMode = D3D11_CULL_FRONT;	break;
		default:		// CullMode::CULL_MODE_FRONTANDBACK,	this is undefined in dx11
			Logger::WriteErrorLog("DX11RenderSystem: SetCullMode Failed. Undefined Cull Mode in DX11.\n");
			break;
		}
	}

	// Set Whitch side of a face will be 
	void DX11RenderSystem::SetFaceFront(bool clockwise){
		if (clockwise)
			m_RasterizerDesc.FrontCounterClockwise = false;
		else
			m_RasterizerDesc.FrontCounterClockwise = true;
	}

	// Enable Scissor Test
	void DX11RenderSystem::EnableScissorTest(bool teston){
		m_RasterizerDesc.ScissorEnable = teston;
	}

	// Set Scissor Rect
	void DX11RenderSystem::SetScissorRect(int tplx, int tply, int btrx, int btry){
		D3D11_RECT rect{ tplx, tply, btrx, btry };
		m_pImmediateContext->RSSetScissorRects(1, &rect);
	}

	// Set the rasterizer state to default
	void DX11RenderSystem::RasterParamDefault(){
		ZeroMemory(&m_RasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
		m_RasterizerDesc.CullMode = D3D11_CULL_BACK;
		m_RasterizerDesc.FillMode = D3D11_FILL_SOLID;
		m_RasterizerDesc.DepthClipEnable = true;
		m_pImmediateContext->RSSetState(0);
	}

	// Apply the set of raster param
	void DX11RenderSystem::ApplyRasterParamSet(){
		ID3D11RasterizerState * rs = NULL;
		HR(m_pDevice->CreateRasterizerState(&m_RasterizerDesc, &rs));
		m_pImmediateContext->RSSetState(rs);
		ReleaseCOM(rs);
	}

	// Set the viewport
	void DX11RenderSystem::SetViewport(float tplx, float tply, float width, float height, float minDepth, float maxDepth){
		m_ScreenViewport.Width = width;
		m_ScreenViewport.Height = height;
		m_ScreenViewport.TopLeftX = tplx;
		m_ScreenViewport.TopLeftY = tply;
		m_ScreenViewport.MinDepth = minDepth;
		m_ScreenViewport.MaxDepth = maxDepth;
		m_pImmediateContext->RSSetViewports(1, &m_ScreenViewport);
	}

	// Enable/Disable the alpha to coverage
	void DX11RenderSystem::EnableBlendAlphaCoverage(bool bac){
		m_BlendDesc.AlphaToCoverageEnable = bac;
	}

	// Enable/Disable Independent Blend Desc for every render target
	void DX11RenderSystem::EnableIndependentBlend(bool ib){
		m_BlendDesc.IndependentBlendEnable = ib;
	}

	// Set the Blend Param for specific render target
	void DX11RenderSystem::SetBlendParam(unsigned int target_index, BlendOperand srcBlend,
		BlendOperand destBlend, BlendOperation blendOp, BlendOperand srcAlpha,
		BlendOperand destAlpha, BlendOperation alphaOp, unsigned char rtMask){
		int i = target_index;
		m_BlendDesc.RenderTarget[i].SrcBlend = (D3D11_BLEND)DX11DatatypeHelper::Get(srcBlend);
		m_BlendDesc.RenderTarget[i].DestBlend = (D3D11_BLEND)DX11DatatypeHelper::Get(destBlend);
		m_BlendDesc.RenderTarget[i].BlendOp = (D3D11_BLEND_OP)DX11DatatypeHelper::Get(blendOp);
		m_BlendDesc.RenderTarget[i].SrcBlendAlpha = (D3D11_BLEND)DX11DatatypeHelper::Get(srcAlpha);
		m_BlendDesc.RenderTarget[i].DestBlendAlpha = (D3D11_BLEND)DX11DatatypeHelper::Get(destAlpha);
		m_BlendDesc.RenderTarget[i].BlendOpAlpha = (D3D11_BLEND_OP)DX11DatatypeHelper::Get(alphaOp);
		m_BlendDesc.RenderTarget[i].RenderTargetWriteMask = rtMask;
	}

	// Enable/Disable Blend for specific render target
	void DX11RenderSystem::EnableBlend(unsigned int target_index, bool blend){
		m_BlendDesc.RenderTarget[target_index].BlendEnable = blend;
	}

	// Set the Blend Factor
	void DX11RenderSystem::SetBlendFactor(float const bf[4], unsigned int sampleMask){
		for (int i = 0; i < 4; i++)
			m_BlendFactor[i] = bf[i];
		m_uSampleMask = sampleMask;
	}

	// Set Blend Param default
	void DX11RenderSystem::BlendParamDefault(){
		m_BlendDesc.AlphaToCoverageEnable = false;
		m_BlendDesc.IndependentBlendEnable = false;
		m_BlendDesc.RenderTarget[0].BlendEnable = false;
		m_BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		m_BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		m_BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		m_BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		m_BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		m_BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		m_BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		for (int i = 1; i < 8; i++)
			m_BlendDesc.RenderTarget[i] = m_BlendDesc.RenderTarget[0];
		for (int i = 0; i < 4; i++)
			m_BlendFactor[i] = 1;
		m_uSampleMask = 0xffffffff;
		m_pImmediateContext->OMSetBlendState(NULL, NULL, 0xffffffff);
	}

	// Apply the set of blend param
	void DX11RenderSystem::ApplyBlendParamSet(){
		ID3D11BlendState* bs = NULL;
		HR(m_pDevice->CreateBlendState(&m_BlendDesc, &bs));
		m_pImmediateContext->OMSetBlendState(bs, m_BlendFactor, m_uSampleMask);
		ReleaseCOM(bs);
	}

	// Set Depth Param
	void DX11RenderSystem::SetDepthParameters(ComparisonFunc func, bool mask){
		m_DepthStencilDesc.DepthWriteMask = mask ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
		m_DepthStencilDesc.DepthFunc = (D3D11_COMPARISON_FUNC)DX11DatatypeHelper::Get(func);
	}

	// Enable/Disable Depth Test
	void DX11RenderSystem::EnableDepthTest(bool teston){
		m_DepthStencilDesc.DepthEnable = teston;
	}

	// Set Stencil Params
	void DX11RenderSystem::SetStencilParameters(unsigned char readMask, unsigned char writeMask, StencilOperation frontFailOp,
		StencilOperation frontDepthFailOp, StencilOperation frontPassOp, ComparisonFunc front,
		StencilOperation backFailOp, StencilOperation backDepthFailOp, StencilOperation backPassOp,
		ComparisonFunc back, unsigned int refv){
		m_DepthStencilDesc.StencilReadMask = readMask;
		m_DepthStencilDesc.StencilWriteMask = writeMask;
		m_DepthStencilDesc.FrontFace.StencilFailOp = (D3D11_STENCIL_OP)DX11DatatypeHelper::Get(frontFailOp);
		m_DepthStencilDesc.FrontFace.StencilDepthFailOp = (D3D11_STENCIL_OP)DX11DatatypeHelper::Get(frontDepthFailOp);
		m_DepthStencilDesc.FrontFace.StencilPassOp = (D3D11_STENCIL_OP)DX11DatatypeHelper::Get(frontPassOp);
		m_DepthStencilDesc.FrontFace.StencilFunc = (D3D11_COMPARISON_FUNC)DX11DatatypeHelper::Get(front);
		
		m_DepthStencilDesc.BackFace.StencilFailOp = (D3D11_STENCIL_OP)DX11DatatypeHelper::Get(backFailOp);
		m_DepthStencilDesc.BackFace.StencilDepthFailOp = (D3D11_STENCIL_OP)DX11DatatypeHelper::Get(backDepthFailOp);
		m_DepthStencilDesc.BackFace.StencilPassOp = (D3D11_STENCIL_OP)DX11DatatypeHelper::Get(backPassOp);
		m_DepthStencilDesc.BackFace.StencilFunc = (D3D11_COMPARISON_FUNC)DX11DatatypeHelper::Get(back);

		m_uStencilRef = refv;
	}

	// Enable/Disable Stencil
	void DX11RenderSystem::EnableStencil(bool teston){
		m_DepthStencilDesc.StencilEnable = teston;
	}

	// Set Depth/Stencil state to default
	void DX11RenderSystem::DepthStencilDefault(){
		m_DepthStencilDesc.DepthEnable = true;
		m_DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		m_DepthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		m_DepthStencilDesc.StencilEnable = false;
		m_DepthStencilDesc.StencilReadMask = 0xff;
		m_DepthStencilDesc.StencilWriteMask = 0xff;
		m_DepthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		m_DepthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		m_DepthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		m_DepthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		m_DepthStencilDesc.BackFace = m_DepthStencilDesc.FrontFace;
		m_uStencilRef = 0;
		m_pImmediateContext->OMSetDepthStencilState(0, m_uStencilRef);
	}

	// Apply the set of depth/stencil param
	void DX11RenderSystem::ApplyDepthStencilParamSet(){
		ID3D11DepthStencilState* dss = NULL;
		HR(m_pDevice->CreateDepthStencilState(&m_DepthStencilDesc, &dss));
		m_pImmediateContext->OMSetDepthStencilState(dss, m_uStencilRef);
		ReleaseCOM(dss);
	}

	// Set all the parameters within this pipeline to default
	void DX11RenderSystem::AllParamDefault(){
		RasterParamDefault();
		BlendParamDefault();
		DepthStencilDefault();
		m_Background = Vector4(0.0f, 0.0f, 0.0f, 0.0f);
		m_fDepthClear = 1.0f;
		m_StencilClear = 0;
	}

	void DX11RenderSystem::SetBackground(const Vector4& background){
		m_Background = background;
	}

	// Set the clear flag of depth and stencil
	void DX11RenderSystem::DepthClearParam(float depth, unsigned char stencil){
		m_fDepthClear = depth;
		m_StencilClear = stencil;
	}

	// Swap back buffer to front and present the frame buffer to the monitor
	void DX11RenderSystem::BufferSwap(){
		if (m_RTStack.empty()){
			HR(m_pSwapChain->Present(0, 0));
			return;
		}
		RenderTarget* rt = m_RTStack.top();
		if (rt == NULL)
			return;
		if (rt->getType() == RenderTarget::RTT_WINDOW)
			HR(m_pSwapChain->Present(0, 0));
	}

	// Set Clear Color
	void DX11RenderSystem::ClearColor(float r, float g, float b, float a){
		m_Background = Vector4(r, g, b, a);
	}

	// Clear color and depth buffer of the current render target ( Default window if NULL )
	void DX11RenderSystem::ClearBuffer(unsigned int buffer){
		if (m_RTStack.empty()){
			m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, m_Background.ptr());
			m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, m_fDepthClear, m_StencilClear);
		}
		DX11RenderTarget* rt = dynamic_cast<DX11RenderTarget*>(m_RTStack.top());
		if (rt == NULL)
			return;
		if (rt->getType() == RenderTarget::RTT_WINDOW){
			m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, m_Background.ptr());
			m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, m_fDepthClear, m_StencilClear);
		}
		else if (rt->getType() == RenderTarget::RTT_TEXTURE){
			std::vector<ID3D11RenderTargetView*> RTVs = rt->GetRTVs();
			for (int i = 0; i < RTVs.size(); i++)
				m_pImmediateContext->ClearRenderTargetView(RTVs[i], m_Background.ptr());
			m_pImmediateContext->ClearDepthStencilView(rt->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, m_fDepthClear, m_StencilClear);
		}
	}

	void DX11RenderSystem::PushRenderTarget(RenderTarget* rt){
		if (rt)
			m_RTStack.push(rt);
	}

	void DX11RenderSystem::PopRenderTarget(){
		if (!m_RTStack.empty())
			m_RTStack.pop();
	}

	//Get Current Render Target
	RenderTarget* DX11RenderSystem::CurrentRenderTarget() const{
		if (!m_RTStack.empty())
			return m_RTStack.top();
		else
			return NULL;
	}

	// Factory function for creating Vertex Buffer
	VertexBuffer* DX11RenderSystem::CreateVertexBuffer(){
		return new DX11VertexBuffer();
	}

	// Factory Function for creating Index Buffer
	IndexBuffer* DX11RenderSystem::CreateIndexBuffer(){
		return new DX11IndexBuffer();
	}

	// Set the Clear State, Not used yet
	void DX11RenderSystem::setClearState(const ClearState& clearState){
	}

	// Set the Raster state
	void DX11RenderSystem::setRasterState(const RasterState& rasterState){
		static RasterState m_last_raster_state;
		if (rasterState == m_last_raster_state)
			return;
		this->SetCullMode(rasterState.mCullMode);
		// the input is clockwise
		this->SetFaceFront(!rasterState.mFrontCounterClockwise);
		this->EnableScissorTest(rasterState.mScissorEnable);
		// No write mask in dx11
		m_last_raster_state = rasterState;
		this->ApplyRasterParamSet();
	}

	// Set the Blend State
	void DX11RenderSystem::setBlendState(const BlendState& blendState){
		static BlendState m_last_blend_state;
		if (blendState == m_last_blend_state)
			return;
		this->EnableBlendAlphaCoverage(blendState.mAlphaToConverageEnable);
		this->EnableIndependentBlend(blendState.mIndependentBlendEnable);
		this->EnableBlend(0, blendState.mBlendEnable);
		this->SetBlendParam(0, blendState.mSrcBlend, blendState.mDestBlend, blendState.mBlendFunc,
			blendState.mSrcBlendAlpha, blendState.mDestBlendAlpla, blendState.mBlendFuncAlpha, blendState.mWriteMask);
		this->SetBlendFactor(blendState.mFactor, blendState.mSampleMask);
		this->ApplyBlendParamSet();
		m_last_blend_state = blendState;
	}

	// Set the Depth/Stencil State
	void DX11RenderSystem::setDepthStencilState(const DepthStencilState& depthStencilState){
		static DepthStencilState m_last_depth_state;
		if (depthStencilState == m_last_depth_state)
			return;
		this->EnableDepthTest(depthStencilState.mDepthEnable);
		this->EnableStencil(depthStencilState.mStencilEnable);
		this->SetDepthParameters(depthStencilState.mDepthFunc, depthStencilState.mDepthWriteMask);
		this->SetStencilParameters(depthStencilState.mStencilReadMask, depthStencilState.mStencilWriteMask, depthStencilState.mFrontStencilFailFunc,
			depthStencilState.mFrontStencilDepthFailFunc, depthStencilState.mFrontStencilPassFunc, depthStencilState.mFrontStencilFunc,
			depthStencilState.mBackStencilFailFunc, depthStencilState.mBackStencilDepthFailFunc, depthStencilState.mBackStencilPassFunc,
			depthStencilState.mBackStencilFunc, depthStencilState.mStencilRef);
		this->ApplyDepthStencilParamSet();
		m_last_depth_state = depthStencilState;
	}

	// Set Polygon offset, Didn't find relatives of dx11
	void DX11RenderSystem::setPolygonOffset(bool enable, float offset_factor, float offset_unit){
	}

	// Main Render Interface, iplKey is unuseful here
	void DX11RenderSystem::RenderMetadata(GpuVertexData* attrData, IndexBuffer* indexData, const IPLKey& iplKey){
		assert(attrData && indexData);
		DX11GpuVertexData* dx11AttrData = dynamic_cast<DX11GpuVertexData*>(attrData);
		int numBuffers = dx11AttrData->GetCurrentBufferNum();
		ID3D11Buffer** vertBuffers = dx11AttrData->GetCurrentBuffers();
		DX11IndexBuffer* dx11IndexBuffer = dynamic_cast<DX11IndexBuffer*>(indexData);
		UINT* stride = new UINT[numBuffers];
		memset(stride, 0, sizeof(UINT)*numBuffers);
		UINT* offset = new UINT[numBuffers];
		memset(offset, 0, sizeof(UINT)*numBuffers);
		m_pImmediateContext->IASetVertexBuffers(0, numBuffers, vertBuffers, stride, offset);
		m_pImmediateContext->IASetIndexBuffer(dx11IndexBuffer->GetIndexBuffer(), dx11IndexBuffer->GetIndexFormat(), 0);
		m_pImmediateContext->DrawIndexed(dx11IndexBuffer->GetIndexCount(), 0, 0);
		delete[] stride;
		delete[] offset;
	}

	bool DX11RenderSystem::Initialize(){
		if (!InitMainWindow())
			return false;
		if (!InitDirect3D())
			return false;
		RasterParamDefault();
		BlendParamDefault();
		DepthStencilDefault();
		return true;
	}

	// Create Window
	bool DX11RenderSystem::InitMainWindow(){
		HINSTANCE hInstance = GetModuleHandle(NULL);
		WNDCLASS wc;
		//wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.style = CS_OWNDC;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInstance;
		wc.lpfnWndProc = DX11MsgProc::MainWndProc;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = "DX11WindowClass";
		
		if (!RegisterClass(&wc)){
			MessageBox(0, "RegisterClass Failed.", 0, 0);
			return false;
		}
		DWORD wStyle = WS_VISIBLE | WS_POPUP | WS_BORDER | WS_SYSMENU | WS_CAPTION;
		RECT r = { 0, 0, m_uClientWidth, m_uClientHeight };
		AdjustWindowRect(&r, wStyle, false);
		int width = r.right - r.left;
		int height = r.bottom - r.top;

		m_hMainWnd = CreateWindow("DX11WindowClass", "DX11Window", wStyle,
			0, 0, width, height, 0, 0, hInstance, 0);
		if (!m_hMainWnd){
			MessageBox(0, "CreateWindow Failed.", 0, 0);
			return false;
		}
		ShowWindow(m_hMainWnd, true);
		UpdateWindow(m_hMainWnd);
		return true;
	}
	
	// Create the device and Context
	bool DX11RenderSystem::InitDirect3D(){
		UINT createDeviceFlags = 0;
#if defined(DEBUG) | defined(_DEBUG)
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_FEATURE_LEVEL featureLevel;
		// Create Device and Context
		HRESULT hr = D3D11CreateDevice(
			NULL, D3D_DRIVER_TYPE_HARDWARE,
			0, createDeviceFlags,
			0, 0,
			D3D11_SDK_VERSION,
			&m_pDevice,
			&featureLevel,
			&m_pImmediateContext);
		// Check if creating succeed
		if (FAILED(hr)){
			MessageBox(0, "D3D11CreateDevice Failed!", 0, 0);
			return false;
		}
		// Check if the feature level is supported
		if (featureLevel != D3D_FEATURE_LEVEL_11_0){
			MessageBox(0, "Direct3D Feature Level 11 unsupported.", 0, 0);
			return false;
		}
		// Check if 4x Multi-sample is supported
		HR(m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_u4xMsaaQuality));
		assert(m_u4xMsaaQuality > 0);

		// Create Swap Chain
		DXGI_SWAP_CHAIN_DESC sd;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.Height = m_uClientHeight;
		sd.BufferDesc.Width = m_uClientWidth;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.BufferCount = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		if (m_bEnable4xMsaa){
			sd.SampleDesc.Count = 4;
			sd.SampleDesc.Quality = m_u4xMsaaQuality - 1;
		}
		else{
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
		}
		sd.Flags = 0;
		sd.OutputWindow = m_hMainWnd;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		IDXGIDevice *dxgiDevice = NULL;
		m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
		IDXGIAdapter* dxgiAdapter = NULL;
		dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
		IDXGIFactory* dxgiFactory = NULL;
		dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
		HR(dxgiFactory->CreateSwapChain(m_pDevice, &sd, &m_pSwapChain));
		ReleaseCOM(dxgiDevice);
		ReleaseCOM(dxgiAdapter);
		ReleaseCOM(dxgiFactory);

		OnResize();
		return true;
	}
	void DX11RenderSystem::RenderMetadata(VertexBuffer* vertData, IndexBuffer* indexData, const IPLKey & ipkey)
	{

	}
	void DX11RenderSystem::OnResize(){
		assert(m_pDevice);
		assert(m_pImmediateContext);
		assert(m_pSwapChain);

		ReleaseCOM(m_pRenderTargetView);
		ReleaseCOM(m_pDepthStencilView);
		ReleaseCOM(m_pDepthStencilBuffer);

		// Create RenderTarget View
		HR(m_pSwapChain->ResizeBuffers(1, m_uClientWidth, m_uClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
		ID3D11Texture2D *backBuffer;
		HR(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer));
		HR(m_pDevice->CreateRenderTargetView(backBuffer, 0, &m_pRenderTargetView));

		// Create Depth/Stencil buffer and view
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.Height = m_uClientHeight;
		depthStencilDesc.Width = m_uClientWidth;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.MiscFlags = 0;
		if (m_bEnable4xMsaa){
			depthStencilDesc.SampleDesc.Count = 4;
			depthStencilDesc.SampleDesc.Quality = m_u4xMsaaQuality - 1;
		}
		else{
			depthStencilDesc.SampleDesc.Count = 1;
			depthStencilDesc.SampleDesc.Quality = 0;
		}
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		HR(m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &m_pDepthStencilBuffer));
		HR(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, 0, &m_pDepthStencilView));

		m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		// Set Viewport
		SetViewport(0.0f, 0.0f, static_cast<float>(m_uClientWidth), static_cast<float>(m_uClientHeight), 0.0f, 1.0f);
	}

	






	LRESULT CALLBACK DX11RenderSystem::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam){
		switch (msg){
		case WM_SIZE:
			m_uClientWidth = LOWORD(lParam);
			m_uClientHeight = HIWORD(lParam);
			if (m_pDevice){
				if (wParam == SIZE_MINIMIZED){
				}
				else if (wParam == SIZE_MAXIMIZED){
					OnResize();
				}
				else if (wParam == SIZE_RESTORED){
					if (m_bResizing){
					}
					else
						OnResize();
				}
			}
			return 0;

		case WM_ENTERSIZEMOVE:
			m_bResizing = true;
			return 0;

		case WM_EXITSIZEMOVE:
			m_bResizing = false;
			OnResize();
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}
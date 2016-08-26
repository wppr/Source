#include "EGLRenderSystem.h"
#include "RenderTarget.h"
#include "EGLUtil.h"
#include "RenderState.h"
#include "GLGeometry.h"
#include "EGLTexture.h"
#include "gl_program.h"
#include "ResourceManager.h"
#include "EGLRenderTarget.h"
#include "GLSampler.h"
namespace HW
{
#ifndef ANDROID

	LRESULT CALLBACK BasicMsgProc::MsgProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lPrarm)
	{
		switch(msg)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hwnd,msg,wParam,lPrarm);
	}
#endif
	EGLRenderSystem::EGLRenderSystem():m_BKColor(0,0,0,0),width(480),height(800)
		//,hWnd(0),eglDisplay(0),eglContext(0),eglSurface(0)
	{

	}
	 void printGLString(const char *name, GLenum s) {
		const char *v = (const char *) glGetString(s);
#ifdef ANDROID
		LOGI("%s",v);
#else
		std::cout<<v<<std::endl;
#endif
	}

	bool EGLRenderSystem::Initialize()
	{
#ifdef ANDROID
		const EGLint attribs[] = {

			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
			EGL_DEPTH_SIZE,24,
			EGL_RENDERABLE_TYPE, EGL_WINDOW_BIT,
			EGL_NONE
		};
		EGLint w, h, dummy, format;
		EGLint numConfigs;
		EGLConfig config;
		EGLSurface surface;
		EGLContext context;

		EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		 if(!eglInitialize(display, 0, 0))
			 LOGE("eglinitialize error");
		if(!eglChooseConfig(display, attribs, &config, 1, &numConfigs))
			LOGE("eglchoose config error");

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
		if(!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format))
			LOGE("get config attrib error");
		 ANativeWindow_setBuffersGeometry(app->window, 0, 0, format);
		 m_format = format;
		 surface = eglCreateWindowSurface(display, config, app->window, NULL);
		 int attrib_list[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };
		 context = eglCreateContext(display, config, EGL_NO_CONTEXT, attrib_list);

		 if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
			 LOGW("Unable to eglMakeCurrent");
			 return -1;
		 }

		 if(!eglQuerySurface(display, surface, EGL_WIDTH, &w))
			 LOGE("get egl width error");
		 if(!eglQuerySurface(display, surface, EGL_HEIGHT, &h))
			 LOGE("get egl height error");
		 width = w;
		 height = h;
		 LOGE("%d, %d",width,height);
#else
//		// the atrrib used for AMD ES emulator
//
//		EGLint attribList[] =
//		{
//			EGL_RED_SIZE,       8,
//			EGL_GREEN_SIZE,     8, 
//			EGL_BLUE_SIZE,      8,
//			EGL_ALPHA_SIZE,     8,
//			EGL_DEPTH_SIZE,     24,
//			EGL_STENCIL_SIZE,   8,
//			EGL_SAMPLE_BUFFERS, 0,
//			EGL_NONE
//		};
//
//		//Create Window;
//		WNDCLASS wndclass = {0}; 
//		DWORD    wStyle   = 0;
//		RECT     windowRect;
//		HINSTANCE hInstance = GetModuleHandle(NULL);
//		wndclass.style         = CS_OWNDC;
//		wndclass.lpfnWndProc   = (WNDPROC)BasicMsgProc::MsgProc; 
//		wndclass.hInstance     = hInstance; 
//		wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH); 
//		static int i = 0;
//		char b = 'a'+i;
//		String className = "opengles2.0";
//		className += b;
//		wndclass.lpszClassName = className.c_str() ; 
//		i++;
//		
//		if (!RegisterClass (&wndclass) ) 
//			return FALSE; 
//		wStyle = WS_VISIBLE | WS_POPUP | WS_BORDER | WS_SYSMENU | WS_CAPTION;
//
//		// Adjust the window rectangle so that the client area has
//		// the correct number of pixels
//		windowRect.left = 0;
//		windowRect.top = 0;
//		windowRect.right = width;
//		windowRect.bottom = height;
//
//		AdjustWindowRect ( &windowRect, wStyle, FALSE );
//		hWnd = CreateWindow(
//			className.c_str(),
//			className.c_str(),
//			wStyle,
//			0,
//			0,
//			windowRect.right - windowRect.left,
//			windowRect.bottom - windowRect.top,
//			NULL,
//			NULL,
//			hInstance,
//			NULL);
//		//SetWindowLongPtr (  hWnd, GWL_USERDATA, (LONG) (LONG_PTR) esContext );
//
//
//		if ( hWnd == NULL )
//			return GL_FALSE;
//
//		ShowWindow ( hWnd, TRUE );
//
//		//get egl context
//
//		EGLint numConfigs;
//		EGLint majorVersion;
//		EGLint minorVersion;
//		EGLDisplay display;
//		EGLContext context;
//		EGLSurface surface;
//		EGLConfig config;
//		EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_CONTEXT_MINOR_VERSION_KHR,1,EGL_NONE };
//	
//		// Get Display
//		display = eglGetDisplay( EGL_DEFAULT_DISPLAY);
//		if ( display == EGL_NO_DISPLAY )
//		{
//			display = eglGetDisplay(GetDC(hWnd));
//			if(display == EGL_NO_DISPLAY)
//				return EGL_FALSE;
//		}
//
//		// Initialize EGL
//		if ( !eglInitialize(display, &majorVersion, &minorVersion) )
//		{
//			return EGL_FALSE;
//		}
//
//		// Get configs
//		if ( !eglGetConfigs(display, NULL, 0, &numConfigs) )
//		{
//			return EGL_FALSE;
//		}
//
//		// Choose config
//		if ( !eglChooseConfig(display, attribList, &config, 1, &numConfigs) )
//		{
//			return EGL_FALSE;
//		}
//
//		// Create a surface
//		surface = eglCreateWindowSurface(display, config, (EGLNativeWindowType)hWnd, NULL);
//		if ( surface == EGL_NO_SURFACE )
//		{
//			return EGL_FALSE;
//		}
//
//		// Create a GL context
//		context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs );
//		std::cout<<eglGetError();
//		if ( context == EGL_NO_CONTEXT )
//		{
//			return EGL_FALSE;
//		}   
//
//		// Make the context current
//		if ( !eglMakeCurrent(display, surface, surface, context) )
//		{
//			return EGL_FALSE;
//		}
#endif
//
//		
//		eglDisplay = display;
//		eglSurface = surface;
//		eglContext = context;
//#ifdef ANDROID		
//		LOGE("display = %d,surface = %d ,context = %d",eglDisplay,eglSurface,eglContext);
//#endif

		printGLString("Version", GL_VERSION);
		printGLString("Vendor", GL_VENDOR);
		printGLString("Renderer", GL_RENDERER);
		printGLString("Extensions", GL_EXTENSIONS);

		InitFactory();
		return true;

	}

	void EGLRenderSystem::BindGpuProgram(GpuProgram *program)
	{
		if(!program) return ;

		GLGpuProgram * pro = static_cast<GLGpuProgram *>(program);
		if(!pro) return ;
		pro->NotifyRenderSystem(this);
		pro->UseProgram();

	}

	void EGLRenderSystem::SetCullMode(CullMode md)
	{
		// 
		glEnable(GL_CULL_FACE);
		switch(md)
		{
		case CULL_MODE_BACK:
			glCullFace(GL_BACK);
			break;
		case CULL_MODE_FRONT:
			glCullFace(GL_FRONT);
			break;
		case CULL_MODE_NONE:
			glDisable(GL_CULL_FACE);
			break;
		case CULL_MODE_FRONT_AND_BACK:
			glCullFace(GL_FRONT_AND_BACK);
			break;
		default:break;

		}
	}

	void EGLRenderSystem::SetFaceFront(bool clockwise /* = false */)
	{
		if(clockwise)
			glFrontFace(GL_CW);
		else
			glFrontFace(GL_CCW);
	}

	void EGLRenderSystem::EnableScissorTest(bool teston /* = false */)
	{
		if(teston)
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);
	}

	void EGLRenderSystem::SetScissorRect(int tplx,int tply,int btrx,int btry)
	{
		glScissor(tplx,tply,btrx -tplx, btry - tply);
	}
	
	void EGLRenderSystem::EnableBlend (unsigned int rendertarget_index,bool blend )
	{
		if (blend)
		{
			glEnable(GL_BLEND);
		}
		else
			glDisable(GL_BLEND);
	}

	// the  target_index is useless in opengl es 2.0 
	void EGLRenderSystem::SetBlendParam(unsigned int target_index,BlendOperand srcBlend, BlendOperand destBlend,BlendOperation blendOp,BlendOperand srcAlpha, BlendOperand destAlpha,BlendOperation alphaOp,unsigned char rtMask)
	{
		glBlendFuncSeparate(EGLUtil::Get(srcBlend),EGLUtil::Get(destBlend),EGLUtil::Get(srcAlpha),EGLUtil::Get(destAlpha));
		glBlendEquationSeparate(EGLUtil::Get(blendOp),EGLUtil::Get(alphaOp));
	}	

	void EGLRenderSystem::SetDepthParameters(ComparisonFunc func,bool depthmask)
	{
		GLboolean mask = depthmask;
		
		glDepthFunc(EGLUtil::Get(func));
		glDepthMask(mask);
	}

	void EGLRenderSystem::SetBlendFactor(float const bf[4],unsigned int sampleMask)
	{
		glBlendColor(bf[0],bf[1],bf[2],bf[3]);
	}
	void EGLRenderSystem::SetViewport(float topLeftX,float topLeftY,float _width,float _height,float mindepth /* = 0.0f */,float maxdepth /* = 1.0f */)
	{
		glViewport(topLeftX, topLeftY, _width, _height);
		assert(glGetError() == 0);
	}

	void EGLRenderSystem::SetStencilParameters(unsigned char readmask,unsigned char writemask,StencilOperation frontFailOp,
		StencilOperation frontdepthfailOp,StencilOperation frontPassOp,ComparisonFunc front, StencilOperation backFailOp,
		StencilOperation backdepthfailOp,StencilOperation backPassOp, ComparisonFunc back,unsigned refv)
	{
		//in the opengl es 2.0 ,only one mask exits so i decide use writemask 
		glStencilFuncSeparate(GL_FRONT,EGLUtil::Get(front),refv,writemask);
		glStencilFuncSeparate(GL_BACK,EGLUtil::Get(back),refv,writemask);
		glStencilOpSeparate(GL_FRONT,EGLUtil::Get(frontFailOp),EGLUtil::Get(frontdepthfailOp),EGLUtil::Get(frontPassOp));
		glStencilOpSeparate(GL_BACK,EGLUtil::Get(backFailOp),EGLUtil::Get(backdepthfailOp),EGLUtil::Get(backPassOp));

	}
	
	void EGLRenderSystem::EnableStencil(bool teston /* = false */)
	{
		if(teston)
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);
	}

	void EGLRenderSystem::BufferSwap()
	{
	/*	if(m_RTStack.empty())
		{
			if(eglSwapBuffers(eglDisplay,eglSurface) == EGL_FALSE)
			{
				Logger::WriteErrorLog(" %d egl swap false",glGetError());
			}

			return ;
		}

		RenderTarget * rt = m_RTStack.top();
		if(rt == NULL)
		{
			return;
		}

		if(rt->getType() == RenderTarget::RTT_WINDOW)
		{

			if(eglSurface == EGL_NO_SURFACE)
				return;
			if(eglDisplay == EGL_NO_DISPLAY)
			    return;

			if(eglSwapBuffers(eglDisplay,eglSurface) == EGL_FALSE)
			{
				Logger::WriteErrorLog(" %d egl swap false",glGetError());
			}
		}*/


		
	}


	void EGLRenderSystem::ClearBuffer(unsigned int buffer)
	{
		
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT| GL_STENCIL_BUFFER_BIT);
		GL_CHECK();
	}

	void EGLRenderSystem::PushRenderTarget(RenderTarget * rt)
	{
		if(rt)
		{
			m_RTStack.push(rt);
		}
	}

	void EGLRenderSystem::PopRenderTarget() 
	{
		if(!m_RTStack.empty())
		{
			m_RTStack.pop();
		}
	}

	 RenderTarget * EGLRenderSystem::CurrentRenderTarget() const
	{
		return m_RTStack.top();
	}

	 void EGLRenderSystem::ClearColor(float r,float g,float b,float a)
	 {
		
		 glClearColor(r,g,b,a);
		
		 GL_CHECK();
	 }

	 void EGLRenderSystem::setRasterState(const RasterState & rasterstate)
	 {
		 static RasterState m_last_raster_state ;
		if(rasterstate == m_last_raster_state)
			 return;
		 // possible way to optimized
		 // store a previous raster state so we can only change the states that changed 
		//Logger::WriteLog("Set Raster");
		 this->SetCullMode(rasterstate.mCullMode);
		 this->SetFaceFront(rasterstate.mFrontCounterClockwise);
		 this->EnableScissorTest(rasterstate.mScissorEnable);
		 glColorMask(rasterstate.mColorWriteMask[0],rasterstate.mColorWriteMask[1],rasterstate.mColorWriteMask[2],rasterstate.mColorWriteMask[3]);
		// if(rasterstate.mNumRects)
		//	 this->SetScissorRect(rasterstate.mRects[0].left,rasterstate.mRects[0].bottom,rasterstate.mRects[0].right,rasterstate.mRects[0].top);
		m_last_raster_state = rasterstate;
	 }

	 void EGLRenderSystem::setBlendState(const BlendState &blendstate)
	 {
		 static BlendState m_last_blend_state;
		 // possible way to optimized
		 // store a previous blend state so we can only change the states that changed 
		if(blendstate == m_last_blend_state)
			 return;
		//Logger::WriteLog("Set Blend");
		 this->EnableBlend(0,blendstate.mBlendEnable);
		 this->SetBlendFactor(blendstate.mFactor,blendstate.mSampleMask);
		 this->SetBlendParam(0,blendstate.mSrcBlend,blendstate.mDestBlend,blendstate.mBlendFunc,
			 blendstate.mSrcBlendAlpha,blendstate.mDestBlendAlpla,blendstate.mBlendFuncAlpha,blendstate.mWriteMask);
		 m_last_blend_state = blendstate;
	 }

	 void EGLRenderSystem::setDepthStencilState(const DepthStencilState & depthstencilstate)
	 {
		 static DepthStencilState m_last_depth_state;
		 if (m_last_depth_state == depthstencilstate)
		 {
			 return;
		 }
		 // depth
		// Logger::WriteLog("Set Depth");
		 this->EnableDepthTest(depthstencilstate.mDepthEnable);
		 this->SetDepthParameters(depthstencilstate.mDepthFunc,depthstencilstate.mDepthWriteMask);

		 //stencil
		 this->EnableStencil(depthstencilstate.mStencilEnable);
		 this->SetStencilParameters(depthstencilstate.mStencilReadMask,
			 depthstencilstate.mStencilWriteMask,depthstencilstate.mFrontStencilFailFunc,
			 depthstencilstate.mFrontStencilDepthFailFunc,depthstencilstate.mFrontStencilPassFunc,
			 depthstencilstate.mFrontStencilFunc,depthstencilstate.mBackStencilFailFunc,
			 depthstencilstate.mBackStencilDepthFailFunc,depthstencilstate.mBackStencilPassFunc,
			 depthstencilstate.mBackStencilFunc,depthstencilstate.mStencilRef);
		 m_last_depth_state = depthstencilstate;

	 }
	 void EGLRenderSystem::setClearState(const ClearState &clearstate)
	 {
		 
	 }
	 void EGLRenderSystem::setPolygonOffset(bool enable /* = false */, float offset_factor /* = 0.0 */, float offset_unit /* = 0.0 */)
	 {
		 if (enable)
		 {
			 glEnable(GL_POLYGON_OFFSET_FILL);
			 glPolygonOffset(offset_factor, offset_unit);
		 }
		 else
		 {
			 glDisable(GL_POLYGON_OFFSET_FILL);
		 }
	 }
	 void EGLRenderSystem::CreateGeometryGpuData(as_Geometry* rdata, VertexDataDesc* inputlayout, Geometry*& geo)
	 {
		 assert(geo == NULL);
		 geo=new GLGeometry;
		 geo->Create(rdata, inputlayout);
	 }
	 void EGLRenderSystem::DrawGeometry(Geometry* geo)
	 {
		 GLGeometry* g = static_cast<GLGeometry*>(geo);
		 g->Draw();
	 }
	 void EGLRenderSystem::InitFactory()
	 {
		 auto& grm = GlobalResourceManager::getInstance();
		 grm.m_platform_info = "GL";
		 grm.m_RenderSystem = this;
		 grm.m_TextureFactory = new EGLTextureFactory;
		 grm.m_GpuProgramFactory = new GLGpuProgramFactory;
		 grm.m_GeometryFactory = new GLGeometryFactory;
		 grm.m_RenderTargetFactory = new EGLRenderTargetFactory;
		 grm.m_SamplerFactory = new GLSamplerFactory;
	 }
#ifdef ANDROID

	 void EGLRenderSystem::GetWandHEGL(int & w,int &h)
	 {
		// if(!eglQuerySurface(eglDisplay,eglSurface,EGL_WIDTH,&w))
		//	 printf("get egl width error");
		// if(!eglQuerySurface(eglDisplay,eglSurface,EGL_HEIGHT,&h))
		//	 printf("egl get height error");
		 w = ANativeWindow_getWidth(app->window);
		 h = ANativeWindow_getHeight(app->window);
		 ANativeWindow_setBuffersGeometry(app->window,w,h,m_format);
		 printf("width = %d ,height = %d",w,h);
		 width = w;
		 height = h;
	 }
#endif

	 void EGLRenderSystem::term_display()
	 {
		 //// LOGE("term_display in rendersystem");
		 //if (EGL_NO_DISPLAY != eglDisplay)
		 //{
			// eglMakeCurrent(eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
			// if (EGL_NO_CONTEXT != eglContext)
			// {
			//	 eglDestroyContext(eglDisplay, eglContext);
			// }

			// if (EGL_NO_SURFACE != eglSurface)
			// {
			//	 eglDestroySurface(eglDisplay, eglSurface);
			// }

			// eglTerminate(eglDisplay);
		 //}

		 //eglDisplay = EGL_NO_DISPLAY;
		 //eglContext = EGL_NO_CONTEXT;
		 //eglSurface = EGL_NO_SURFACE;
	 }

}

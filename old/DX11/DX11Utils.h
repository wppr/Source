#ifndef _DX11_UTILS_H_
#define _DX11_UTILS_H_

#include "d3d11.h"
#include "DxErr.h"

#if defined(DEBUG) | defined(_DEBUG)

#ifndef HR(x)
#define HR(x) \
	{		\
		HRESULT hr = (x);	\
		if (FAILED(x)){	\
			DXTrace(__FILE__, (DWORD)__LINE__, hr, #x, true);	\
		}		\
	}
#endif

#ifndef HRRET(x)
#define HRRET(x) \
	{		\
		HRESULT hr = (x);	\
		if (FAILED(x)){	\
			DXTrace(__FILE__, (DWORD)__LINE__, hr, #x, true);	\
			return false;	\
		}		\
	}
#endif

#else
	#ifndef HR(x)
	#define HR(x) x
	#endif

	#ifndef HRRET(x)
	#define HRRET(x) x
	#endif
#endif

#ifndef ReleaseCOM(x)
#define ReleaseCOM(x) { if(x){ x->Release(); x = 0; } }

#endif

#endif
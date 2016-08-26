#ifndef _DX11_DATATYPE_HELPER_H_
#define _DX11_DATATYPE_HELPER_H_

#include "MyString.h"
#include "d3d11.h"
#include "Texture.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "RenderTarget.h"
#include "RenderSystemEnum.h"

namespace HW{
	enum DX11_DATA_TYPE{
		FLOAT2 = 0x2,
		FLOAT3,
		FLOAT4,
		FLOAT4X4,
		MATRIX,
		TEXTURE2D,
		TEXTURE3D,
		TEXTURE_CUBE,
		SAMLER_STATE
	};

	class DX11DatatypeHelper{
	public:
		static DX11_DATA_TYPE GetTypeByString(const String& typeName);
		static unsigned int GetTypeSize(DX11_DATA_TYPE type);
		static unsigned int GetTypeSizeByString(const String& typeName);

		// The Funtion Get is used for converting Render framework type into dx11 type
		static int Get(BUFFER_STORE buf_store);
		/*
		static DXGI_FORMAT ConvertToDX11Format(Texture::PixelFormat format);
		static D3D11_TEXTURE_ADDRESS_MODE ConvertToDX11SAM(Texture::TextureWarp mode);
		static D3D11_FILTER ConvertToDX11SampFilter(Texture::TextureFilter min, Texture::TextureFilter mag);
		static D3D11_USAGE ConvertToDX11Usage(HardwareUsage usage);
		static D3D11_PRIMITIVE_TOPOLOGY ConvertToDX11PrimType(PrimType geoType);
		static D3D11_BLEND ConvertToDX11Blend(BlendOperand blend);*/
		static int Get(Texture::PixelFormat format);
		static int Get(Texture::TextureWarp mode);
		static int Get(Texture::TextureFilter min, Texture::TextureFilter mag);
		static int Get(HardwareUsage usage);
		static int Get(PrimType geoType);
		static int Get(BlendOperand blend);
		static int Get(BlendOperation op);
		static int Get(ComparisonFunc func);
		static int Get(StencilOperation op);
	};
}

#endif
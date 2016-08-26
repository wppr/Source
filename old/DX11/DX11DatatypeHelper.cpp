#include "DX11DatatypeHelper.h"

namespace HW{
	const String DatatypeString[9] = {
		"FLOAT2", "FLOAT3", "FLOAT4", "FLOAT4X4", "MATRIX", "TEXTURE2D", "TEXTURE3D", "TEXTURECUBE", "SAMPLERSTATE"
	};

	DX11_DATA_TYPE DX11DatatypeHelper::GetTypeByString(const String& typeName){
		String typeStr = UpcaseString(typeName);		// get uppercase string
		for (int i = 0; i < 9; i++){
			if (typeStr == DatatypeString[i])
				return DX11_DATA_TYPE(DX11_DATA_TYPE::FLOAT2 + i);
		}
	}

	unsigned int DX11DatatypeHelper::GetTypeSize(DX11_DATA_TYPE type){
		switch (type){
		case DX11_DATA_TYPE::FLOAT2:
			return 2 * sizeof(float);
		case DX11_DATA_TYPE::FLOAT3:
			return 3 * sizeof(float);
		case DX11_DATA_TYPE::FLOAT4:
			return 4 * sizeof(float);
		case DX11_DATA_TYPE::FLOAT4X4:
		case DX11_DATA_TYPE::MATRIX:
			return 16 * sizeof(float);
		case DX11_DATA_TYPE::SAMLER_STATE:
		case DX11_DATA_TYPE::TEXTURE2D:
		case DX11_DATA_TYPE::TEXTURE3D:
			return sizeof(void*);						// They are all pointers
		}
	}

	unsigned int DX11DatatypeHelper::GetTypeSizeByString(const String& typeName){
		DX11_DATA_TYPE type = GetTypeByString(typeName);
		return GetTypeSize(type);
	}

	int DX11DatatypeHelper::Get(BUFFER_STORE buf_store){
		switch (buf_store){
		case BUFFER_STORE::BS_STATIC:
		case BUFFER_STORE::BS_STREAM:
			return D3D11_USAGE_IMMUTABLE;
		case BUFFER_STORE::BS_DYNAMIC:
			return D3D11_USAGE_DYNAMIC;
		}
	}


	// May not match, may not use
	int DX11DatatypeHelper::Get(Texture::PixelFormat format){
		DXGI_FORMAT dx11Format;
		switch (format){
		case Texture::PixelFormat::GREYSCALE:
			dx11Format = DXGI_FORMAT_A8_UNORM;	break;
		case Texture::PixelFormat::UNSIGNED_SHORT_4_4_4_4:
			dx11Format = DXGI_FORMAT_UNKNOWN;	break; // Don't know now
		case Texture::PixelFormat::UNSIGNED_SHORT_5_5_5_1:
			dx11Format = DXGI_FORMAT_B5G5R5A1_UNORM;	break;
		case Texture::PixelFormat::UNSIGNED_SHORT_5_6_5:
			dx11Format = DXGI_FORMAT_B5G6R5_UNORM;	break;
		case Texture::PixelFormat::UNSIGNED_CHAR_8_8_8:
			dx11Format = DXGI_FORMAT_UNKNOWN;	break;	// Don't know now
		case Texture::PixelFormat::UNSIGNED_CHAR_8_8_8_8:
			dx11Format = DXGI_FORMAT_R8G8B8A8_UNORM;	break;
		case Texture::PixelFormat::UNSIGNED_CHAR_10_10_10_2:
			dx11Format = DXGI_FORMAT_R10G10B10A2_UNORM;	break;
		case Texture::PixelFormat::DEPTH_COMPONENT:
			dx11Format = DXGI_FORMAT_UNKNOWN;	break;	// Dont know now
		case Texture::PixelFormat::DEPTH_COMPONENT16:
			dx11Format = DXGI_FORMAT_D16_UNORM;	break;
		case Texture::PixelFormat::DEPTH_COMPONENT24:
			dx11Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	break;
		case Texture::PixelFormat::DEPTH_COMPONENT32:
			dx11Format = DXGI_FORMAT_D32_FLOAT; break;
		case Texture::PixelFormat::FLOAT_32_32_32_32:
			dx11Format = DXGI_FORMAT_R32G32B32A32_FLOAT;	break;
		case Texture::PixelFormat::FLOAT_16_16_16_16:
			dx11Format = DXGI_FORMAT_R16G16B16A16_FLOAT;	break;
		}
		return dx11Format;
	}

	int DX11DatatypeHelper::Get(Texture::TextureWarp mode){
		switch (mode){
		case Texture::TextureWarp::TextureWarp_Clamp:
			return D3D11_TEXTURE_ADDRESS_CLAMP;
		case Texture::TextureWarp::TextureWarp_Repeat:
			return D3D11_TEXTURE_ADDRESS_WRAP;
		case Texture::TextureWarp::TextureWarp_Mirror:
			return D3D11_TEXTURE_ADDRESS_MIRROR;
		}
	}

	int DX11DatatypeHelper::Get(Texture::TextureFilter min, Texture::TextureFilter mag){
		if (min == Texture::TextureFilter::TextureFilter_Nearest){
			if (mag == Texture::TextureFilter::TextureFilter_Nearest)
				return D3D11_FILTER_MIN_MAG_MIP_POINT;
			else if (mag == Texture::TextureFilter::TextureFilter_Linear)
				return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		}
		else if (min == Texture::TextureFilter::TextureFilter_Linear){
			if (mag == Texture::TextureFilter::TextureFilter_Nearest)
				return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
			else if (mag == Texture::TextureFilter::TextureFilter_Linear)
				return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		}
	}

	int DX11DatatypeHelper::Get(HardwareUsage usage){
		switch (usage){
		case HardwareUsage::HARDWARE_USAGE_DEFAULT:
			return D3D11_USAGE_DEFAULT;
		case HardwareUsage::HARDWARE_USAGE_DYNAMIC:
			return D3D11_USAGE_DYNAMIC;
		case HardwareUsage::HARDWARE_USAGE_IMMUTABLE:
			return D3D11_USAGE_IMMUTABLE;
		case HardwareUsage::HARDWARE_USAGE_STAGING:
			return D3D11_USAGE_STAGING;
		}
	}

	int DX11DatatypeHelper::Get(PrimType geoType){
		switch (geoType){
		case PrimType::PRIM_TYPE_LINES:
			return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
		case PrimType::PRIM_TYPE_LINE_LOOP:
			return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		case PrimType::PRIM_TYPE_LINE_STRIP:
			return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		case PrimType::PRIM_TYPE_POINTS:
			return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		case PrimType::PRIM_TYPE_TRIANGLES:
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		case PrimType::PRIM_TYPE_TRIANGLE_STRIP:
			return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
		case PrimType::PRIM_TYPE_TRIANGLE_FAN:
			return D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
		}
	}

	int DX11DatatypeHelper::Get(BlendOperand blend){
		switch (blend){
		case BlendOperand::BLEND_ZERO:
			return D3D11_BLEND_ZERO;
		case BlendOperand::BLEND_ONE:
			return D3D11_BLEND_ONE;
		case BlendOperand::BLEND_SRC_COLOR:
			return D3D11_BLEND_SRC_COLOR;
		case BlendOperand::BLEND_INV_SRC_COLOR:
			return D3D11_BLEND_INV_SRC_COLOR;
		case BlendOperand::BLEND_SRC_ALPHA:
			return D3D11_BLEND_SRC_ALPHA;
		case BlendOperand::BLEND_INV_SRC_ALPHA:
			return D3D11_BLEND_INV_SRC_ALPHA;
		case BlendOperand::BLEND_DEST_ALPHA:
			return D3D11_BLEND_DEST_ALPHA;
		case BlendOperand::BLEND_INV_DEST_ALPHA:
			return D3D11_BLEND_INV_DEST_ALPHA;
		case BlendOperand::BLEND_DEST_COLOR:
			return D3D11_BLEND_DEST_COLOR;
		case BlendOperand::BLEND_INV_DEST_COLOR:
			return D3D11_BLEND_INV_DEST_COLOR;
		case BlendOperand::BLEND_SRC_ALPHA_SAT:
			return D3D11_BLEND_SRC_ALPHA_SAT;
		case BlendOperand::BLEND_BLEND_FACTOR:
			return D3D11_BLEND_BLEND_FACTOR;
		case BlendOperand::BLEND_INV_BLEND_FACTOR:
			return D3D11_BLEND_INV_BLEND_FACTOR;
		case BlendOperand::BLEND_SRC1_COLOR:
			return D3D11_BLEND_SRC1_COLOR;
		case BlendOperand::BLEND_INV_SRC1_COLOR:
			return D3D11_BLEND_INV_SRC1_COLOR;
		case BlendOperand::BLEND_SRC1_ALPHA:
			return D3D11_BLEND_SRC1_ALPHA;
		case BlendOperand::BLEND_INV_SRC1_ALPHA:
			return D3D11_BLEND_INV_SRC1_ALPHA;
		}
	}

	int DX11DatatypeHelper::Get(BlendOperation op){
		switch (op){
		case BlendOperation::BLEND_OP_ADD:
			return D3D11_BLEND_OP_ADD;
		case BlendOperation::BLEND_OP_SUBTRACT:
			return D3D11_BLEND_OP_SUBTRACT;
		case BlendOperation::BLEND_OP_REV_SUBTRACT:
			return D3D11_BLEND_OP_REV_SUBTRACT;
		case BlendOperation::BLEND_OP_MIN:
			return D3D11_BLEND_OP_MIN;
		case BlendOperation::BLEND_OP_MAX:
			return D3D11_BLEND_OP_MAX;
		}
	}

	int DX11DatatypeHelper::Get(ComparisonFunc func){
		switch (func){
		case ComparisonFunc::COMP_LESS:
			return D3D11_COMPARISON_LESS;
		case ComparisonFunc::COMP_NEVER:
			return D3D11_COMPARISON_NEVER;
		case ComparisonFunc::COMP_EQUAL:
			return D3D11_COMPARISON_EQUAL;
		case ComparisonFunc::COMP_LESS_EQUAL:
			return D3D11_COMPARISON_LESS_EQUAL;
		case ComparisonFunc::COMP_GREATER:
			return D3D11_COMPARISON_GREATER;
		case ComparisonFunc::COMP_GREATER_EQUAL:
			return D3D11_COMPARISON_GREATER_EQUAL;
		case ComparisonFunc::COMP_NOT_EQUAL:
			return D3D11_COMPARISON_NOT_EQUAL;
		case ComparisonFunc::COMP_ALWAYS:
			return D3D11_COMPARISON_ALWAYS;
		}
	}

	int DX11DatatypeHelper::Get(StencilOperation op){
		switch (op){
		case StencilOperation::STENCIL_OP_KEEP:
			return D3D11_STENCIL_OP_KEEP;
		case StencilOperation::STENCIL_OP_ZERO:
			return D3D11_STENCIL_OP_ZERO;
		case StencilOperation::STENCIL_OP_REPLACE:
			return D3D11_STENCIL_OP_REPLACE;
		case StencilOperation::STENCIL_OP_INCR:
			return D3D11_STENCIL_OP_INCR;
		case StencilOperation::STENCIL_OP_DECR:
			return D3D11_STENCIL_OP_DECR;
		case StencilOperation::STENCIL_OP_INCR_SAT:
			return D3D11_STENCIL_OP_INCR_SAT;
		case StencilOperation::STENCIL_OP_DECR_SAT:
			return D3D11_STENCIL_OP_DECR_SAT;
		case StencilOperation::STENCIL_OP_INVERT:
			return D3D11_STENCIL_OP_INVERT;
		}
	}
}
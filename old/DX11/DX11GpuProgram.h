#ifndef _DX11_GPU_PROGRAM_H_
#define _DX11_GPU_PROGRAM_H_

#include "GpuProgram.h"
#include "d3d11.h"
#include "DX11DatatypeHelper.h"
#include "MyString.h"
#include <map>

namespace HW{
	enum DX11_SHADER_TYPE{
		DX11_ST_VERTEX = 0x01,			// 00000001
		DX11_ST_HULL = 0x02,				// 00000010
		DX11_ST_DOMAIN = 0x04,		// 00000100
		DX11_ST_GEOMETRY = 0x08,		// 00001000
		DX11_ST_PIXEL = 0x1F				// 00010000
	};

	enum DX11_ELEMENT_TYPE{
		CBUFFER_CONST,		// the cbuffer constants
		TEXTURE,					// the texture variable
		SAMPLER_STATE 		// the sampler state variable
	};

	struct DX11CBuffer{	// The structure to store cbuffer data
		UINT BindPoint[5];	// The bind index in every shader
		unsigned char* pData;		// The buffer to store cbuffer datas in CPU memory
		UINT uSize;
		bool bDirty;		// flag whether this cbuffer should update
		unsigned char Belong;	// to flag the shaders which this cbuffer belongs to
		ID3D11Buffer* CBuffer;	// The Buffer resources
		DX11CBuffer(){ 
			pData = NULL;
			uSize = 0;
			bDirty = true;
			Belong = 0;
			CBuffer = 0;
		}
	};

	struct DX11Uniform{
		//String Name;						// the Variable name
		DX11_ELEMENT_TYPE eElementType;		// the element type
		//DX11_DATA_TYPE eDataType;		// the data type
		UINT BindPoint[5];							// the index among this kind of element, ex. texture index in every shader, no use for cbuffer
		UINT uCount;						// Maybe array
		String CBufferName;
		D3D_SRV_DIMENSION Dimension;				// The dimension if texture
		UINT uOffset;						// the offset in cbuffer
		UINT uSize;							// the size of this variable
		unsigned char Belong;		// the uniform variable belong, no use for cbuffer consts
		bool bDirty;							// no use for cbuffer_const
		void **ppResource;					// The resource pointer array, ex. ID3D11ShaderResourceView*   or   ID3D11SamplerState*
		DX11Uniform(){
			bDirty = true;				// First time
			Belong = 0;
			ppResource = NULL;
		}
	};

	class DX11GpuProgram : public GpuProgram{
	public:
		DX11GpuProgram();
		DX11GpuProgram(const String& name);
		virtual ~DX11GpuProgram();

		// Compile and link program
		virtual bool CompileAndLink();
		virtual unsigned int GetErrorMessageSize() const{ return 0; };
		virtual void GetErrorMessage(void* msg, unsigned int size){};

		// Set the current gpu program as this
		virtual void UseProgram();

		// Apply all the updates of constants
		virtual void ApplyUpdate();

		// Update textures used by this program at rendering time
		virtual void UpdateTexture(const String& name, Texture* texture);
		virtual void UpdateGlobalVariable(GlobalVariablePool* glob);
		virtual void UpdateConstantVariable(const String& name, unsigned int size, void* val, bool int_flag);
		
		// submit all the changes to the video memory
		virtual void Submit();
		// Is not used yet
		virtual bool ParseProgramFromJson(String filename){ return false; };
		virtual void releaseInternalRes();

		/** update the programData into program's uniform;
		@remarks the program_data could be the program defualt data in m_ProgramData,or pass by material's
		@param program_data is an map<string,ProgramData> which contain the name and the value of
		uniform parameters;
		*/
		virtual void updateProgramData(const std::map<String, ProgramData>& program_data);

		//void SetProgramCBuffer(std::vector<DX11CBuffer>& cbuffers);

	private:
		// set uniforms description by compiled shader buffer
		void SetUniforms(void* shaderBuffer, UINT shaderBufferSize, DX11_SHADER_TYPE shaderType);
		// Create uniform resources by the description that has been set by the function above
		void CreateUniformResources();
		// Get the index in shaderType, ex. VS = 1, HS = 2, etc.
		int GetShaderTypeIndex(DX11_SHADER_TYPE type);
		// Create Texture Resource View By Dimension
		//void CreateTextureByDimension(ID3D11Resource*& pResource, D3D_SRV_DIMENSION dimension);

	private:
		std::map<String, DX11CBuffer> m_mCBuffers;			// the cbuffer to store uniform constants, the first is cbuffer name
		std::map<String, DX11Uniform> m_mUniforms;		// The description of uniform variables
		ID3D11VertexShader * m_pVS;
		ID3D11HullShader* m_pHS;
		ID3D11DomainShader* m_pDS;
		ID3D11GeometryShader* m_pGS;
		ID3D11PixelShader* m_pPS;
	};

	class DX11GpuProgramFactory : public GpuProgramFactory{
	public:
		DX11GpuProgramFactory(){}
		virtual ~DX11GpuProgramFactory(){}
		virtual DX11GpuProgram* CreateGpuProgram(const String& name);
		virtual DX11GpuProgram* CreateGpuProgram();
	};
}

#endif
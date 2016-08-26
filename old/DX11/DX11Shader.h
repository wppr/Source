#ifndef _DX11_SHADER_H_
#define _DX11_SHADER_H_

#include "d3d11.h"
#include "Shader.h"
#include "DX11InputLayout.h"

namespace HW{
	class DX11InputLayout;
	class DX11Shader : public Shader{
	public:
		DX11Shader(ShaderType type);
		DX11Shader(ShaderType type, const String& name);
		virtual ~DX11Shader();
		virtual bool loadShaderFromFile(const char* filename, const char* entryFunc, const char* mode);
		// Dont read xml shader yet
		virtual bool ParseShaderFromXml(rapidxml::xml_node<> * node){ return false; }
		// Dont read shader from json yet
		virtual bool ParseShaderFromJson(rapidjson::Document& d);
		virtual unsigned int getErrorMessageSize() const;
		virtual void getErrorMessage(char* dest, unsigned int size);
		virtual InputLayout* getInputLayout();

		virtual void releaseInternalRes();

		/*
		void* GetShader() const{
			return m_pShader;
		}*/
		void* GetShaderBuffer() const{
			return m_pShaderBlob->GetBufferPointer();
		}
		SIZE_T GetShaderBufferSize() const{
			return m_pShaderBlob->GetBufferSize();
		}

	private:
		bool LoadShader(ShaderType shaderType, ID3D10Blob* blob);	// Compile the shader by the shaderType
		ID3D10Blob* m_pShaderBlob;					// The buffer used to store the compiled shader
		ID3D10Blob* m_pCompilationMsgBlob;	// The buffer used to store the compilation error message
		DX11InputLayout* m_pInputLayout;
	//	void* m_pShader;						// Dynamic determination of Shader Type
	//	bool m_bIsReleased;				// To flag whether the Internal resource has been released
	};

	class DX11ShaderFactory : public ShaderFactory{
	public:
		DX11ShaderFactory(){}
		virtual ~DX11ShaderFactory(){}

		virtual Shader* CreateShader(const String& name, ShaderType shaderType);
		virtual Shader* CreateShader(ShaderType shaderType);
	};
}

#endif
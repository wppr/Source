#include "DX11Shader.h"
#include "d3dx11.h"
#include "DxErr.h"
#include "DX11RenderSystem.h"
#include "DX11Utils.h"
#include "EngineUtil.h"

namespace HW{
	DX11Shader::DX11Shader(ShaderType shaderType, const String& name)
		: Shader(shaderType, name){
		m_pShaderBlob = NULL;
		m_pCompilationMsgBlob = NULL;
		m_pInputLayout = NULL;
	}

	DX11Shader::DX11Shader(ShaderType shaderType)
		: Shader(shaderType){
		m_pShaderBlob = NULL;
		m_pCompilationMsgBlob = NULL;
		m_pInputLayout = NULL;
	}

	DX11Shader::~DX11Shader(){
		releaseInternalRes();
		if (m_pInputLayout)
			delete m_pInputLayout;
	}

	// The "mode" is used to flag the version of shader, such as "vs_5_0"
	bool DX11Shader::loadShaderFromFile(const char* filename, const char* entryFunc, const char* mode){
//		releaseInternalRes();		// Ensure that the resources used before has been released
		UINT flags = 0;
		// set Row Major, so we needn't transpose matrix before set it to GPU
		flags |= D3D10_SHADER_ENABLE_STRICTNESS | D3D10_SHADER_PACK_MATRIX_ROW_MAJOR;

		HRESULT hr = D3DX11CompileFromFile(
									filename, 0, 0,
									entryFunc, mode, flags,
									0, 0, &m_pShaderBlob,
									&m_pCompilationMsgBlob, 0);
		if (FAILED(hr)){
			DXTrace(__FILE__, (DWORD)__LINE__, hr, "D3DX11CompileFromFile Failed.", true);
			m_valid = false;
			return false;
		}
		m_valid = true;
		return true;
	}
	
	unsigned int DX11Shader::getErrorMessageSize() const{
		if (m_pCompilationMsgBlob)
			return m_pCompilationMsgBlob->GetBufferSize();
	}

	void DX11Shader::getErrorMessage(char* dest, unsigned int size){
		if (m_pCompilationMsgBlob)
			memcpy(dest, m_pCompilationMsgBlob->GetBufferPointer(), size);
	}

	InputLayout* DX11Shader::getInputLayout(){
		return m_pInputLayout;
	}

	void DX11Shader::releaseInternalRes(){
		ReleaseCOM(m_pCompilationMsgBlob);
		ReleaseCOM(m_pShaderBlob);
		m_valid = false;
	}

	bool DX11Shader::ParseShaderFromJson(rapidjson::Document& d){
		if (m_ShaderType == ST_VERTEX)
		{
			rapidjson::Value& attributes = d["input"]["attribute"];

			m_pInputLayout = new DX11InputLayout();
			for (auto it = attributes.MemberBegin(); it != attributes.MemberEnd(); it++){
				DX11InputDesc desc;
				desc.Name = it->name.GetString();
				desc.Semantic = it->value.FindMember("semantic")->value.GetString();
				desc.eDataType = (DX11_DATA_TYPE)it->value.FindMember("Length")->value.GetInt();
				desc.uSemanticIndex = it->value.FindMember("index")->value.GetInt();
				m_pInputLayout->m_Element.push_back(desc);
			}

			m_pInputLayout->m_IPLKey.Length = m_pInputLayout->m_Element.size();
			m_pInputLayout->m_IPLKey.keys = new unsigned int[m_pInputLayout->m_IPLKey.Length];

			for (unsigned int i = 0; i < m_pInputLayout->m_Element.size(); i++)
			{
				String semantic = m_pInputLayout->m_Element[i].Semantic;
				UINT index = m_pInputLayout->m_Element[i].uSemanticIndex;
				unsigned int & key = m_pInputLayout->m_IPLKey.keys[i];
				unsigned int temp = 0;
				if (semantic == "POSITION" && index == 0)
				{
					key = 0x10000000;
					temp = 0;
				}
				else if (semantic == "NORMAL" && index == 0)
				{
					key = 0x20000000;
					temp = 1;
				}
				else if (semantic == "TEXCOORD" && index == 0)
				{
					key = 0x30000000;
					temp = 2;
				}
				else if (semantic == "POSITION" && index == 1)
				{
					key = 0x40000000;
					temp = 3;
				}
				else if (semantic == "NORMAL" && index == 1)
				{
					key = 0x50000000;
					temp = 4;
				}
				else if (semantic == "TEXCOORD" && index == 1)
				{
					key = 0x60000000;
					temp = 5;
				}
				else if (semantic == "BONEINDEX")
				{
					key = 0x70000000;
					temp = 6;
				}
				else if (semantic == "BONEWEIGHT")
				{
					key = 0x80000000;
					temp = 7;
				}
				temp = temp << 24;
				key |= temp;
			}
		}

		String srcname, entry, mode;
		if (m_ShaderType == ST_VERTEX){
			srcname = d["vertex shader"]["src"].GetString();
			entry = d["vertex shader"]["entry"].GetString();
			mode = d["vertex shader"]["mode"].GetString();
		}

		if (m_ShaderType == ST_PIXEL){
			srcname = d["fragment shader"]["src"].GetString();
			entry = d["fragment shader"]["entry"].GetString();
			mode = d["fragment shader"]["mode"].GetString();
		}
		Logger::WriteLog("src_name :%s", srcname.c_str());
		String valid_src_name = FileUtil::getShaderFileValidPath(srcname);
		Logger::WriteLog("valid_src_name :%s", valid_src_name.c_str());

		releaseInternalRes();

		if (!loadShaderFromFile(valid_src_name.c_str(), entry.c_str(), mode.c_str()))
			return false;
		return true;
	}

	Shader* DX11ShaderFactory::CreateShader(const String& name, ShaderType shaderType){
		return new DX11Shader(shaderType, name);
	}

	Shader* DX11ShaderFactory::CreateShader(ShaderType shaderType){
		return new DX11Shader(shaderType);
	}
}
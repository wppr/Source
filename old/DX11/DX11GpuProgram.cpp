#include "DX11GpuProgram.h"
#include "DX11Shader.h"
#include "DX11InputLayout.h"
#include "DX11RenderSystem.h"
#include "DX11Utils.h"
#include "DX11Texture.h"
#include "ShaderManager.h"
#include "D3DCompiler.h"
#include "GlobalVariableSemantics.h"
#include "GlobalVariablePool.h"
#include "MyString.h"

namespace HW{
	

	DX11GpuProgram::DX11GpuProgram(){
		m_pVS = NULL;
		m_pHS = NULL;
		m_pDS = NULL;
		m_pGS = NULL;
		m_pPS = NULL;
	}

	DX11GpuProgram::DX11GpuProgram(const String& name) : GpuProgram(name){
		m_pVS = NULL;
		m_pHS = NULL;
		m_pDS = NULL;
		m_pGS = NULL;
		m_pPS = NULL;
	}

	DX11GpuProgram::~DX11GpuProgram(){
		releaseInternalRes();
	}

	void DX11GpuProgram::releaseInternalRes(){
		// if the gpuprogram has not been released
		if (m_valid){
			ReleaseCOM(m_pVS);
			ReleaseCOM(m_pHS);
			ReleaseCOM(m_pDS);
			ReleaseCOM(m_pGS);
			ReleaseCOM(m_pPS);
			for (auto& it = m_mCBuffers.begin(); it != m_mCBuffers.end(); it++){
				if (it->second.pData)
					delete it->second.pData;
				ReleaseCOM(it->second.CBuffer);
			}
			for (auto& it = m_mUniforms.begin(); it != m_mUniforms.end(); it++){
				DX11Uniform uniform = it->second;
				if (uniform.ppResource){				// The ppResource field has data
					if (uniform.eElementType == DX11_ELEMENT_TYPE::SAMPLER_STATE){
						for (int i = 0; i < uniform.uCount; i++){
							ID3D11SamplerState *pTemp = reinterpret_cast<ID3D11SamplerState*>(uniform.ppResource[i]);
							ReleaseCOM(pTemp);
						}
					}
					else if (uniform.eElementType == DX11_ELEMENT_TYPE::TEXTURE){
						for (int i = 0; i < uniform.uCount; i++){
							ID3D11ShaderResourceView *pTemp = reinterpret_cast<ID3D11ShaderResourceView*>(uniform.ppResource[i]);
							ReleaseCOM(pTemp);
						}
					}
					delete[] uniform.ppResource;
				}
			}
		}
	}

	// Compile all the shaders and set uniforms
	bool DX11GpuProgram::CompileAndLink(){
		// Return false if vs or ps doesn't exist
		if (m_ShaderList[ST_VERTEX] == NULL || m_ShaderList[ST_PIXEL] == NULL)
			return false;
		// Clear the resources that may have not been released
		releaseInternalRes();
		DX11Shader* dx11vs = dynamic_cast<DX11Shader*>(m_ShaderList[ST_VERTEX]);
		DX11Shader* dx11ps = dynamic_cast<DX11Shader*>(m_ShaderList[ST_PIXEL]);
		DX11Shader* dx11hs = dynamic_cast<DX11Shader*>(m_ShaderList[ST_HULL]);
		DX11Shader* dx11ds = dynamic_cast<DX11Shader*>(m_ShaderList[ST_DOMAIN]);
		DX11Shader* dx11gs = dynamic_cast<DX11Shader*>(m_ShaderList[ST_GEOMETRY]);
		DX11InputLayout* dx11InputLayout = dynamic_cast<DX11InputLayout*>(dx11vs->getInputLayout());
		DX11RenderSystem* renderSystem = dynamic_cast<DX11RenderSystem*>(m_RenderSystem);
		assert(renderSystem && "Render System Cannot be NULL");
		ID3D11DeviceContext* immediateContext = renderSystem->GetDX11Context();
		ID3D11Device* device = renderSystem->GetDX11Device();

		// create all the shaders and uniforms
		HRRET(device->CreateVertexShader(dx11vs->GetShaderBuffer(), dx11vs->GetShaderBufferSize(), NULL, &m_pVS));
		SetUniforms(dx11vs->GetShaderBuffer(), dx11vs->GetShaderBufferSize(), DX11_SHADER_TYPE::DX11_ST_VERTEX);
		HRRET(device->CreatePixelShader(dx11ps->GetShaderBuffer(), dx11ps->GetShaderBufferSize(), NULL, &m_pPS));
		SetUniforms(dx11vs->GetShaderBuffer(), dx11vs->GetShaderBufferSize(), DX11_SHADER_TYPE::DX11_ST_PIXEL);
		if (dx11hs){
			HRRET(device->CreateHullShader(dx11hs->GetShaderBuffer(), dx11hs->GetShaderBufferSize(), NULL, &m_pHS));
			SetUniforms(dx11vs->GetShaderBuffer(), dx11vs->GetShaderBufferSize(), DX11_SHADER_TYPE::DX11_ST_HULL);
		}
		if (dx11ds){
			HRRET(device->CreateDomainShader(dx11ds->GetShaderBuffer(), dx11ds->GetShaderBufferSize(), NULL, &m_pDS));
			SetUniforms(dx11vs->GetShaderBuffer(), dx11vs->GetShaderBufferSize(), DX11_SHADER_TYPE::DX11_ST_DOMAIN);
		}
		if (dx11gs){
			HRRET(device->CreateGeometryShader(dx11gs->GetShaderBuffer(), dx11gs->GetShaderBufferSize(), NULL, &m_pGS));
			SetUniforms(dx11vs->GetShaderBuffer(), dx11vs->GetShaderBufferSize(), DX11_SHADER_TYPE::DX11_ST_GEOMETRY);
		}
		m_valid = true;
		CreateUniformResources();

		// Make the whole sematic varName map
		if (dx11vs){		// VS exists
			const std::map<unsigned int, String>& vmap = dx11vs->getVariableDescription();
			for (auto it = vmap.begin(); it != vmap.end(); it++)
				m_SemanticVariableMap.insert(std::make_pair(it->first, it->second));
		}
		if (dx11ps){
			const std::map<unsigned int, String>& vmap = dx11ps->getVariableDescription();
			for (auto it = vmap.begin(); it != vmap.end(); it++)
				m_SemanticVariableMap.insert(std::make_pair(it->first, it->second));
		}

		return true;
	}

	// Apply all the updates of constants
	void DX11GpuProgram::ApplyUpdate(){
		DX11RenderSystem* renderSystem = dynamic_cast<DX11RenderSystem*>(m_RenderSystem);
		ID3D11DeviceContext* immediateContext = renderSystem->GetDX11Context();

		// Apply updates of CBuffers
#pragma region UpdateCBuffers
		for (auto& it = m_mCBuffers.begin(); it != m_mCBuffers.end(); it++){
			// We just update the cbuffer to GPU if the dirty is true
			DX11CBuffer cb = it->second;
			if (cb.bDirty){
				assert(cb.pData);			// The pData cannot be NULL
				D3D11_MAPPED_SUBRESOURCE mappedData;
				HR(immediateContext->Map(cb.CBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData));
				memcpy(mappedData.pData, cb.pData, cb.uSize);		// copy the data to the mapped position
				immediateContext->Unmap(cb.CBuffer, 0);		// Unmap
				// Set the constant buffers to all the shader by judging "Belong"
				if (cb.Belong & DX11_SHADER_TYPE::DX11_ST_VERTEX)
					immediateContext->VSSetConstantBuffers(cb.BindPoint[0], 1, &cb.CBuffer);
				if (cb.Belong & DX11_SHADER_TYPE::DX11_ST_HULL)
					immediateContext->HSSetConstantBuffers(cb.BindPoint[1], 1, &cb.CBuffer);
				if (cb.Belong & DX11_SHADER_TYPE::DX11_ST_DOMAIN)
					immediateContext->DSSetConstantBuffers(cb.BindPoint[2], 1, &cb.CBuffer);
				if (cb.Belong & DX11_SHADER_TYPE::DX11_ST_GEOMETRY)
					immediateContext->GSSetConstantBuffers(cb.BindPoint[3], 1, &cb.CBuffer);
				if (cb.Belong & DX11_SHADER_TYPE::DX11_ST_PIXEL)
					immediateContext->PSSetConstantBuffers(cb.BindPoint[4], 1, &cb.CBuffer);
				cb.bDirty = false;			// Change the dirty
			}
		}
#pragma endregion
		// Apply updates of Other uniforms
		for (auto& it = m_mUniforms.begin(); it != m_mUniforms.end(); it++){
			DX11Uniform& uniformDesc = it->second;
			// The CBuffer const has been handled above
			if (uniformDesc.eElementType == DX11_ELEMENT_TYPE::CBUFFER_CONST)
				continue;
			// We only update if bDirty is true
			if (uniformDesc.bDirty){
				// Handle with Texture
				if (uniformDesc.eElementType == DX11_ELEMENT_TYPE::TEXTURE){
#pragma region UpdateTextures
					ID3D11ShaderResourceView** ppsrv = reinterpret_cast<ID3D11ShaderResourceView**>(uniformDesc.ppResource);
					if (uniformDesc.Belong & DX11_SHADER_TYPE::DX11_ST_VERTEX)
						immediateContext->VSSetShaderResources(uniformDesc.BindPoint[0], uniformDesc.uCount, ppsrv);
					if (uniformDesc.Belong & DX11_SHADER_TYPE::DX11_ST_HULL)
						immediateContext->HSSetShaderResources(uniformDesc.BindPoint[1], uniformDesc.uCount, ppsrv);
					if (uniformDesc.Belong & DX11_SHADER_TYPE::DX11_ST_DOMAIN)
						immediateContext->DSSetShaderResources(uniformDesc.BindPoint[2], uniformDesc.uCount, ppsrv);
					if (uniformDesc.Belong & DX11_SHADER_TYPE::DX11_ST_GEOMETRY)
						immediateContext->GSSetShaderResources(uniformDesc.BindPoint[3], uniformDesc.uCount, ppsrv);
					if (uniformDesc.Belong & DX11_SHADER_TYPE::DX11_ST_PIXEL)
						immediateContext->PSSetShaderResources(uniformDesc.BindPoint[4], uniformDesc.uCount, ppsrv);
#pragma endregion
				}
				// Handle with SamplerStates
				else{
#pragma region UpdateSamplers
					ID3D11SamplerState** ppSampler = reinterpret_cast<ID3D11SamplerState**>(uniformDesc.ppResource);
					if (uniformDesc.Belong & DX11_SHADER_TYPE::DX11_ST_VERTEX)
						immediateContext->VSSetSamplers(uniformDesc.BindPoint[0], uniformDesc.uCount, ppSampler);
					if (uniformDesc.Belong & DX11_SHADER_TYPE::DX11_ST_HULL)
						immediateContext->HSSetSamplers(uniformDesc.BindPoint[1], uniformDesc.uCount, ppSampler);
					if (uniformDesc.Belong & DX11_SHADER_TYPE::DX11_ST_DOMAIN)
						immediateContext->DSSetSamplers(uniformDesc.BindPoint[2], uniformDesc.uCount, ppSampler);
					if (uniformDesc.Belong & DX11_SHADER_TYPE::DX11_ST_GEOMETRY)
						immediateContext->GSSetSamplers(uniformDesc.BindPoint[3], uniformDesc.uCount, ppSampler);
					if (uniformDesc.Belong & DX11_SHADER_TYPE::DX11_ST_PIXEL)
						immediateContext->PSSetSamplers(uniformDesc.BindPoint[4], uniformDesc.uCount, ppSampler);
#pragma endregion
				}
				uniformDesc.bDirty = false;			// Change the dirty
			}
		}
	}

	void DX11GpuProgram::Submit(){
		ApplyUpdate();
	}

	// Set all the shaders as current shaders
	void DX11GpuProgram::UseProgram(){
		if (!m_valid)
			return; 
		DX11RenderSystem* renderSystem = dynamic_cast<DX11RenderSystem*>(m_RenderSystem);
		ID3D11DeviceContext* immediateContext = renderSystem->GetDX11Context();
		
		// Set all the shaders as current shaders
		immediateContext->VSSetShader(m_pVS, 0, 0);
		immediateContext->PSSetShader(m_pPS, 0, 0);
		if (m_pHS)
			immediateContext->HSSetShader(m_pHS, 0, 0);
		if (m_pDS)
			immediateContext->DSSetShader(m_pDS, 0, 0);
		if (m_pGS)
			immediateContext->GSSetShader(m_pGS, 0, 0);

		ShaderManager* shaderMgr = ShaderManager::getSingletonPtr();
		shaderMgr->SetCurrent(this);
	}

	// Update the texture and relative sampler states
	void DX11GpuProgram::UpdateTexture(const String& name, Texture* texture){
		DX11Texture* pTexture = dynamic_cast<DX11Texture*>(texture);
		ID3D11Resource * pResource = pTexture->GetResource();		// Get the texture resource
		//Prevent NULL pointer error
		assert(pResource);
		auto& it = m_mUniforms.find(name);
		// This uniform is not found or not texture
		if (it == m_mUniforms.end() || it->second.eElementType != DX11_ELEMENT_TYPE::TEXTURE)			
			return;

		DX11RenderSystem * pRenderSystem = dynamic_cast<DX11RenderSystem*>(m_RenderSystem);
		ID3D11Device* pDevice = pRenderSystem->GetDX11Device();
		DX11Uniform& texUniform = it->second;
		texUniform.bDirty = true;
		if (texUniform.ppResource[0])
			delete texUniform.ppResource[0];
		HR(pDevice->CreateShaderResourceView(pResource, 0, (ID3D11ShaderResourceView**)&texUniform.ppResource[0]));

		it = m_mUniforms.find(pTexture->GetSamplerStateName());
		if (it == m_mUniforms.end())		// This sampler is not found
			return;
		DX11Uniform& sampUniform = it->second;
		sampUniform.bDirty = true;
		if (sampUniform.ppResource[0])
			delete sampUniform.ppResource[0];
		const D3D11_SAMPLER_DESC& sampDesc = pTexture->GetSamplerDesc();
		HR(pDevice->CreateSamplerState(&sampDesc, (ID3D11SamplerState**)&sampUniform.ppResource[0]));
	}

	// Update constant buffer constant used by this program at rendering time
	void DX11GpuProgram::UpdateConstantVariable(const String& name, unsigned int size, void* val, bool int_flag){
		auto& it = m_mUniforms.find(name);
		if (it == m_mUniforms.end())
			return;
		DX11Uniform& uniform = it->second;
		assert(size == uniform.uSize);			// error when the size not match
		auto& cbMap = m_mCBuffers.find(uniform.CBufferName);		// Get the cbuffer
		DX11CBuffer& cb = cbMap->second;
		// Set the value to the cbuffer memory in CPU
		memcpy(&cb.pData[uniform.uOffset], val, size);
		cb.bDirty = true;				// Set Dirty
	}

	// the public interface that used to set all the uniforms
	void DX11GpuProgram::updateProgramData(const std::map<String, ProgramData>& program_data){
		for (auto& it = program_data.begin(); it != program_data.end(); it++){
			// Get the uniform with this name
			auto& uniform = m_mUniforms.find(it->first);
			if (uniform == m_mUniforms.end()){		// This uniform is not Found
				Logger::WriteErrorLog("The uniform:%s is not found!\n", it->first);
				continue;
			}
			// Handle with cbuffer constants
			if (it->second.type == SDT_CONSTANT){
				if (uniform->second.eElementType != DX11_ELEMENT_TYPE::CBUFFER_CONST){
					Logger::WriteErrorLog("The datatype of uniform:%s is not match.\n", it->first);
					continue;
				}	// Update the constant variable
				UpdateConstantVariable(it->first, it->second.size, it->second.data, it->second.int_flag);
			}
			// Handle with Texture resources
			else if (it->second.type == SDT_TEXTURE){
				if (uniform->second.eElementType != DX11_ELEMENT_TYPE::TEXTURE){
					Logger::WriteErrorLog("The datatype of uniform:%s is not match.\n", it->first);
					continue;
				}	// Update the Texture resources
				UpdateTexture(it->first, (Texture*)it->second.data);
			}
		}
	}

	int DX11GpuProgram::GetShaderTypeIndex(DX11_SHADER_TYPE type){
		int typeInt = type;
		int i;
		for (i = 0; i < 5; i++, typeInt >>= 1){
			if (typeInt & 0x01)
				break;
		}
		return i;
	}

	void DX11GpuProgram::SetUniforms(void* shaderBuffer, UINT shaderBufferSize, DX11_SHADER_TYPE type){
		ID3D11ShaderReflection* sr = NULL;
		int shaderTypeIndex = GetShaderTypeIndex(type);
		// Get shader reflection
		HR(D3DReflect(shaderBuffer, shaderBufferSize, IID_ID3D11ShaderReflection, (void**)sr));
		D3D11_SHADER_DESC shaderDesc;
		HR(sr->GetDesc(&shaderDesc));
		for (int i = 0; i < shaderDesc.BoundResources; i++){
			D3D11_SHADER_INPUT_BIND_DESC desc;
			HR(sr->GetResourceBindingDesc(i, &desc));
			if (desc.Type == D3D_SIT_CBUFFER){		// The resource is cbuffer
#pragma region GetCBufferDesc
				DX11CBuffer cb;
				D3D11_SHADER_BUFFER_DESC cbDesc;
				// Get constant buffer
				ID3D11ShaderReflectionConstantBuffer* srcb = sr->GetConstantBufferByIndex(desc.BindPoint);
				srcb->GetDesc(&cbDesc);					// Get CBuffer Desc
				std::map<String, DX11CBuffer>::iterator it = m_mCBuffers.find(cbDesc.Name);
				if (it != m_mCBuffers.end()){		// We have insert this cbuffer for other shader
					it->second.Belong |= type;	// Add the belong
					continue;
				}

				cb.Belong |= type;							// A CBuffer may belong to many shaders
				cb.BindPoint[shaderTypeIndex] = desc.BindPoint;		// Set BindPoint (real index)
				cb.uSize = cbDesc.Size;
				m_mCBuffers.insert(std::make_pair(cbDesc.Name, cb));
				for (int j = 0; j < cbDesc.Variables; j++){
					DX11Uniform uniformDesc;
					D3D11_SHADER_VARIABLE_DESC varDesc;
					ID3D11ShaderReflectionVariable* srv = srcb->GetVariableByIndex(j);
					srv->GetDesc(&varDesc);
					String name = varDesc.Name;			// The name including "[n]" for array, so we have to cut it
					int pos = name.find('[');
					if (pos != String::npos)		// Array, find '['
						name.replace(pos, 3, "");
					uniformDesc.CBufferName = cbDesc.Name;			// The array name including "[n]" ?
					uniformDesc.uSize = varDesc.Size;
					uniformDesc.eElementType = DX11_ELEMENT_TYPE::CBUFFER_CONST;
					uniformDesc.uOffset = varDesc.StartOffset;
					m_mUniforms.insert(std::make_pair(name, uniformDesc));
				}
#pragma endregion
			}
			else{
				String name = desc.Name;			// The name including "[n]" for array, so we have to cut it
				int pos = name.find('[');
				if (pos != String::npos)		// Array, find '['
					name.replace(pos, 3, "");
				std::map<String, DX11Uniform>::iterator it = m_mUniforms.find(name);
				if (it != m_mUniforms.end()){		// We have insert this uniform for other shader
					it->second.Belong |= type;
					continue;
				}
				if (desc.Type == D3D_SIT_TEXTURE){
#pragma region GetTextureDesc
					DX11Uniform uniformDesc;
					uniformDesc.eElementType = DX11_ELEMENT_TYPE::TEXTURE;
					uniformDesc.Dimension = desc.Dimension;
					uniformDesc.BindPoint[shaderTypeIndex] = desc.BindPoint;
					uniformDesc.uCount = desc.BindCount;
					m_mUniforms.insert(std::make_pair(name, uniformDesc));
#pragma endregion
				}
				else if (desc.Type == D3D_SIT_SAMPLER){
#pragma region GetSamplerDesc
					DX11Uniform uniformDesc;
					uniformDesc.eElementType = DX11_ELEMENT_TYPE::SAMPLER_STATE;
					uniformDesc.BindPoint[shaderTypeIndex] = desc.BindPoint;
					uniformDesc.uCount = desc.BindCount;
					m_mUniforms.insert(std::make_pair(name, uniformDesc));
#pragma endregion
				}
			}
		}
		ReleaseCOM(sr);
	}

	// Create Texture Resource View By Dimension
	/*
	void DX11GpuProgram::CreateTextureByDimension(ID3D11Resource*& pResource, D3D_SRV_DIMENSION dimension){
		switch (dimension){
		case D3D_SRV_DIMENSION_TEXTURE2D:
			D3D11_TEXTURE2D_DESC tex2dDesc;
			tex2dDesc.ArraySize = 1;
			tex2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			tex2dDesc.CPUAccessFlags = 
		}
	}*/

	void DX11GpuProgram::CreateUniformResources(){
		if (m_valid)			// if the resources has been created, then return
			return;

		DX11RenderSystem* renderSystem = dynamic_cast<DX11RenderSystem*>(m_RenderSystem);
		ID3D11Device* device = renderSystem->GetDX11Device();
		
		// Create CBuffer Resources
		for (auto& it = m_mCBuffers.begin(); it != m_mCBuffers.end(); it++){
			DX11CBuffer cb = it->second;
			// Set CBuffer resource description
			D3D11_BUFFER_DESC cbDesc;
			cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbDesc.ByteWidth = cb.uSize;
			cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbDesc.MiscFlags = 0;
			cbDesc.Usage = D3D11_USAGE_DYNAMIC;
			// Create CBuffer in GPU Memory
			HR(device->CreateBuffer(&cbDesc, 0, &cb.CBuffer));
			// Allocate space for cbuffer data in CPU Memory
			cb.pData = new unsigned char[cb.uSize];
		}
		
		for (auto& it = m_mUniforms.begin(); it != m_mUniforms.end(); it++){
			DX11Uniform& uniform = it->second;
			// Create Texture Resources
			if (uniform.eElementType == DX11_ELEMENT_TYPE::TEXTURE){
				uniform.ppResource = (void **)new ID3D11ShaderResourceView*[uniform.uCount];
				for (int i = 0; i < uniform.uCount; i++)
					uniform.ppResource[i] = NULL;
			}

			// Create SamplerState Resources
			else if (uniform.eElementType == DX11_ELEMENT_TYPE::SAMPLER_STATE){
				uniform.ppResource = (void**)new ID3D11SamplerState*[uniform.uCount];
				for (int i = 0; i < uniform.uCount; i++)
					uniform.ppResource[i] = NULL;
			}
		}
	}

	// Update the Uniforms that semantic is not GVS_MANUAL_PARA in Global Pool
	void DX11GpuProgram::UpdateGlobalVariable(GlobalVariablePool* glob){
		unsigned int size;
		for (std::map<unsigned int, String>::iterator it = m_SemanticVariableMap.begin(); it != m_SemanticVariableMap.end(); it++){
			GlobalVariableSemantic key = _global_getSemanticKey(it->first);
			unsigned int element_num = _global_getSemanticNum(it->first);
			switch (key){
			// matrix type
			case GVS_WORLD_MATRIX:
			case GVS_VIEW_MATRIX:
			case GVS_PROJECTION_MATRIX:
			case GVS_WORLD_VIEW_MATRIX:
			case GVS_VIEW_PROJECTION_MATRIX:
			case GVS_WORLD_VIEW_PROJECTION_MATRIX:
			case GVS_TRANSPOSE_WORLD_MATRIX:
			case GVS_TRANSPOSE_VIEW_MATRIX:
			case GVS_TRANSPOSE_PROJECTION_MATRIX:
			case GVS_TRANSPOSE_WORLD_VIEW_MATRIX:
			case GVS_TRANSPOSE_VIEW_PROJECTION_MATRIX:
			case GVS_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX:
			case GVS_INVERSE_WORLD_MATRIX:
			case GVS_INVERSE_VIEW_MATRIX:
			case GVS_INVERSE_PROJECTION_MATRIX:
			case GVS_INVERSE_WORLD_VIEW_MATRIX:
			case GVS_INVERSE_VIEW_PROJECTION_MATRIX:
			case GVS_INVERSE_WORLD_VIEW_PROJECTION_MATRIX:
			{
				size = sizeof(Matrix4);
				Matrix4 tempMat;
				glob->GetVal(it->first, size, tempMat.ptr());
				UpdateConstantVariable(it->second, size, tempMat.ptr(), false);
			}	break;

			case GVS_BONE_MATRICES:
			{
				size = sizeof(Matrix4);
				std::vector<Matrix4> tempMatVec(element_num, Matrix4::IDENTITY);
				glob->GetVal(it->first, size, tempMatVec[0].ptr(), element_num);
				UpdateConstantVariable(it->second, size*element_num, tempMatVec[0].ptr(), false);
			}	break;

			case GVS_MATERIAL_AMBIENT:
			case GVS_MATERIAL_DIFFUSE:
			case GVS_MATERIAL_SPECULAR:
			case GVS_MATERIAL_EMISSIVE:
			case GVS_MATERIAL_TRANSPARENT:
			case GVS_DIRECTIONAL_LIGHT_DIFFUSE:
			case GVS_DIRECTIONAL_LIGHT_SPECULAR:
			case GVS_POINT_LIGHT_DIFFUSE:
			case GVS_POINT_LIGHT_SPECULAR:
			case GVS_SPOT_LIGHT_DIFFUSE:
			case GVS_SPOT_LIGHT_SPECULAR:
			{
				size = sizeof(Vector4);
				Vector4 tempVec4;
				glob->GetVal(it->first, size, &tempVec4);
				UpdateConstantVariable(it->second, size, tempVec4.ptr(), false);
			}	break;

			case GVS_DIRECTIONAL_LIGHT_DIRECTION:
			case GVS_POINT_LIGHT_POSITION:
			case GVS_SPOT_LIGHT_POSITION:
			case GVS_SPOT_LIGHT_DIRECTION:
			case GVS_CAMERA_POSITION:
			case GVS_CAMERA_DIRECTION:
			case GVS_CAMERA_UP:
			{
				size = sizeof(Vector3);
				Vector3 tempVec3;
				glob->GetVal(it->first, size, &tempVec3);
				UpdateConstantVariable(it->second, size, &tempVec3, false);
			}	break;

			case GVS_MATERIAL_OPACITY:
			case GVS_MATERIAL_SHININESS:
			case GVS_POINT_LIGHT_CONST_ATT:
			case GVS_POINT_LIGHT_LINEAR_ATT:
			case GVS_POINT_LIGHT_QUAD_ATT:
			case GVS_SPOT_LIGHT_CONST_ATT:
			case GVS_SPOT_LIGHT_LINEAR_ATT:
			case GVS_SPOT_LIGHT_QUAD_ATT:
			case GVS_SPOT_LIGHT_EXP:
			case GVS_SPOT_LIGHT_CUTOFF:
			case GVS_POINT_LIGHT_DISTANCE:
			case GVS_SPOT_LIGHT_DISTANCE:
			case GVS_SYSTEM_ELAPSE_TIME:
			case GVS_SYSTEM_DELTA_TIME:
			case GVS_INTER_FACTOR:
			case GVS_SHADOW_BIAS:
			{
				size = sizeof(float);
				float tempFloat;
				glob->GetVal(it->first, size, &tempFloat);
				UpdateConstantVariable(it->second, size, &tempFloat, false);
			}	break;

			case GVS_FRAME_ANIMATION:
			case GVS_ACTIVATE_LIGHTNUM:
			{
				size = sizeof(unsigned int);
				unsigned int tempInt;
				glob->GetVal(it->first, size, &tempInt);
				UpdateConstantVariable(it->second, size, &tempInt, true);
			}	break;

			case GVS_TEXTURE_DIFFUSE:
			case GVS_TEXTURE_AMBIENT:
			case GVS_TEXTURE_SPECULAR:
			case GVS_SHADOW_MAP:
			case GVS_POST_TEXTURE:
			{
				size = sizeof(Texture*);
				Texture* texture = NULL;
				glob->GetVal(it->first, size, &texture);
				UpdateTexture(it->second, texture);
			}	break;
			}
		}
	}

	/************************** DX11GpuProgramFactory ****************************/
	DX11GpuProgram* DX11GpuProgramFactory::CreateGpuProgram(const String& name){
		return new DX11GpuProgram(name);
	}

	DX11GpuProgram* DX11GpuProgramFactory::CreateGpuProgram(){
		return new DX11GpuProgram();
	}
}
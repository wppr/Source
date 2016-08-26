#ifndef _DX11_INPUT_LAYOUT_H_
#define _DX11_INPUT_LAYOUT_H_

#include "InputLayout.h"
#include "DX11Shader.h"
#include "DX11DatatypeHelper.h"

namespace HW{

	struct DX11InputDesc{
		String Name;						// the Variable name
		String Semantic;					// the semantic
		UINT uSemanticIndex;		// the index within semantic
		DX11_DATA_TYPE eDataType;		// the data type
	};

	class DX11InputLayout : public InputLayout{
		friend class DX11Shader;
	public:
		DX11InputLayout();
		virtual ~DX11InputLayout();
		virtual bool CreateInternalRes(RenderSystem* renderSystem, Shader* shader);
		std::vector<DX11InputDesc>& GetElement();

		ID3D11InputLayout* GetInputLayout() const{
			return m_pInputLayout;
		}
		// Return the size of the element with index i
		unsigned int GetElementSize(int i) const;
		unsigned int GetTotalSize() const{
			return m_uTotalSize;
		}
	private:
		std::vector<DX11InputDesc> m_Element;
		ID3D11InputLayout* m_pInputLayout;
		unsigned int m_uTotalSize;
	};
}

#endif
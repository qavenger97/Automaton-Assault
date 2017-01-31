#include "pch.h"
#include "Shader.h"
namespace Hourglass
{

	HRESULT Shader::CompileShader(LPCWSTR srcFile, ID3D11Device * device, ID3DBlob ** blob, LPCSTR profile, const D3D_SHADER_MACRO* marco/* = nullptr */, LPCSTR entryPoint /*= "main" */)
	{
	
		if (!srcFile || !device || !blob || !profile || !entryPoint)
		{
			return E_INVALIDARG;
		}

		*blob = nullptr;
		UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

	#if defined( DEBUG ) || defined( _DEBUG )
		flags |= D3DCOMPILE_DEBUG;
		flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	#endif

		ID3DBlob* shaderBlob = nullptr;
		ID3DBlob* errorBlob = nullptr;

		HRESULT hr = D3DCompileFromFile(srcFile, marco, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, profile, flags, 0, &shaderBlob, &errorBlob);

		if (FAILED(hr))
		{
			if (errorBlob)
			{
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
			}
			if (shaderBlob)
				shaderBlob->Release();

			return hr;
		}

		*blob = shaderBlob;

		return hr;
	}

	HRESULT Shader::CreateStructuredBuffer(ID3D11Device* device, UINT elementSize, UINT elementCount, ID3D11Buffer ** ppOut, void * pInitData)
	{
		if (ppOut == nullptr || device == nullptr)
		{
			return E_INVALIDARG;
		}

		*ppOut = nullptr;

		D3D11_BUFFER_DESC desc = {};

		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = elementCount * elementSize;
		desc.StructureByteStride = elementSize;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		if (pInitData)
		{
			D3D11_SUBRESOURCE_DATA initData;
			initData.pSysMem = pInitData;
			return device->CreateBuffer(&desc, &initData, ppOut);
		}
		else
		{
			return device->CreateBuffer(&desc, nullptr, ppOut);
		}
	}

}
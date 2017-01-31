#pragma once
namespace Hourglass
{
	class Shader
	{
	public:
		static HRESULT CompileShader(_In_ LPCWSTR srcFile,
			_In_ ID3D11Device* device, _Outptr_ ID3DBlob** blob, _In_ LPCSTR profile, _In_ const D3D_SHADER_MACRO* marco = nullptr , _In_ LPCSTR entryPoint = "main");
		static HRESULT CreateStructuredBuffer(ID3D11Device* device, UINT elementSize, UINT elementCount, ID3D11Buffer** ppOut, void* initData = nullptr);
	};
}

#include "pch.h"

#include "RenderDevice.h"

namespace Hourglass
{
	RenderDevice					g_RenderDev;
	ComPtr<ID3D11InputLayout>		g_InputLayouts[kVertexDeclCount];
	ComPtr <ID3D11BlendState>		g_BlendState[kBlendStateCount];
	ComPtr<ID3D11SamplerState>		g_SamplerState[kSamplerStateCount];
	RenderShader					g_RenderShaders[kRenderShaderCount];
	ComPtr<ID3D11ComputeShader>		g_ComputeShaders[kComputeShaderCount];
}
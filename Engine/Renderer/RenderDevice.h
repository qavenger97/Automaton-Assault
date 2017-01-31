/*!
 * \file RenderDevice.h
 *
 * \author Shiyang
 * \date October 2016
 *
 * Shared render device for render system
 * This file should only be included by render system source code.
 */
#pragma once

#include "VertexDeclaration.h"
#include "CommonDef.h"

namespace Hourglass
{
	struct RenderDevice
	{
		ID3D11Device* device;
		ID3D11DeviceContext* deviceContext;
	};

	// global access to render device and device context
	extern RenderDevice g_RenderDev;

	// global access to pre-defined vertex input layouts
	extern ComPtr<ID3D11InputLayout> g_InputLayouts[kVertexDeclCount];

	// global access to blend state
	extern ComPtr <ID3D11BlendState> g_BlendState[kBlendStateCount];

	// global access to sampler states
	extern ComPtr<ID3D11SamplerState> g_SamplerState[kSamplerStateCount];

	//////////////////////////////////////////////////////////////////////////
	// Render shaders
	//////////////////////////////////////////////////////////////////////////
	enum RenderShaderType
	{
		kRenderShader_Color,
		kRenderShader_Font,
		kRenderShader_Depth,
		kRenderShader_Lighting,
		kRenderShader_LightingSkinned,
		kRenderShader_LightingSkinned_LightWeight,
		kRenderShader_PbrFlow,
		kRenderShader_PbrLighting,
		kRenderShader_Barrier,
		kRenderShader_DebugSimpleColor,
		kRenderShader_PostProcessor,

		kRenderShaderCount
	};

	struct RenderShader
	{
		ComPtr<ID3D11VertexShader>	VertexShader;
		ComPtr<ID3D11PixelShader>	PixelShader;
	};

	extern RenderShader g_RenderShaders[kRenderShaderCount];

	//////////////////////////////////////////////////////////////////////////
	// Compute shaders
	//////////////////////////////////////////////////////////////////////////
	enum ComputeShaderType
	{
		kComputeShader_Blur,

		kComputeShaderCount
	};

	extern ComPtr<ID3D11ComputeShader> g_ComputeShaders[kComputeShaderCount];
}
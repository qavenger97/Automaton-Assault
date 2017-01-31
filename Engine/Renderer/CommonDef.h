/*!
 * \file CommonDef.h
 *
 * \author Shiyang Ao
 * \date October 2016
 *
 * Common definitions for render system
 */
#pragma once

namespace Hourglass
{
	enum DepthStateType
	{
		kDepthState_WriteAndTest,
		kDepthState_NoWrite,

		kDepthStateCount
	};

	enum SamplerStateType
	{
		kSamplerState_Point,			// Sampler state for low resolution/pixelated texture 
		kSamplerState_Bilinear,
		kSamplerState_Trilinear,		// Sampler state for generic texture
		kSamplerState_Anisotropic,
		kSamplerState_Bilinear_Clamp,

		kSamplerStateCount
	};

	enum BlendStateType
	{
		kBlend_Opaque,
		kBlend_AlphaBlend,
		kBlend_AlphaToCoverage,
		kBlend_Additive,

		kBlendStateCount
	};
}
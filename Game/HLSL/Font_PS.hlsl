//=============================================================================
// Font_PS.hlsl by Shiyang Ao, 2016 All Rights Reserved.
//
// 
//=============================================================================

#include "ConstBufferPS.h"
#include "ShaderCommon.hlsli"

Texture2D FontTexture	: register(t0);
SamplerState Sampler	: register(s0);

struct OUTPUT_VERTEX
{
	float4 PosH		: SV_POSITION;
	float4 ColorFg	: COLOR0;
	float4 ColorBg	: COLOR1;
	float2 UV		: TEXCOORD0;
};

float4 main(OUTPUT_VERTEX Input) : SV_TARGET
{
	float tex = FontTexture.Sample(Sampler, Input.UV).r;

	float4 Final;
	Final.rgb = Input.ColorFg.rgb * tex.rrr * tex.r + Input.ColorBg.rgb * (1.0 - tex.r);
	Final.a = Input.ColorFg.a * tex.r + Input.ColorBg.a * (1.0 - tex.r);

	return Final;
}

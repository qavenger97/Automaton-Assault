//=============================================================================
// PostProcessor_PS.hlsl by Shiyang Ao, 2016 All Rights Reserved.
//
// 
//=============================================================================

Texture2D ScreenTexture : register(t0);

SamplerState Sampler;

struct OUTPUT_VERTEX
{
	float4 PosH		: SV_POSITION;
	float4 Color	: COLOR0;
	float2 UV		: TEXCOORD0;
};

float4 main(OUTPUT_VERTEX Input) : SV_TARGET
{
	float4 Final;
	Final = Input.Color * ScreenTexture.Sample(Sampler, Input.UV);
	return Final;
}
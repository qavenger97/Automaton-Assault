
#include "ConstBufferVS.h"

Texture2D EmissiveBuffer : register(t0);
RWTexture2D<float4> ScreenOutput : register(u0);

SamplerState Sampler : register(s0);

static const float GaussianKernel[7][7] =
{
	{ 0.000036,	0.000363,	0.001446,	0.002291,	0.001446,	0.000363,	0.000036 },
	{ 0.000363,	0.003676,	0.014662,	0.023226,	0.014662,	0.003676,	0.000363 },
	{ 0.001446,	0.014662,	0.058488,	0.092651,	0.058488,	0.014662,	0.001446 },
	{ 0.002291,	0.023226,	0.092651,	0.146768,	0.092651,	0.023226,	0.002291 },
	{ 0.001446,	0.014662,	0.058488,	0.092651,	0.058488,	0.014662,	0.001446 },
	{ 0.000363,	0.003676,	0.014662,	0.023226,	0.014662,	0.003676,	0.000363 },
	{ 0.000036,	0.000363,	0.001446,	0.002291,	0.001446,	0.000363,	0.000036 },
};

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint3 Gid : SV_GroupID)
{
	float2 uv = float2(DTid.xy) * screenSize.zw;

	float texel_radius = 4.0f;
	float scaled_texel_radius = texel_radius * screenSize.y / 768.0f;

	float4 Final = (float4)0;

	for (int k_x = 0; k_x < 7; k_x++)
	{
		for (int k_y = 0; k_y < 7; k_y++)
		{
			float2 offset = float2(k_x - 3, k_y - 3) * screenSize.zw * scaled_texel_radius;
			Final += EmissiveBuffer.SampleLevel(Sampler, uv + offset, 2) * GaussianKernel[k_x][k_y];
		}
	}

	ScreenOutput[DTid.xy] = Final;
}

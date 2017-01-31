#include "SharedDefines.h"
#include "ShaderCommon.hlsli"
#include "ParticleShared.hlsl"

[maxvertexcount(4)]
void main(point VS_OUTPUT input[1], inout TriangleStream<PS_INPUT> output)
{
	float4 color = input[0].color;
	const float3 offsets[4] =
	{
		float3(-1,  1, 0),
		float3( 1,  1, 0),
		float3(-1, -1, 0),
		float3( 1, -1, 0)
	};

	float4 posW = float4(input[0].position.xyz, 1);

	[unroll]
	for (int i = 0; i < 4; i++)
	{
		float3 offset = offsets[i];
		PS_INPUT p = (PS_INPUT)0;

		float4 posV = float4(MulQuaternionVector(g_viewOrientation, offset * input[0].size),0);
		p.position = posW + posV;
		p.position = mul(p.position, g_viewProjection);
		p.normal = MulQuaternionVector(g_viewOrientation, float3(0, 0, -1));

		p.uv = (offset + 1) * 0.5;
		p.color = color;
		output.Append(p);
	}
	output.RestartStrip();
}
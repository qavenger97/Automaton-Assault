#include "ShaderCommon.hlsli"
#include "ParticleShared.hlsl"

static float3 lightDir = float3(-0.382925, -0.568176, 0.728385);

struct OUTPUT_PIXEL
{
	float4 Color		: SV_Target0;
	float4 Emissive		: SV_Target1;
};

OUTPUT_PIXEL main(PS_INPUT input) :	SV_TARGET
{
	OUTPUT_PIXEL output = (OUTPUT_PIXEL)0;
	//float lightValue = saturate(dot(input.normal, -lightDir));
	//output.Color = float4(input.color.xyz * lightValue, input.color.w);
	output.Color = input.color;
	output.Emissive = saturate(output.Color - float4(1,1,1,0));
	return output;
}
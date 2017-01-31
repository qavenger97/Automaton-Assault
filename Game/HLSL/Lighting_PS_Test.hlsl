
#include "ConstBufferPS.h"
#include "ShaderCommon.h"

Texture2D DiffuseMap : register(t0);
Texture2D NormalMap  : register(t1);
SamplerState Sampler : register(s0);

struct OUTPUT_VERTEX
{
	float4 Color	 : COLOR;
	float4 PosH		 : SV_POSITION;
	float2 UV		 : TEXCOORD0;
	float3 PosW		 : TEXCOORD1;		// World space position
	float3 NormalW	 : TEXCOORD2;		// World space normal
	float3 TangentW	 : TEXCOORD3;
	float3 BinormalW : TEXCOORD4;
};

float4 main(OUTPUT_VERTEX Input) : SV_TARGET
{
	float4 Diffuse = (float4)0;
	float4 Specular = (float4)0;
	float3 Normal = (float3)0;
	float3 viewDir = normalize(cameraPos.xyz - Input.PosW);

	if (useNormalMap)
	{
		float3x3 TBN = float3x3(normalize(Input.TangentW), normalize(Input.BinormalW), normalize(Input.NormalW));
		Normal = NormalMap.Sample(Sampler, Input.UV).xyz;
		Normal.z = sqrt(1.0f - dot(Normal.xy, Normal.xy));
		Normal = mul(Normal, TBN);
	}
	else
	{
		Normal = normalize(Input.NormalW);
	}

	float4 specularIntensity = (float4) 0;
	//[unroll]
	for (int i = 0; i < lightCount; i++)
	{
		// Note: All vectors are in world space
		float3 lightVec = lights[i].PositionRadius.xyz - Input.PosW;
		float distance = length(lightVec);
		float3 lightDir = lightVec / distance;

		float attenuation = 1.0f - saturate(distance / lights[i].PositionRadius.w);

		float diffuseIntensity = saturate(dot(lightDir, Normal));

		//float3 lightReflect = reflect(-lightDir, Normal);
		//float specularIntensity = max(pow(saturate(dot(viewDir, lightReflect)), 2.0f), 0.0f);

		//float3 halfVec = ComputeHalfVector(lightDir, viewDir);
		//float specularIntensity = max(pow(saturate(dot(Normal, halfVec)), 128.0f), 0.0f);

		float4 lightColor = lights[i].Color * lights[i].Color.a * attenuation*attenuation*attenuation;
		ComputeSpecular(lights[i].Color, Input.PosW, viewDir, Normal, lightDir, specularIntensity);
		
		Diffuse.rgb +=  diffuseIntensity * lightColor;
		Specular.rgb +=  specularIntensity * lightColor;
		//specular *= attenuation;
	}

	Diffuse *= 0.3183f;
	Diffuse.a = 1.0f;
	Specular.a = 0.0f;
	float4 finalDiffuse = DiffuseMap.Sample(Sampler, Input.UV) * Diffuse;
	float4 finalSpecular = /*DiffuseMap.Sample(Sampler, Input.UV) +*/ Specular;

	float4 FinalColor = Input.Color * lerp(finalDiffuse, finalSpecular, 0.1) + Specular * 0.04;

	//float fresnel = pow(1 - saturate(dot(viewDir, Normal)), 4.0f) * 0.2f;
	//FinalColor.rgb += fresnel;

	return FinalColor;
}


#include "ConstBufferPS.h"

const static float DIELECTRIC_CONST = 0.04;
const static float PI = 3.1415926535897932;

Texture2D BaseColorMap: register(t0);
Texture2D NormalMap   : register(t1);
Texture2D MetallicMap : register(t2);
Texture2D RoughnessMap: register(t3);
Texture2D 
SamplerState Sampler  : register(s0);

struct OUTPUT_VERTEX
{
	float4 BaseColor : COLOR;
	float  RoughnessF: ROUGHNESS;
	float  MetallicF : METALLIC;
	float2 UV		 : TEXCOORD0;
	float4 PosH		 : SV_POSITION;
	float3 PosW		 : TEXCOORD1;		// World space position
	float3 NormalW	 : TEXCOORD2;		// World space normal
	float3 TangentW	 : TEXCOORD3;
	float3 BinormalW : TEXCOORD4;
};

float4 main(OUTPUT_VERTEX Input) : SV_TARGET
{
	float4 Metallic = float4(MetallicF,MetallicF,MetallicF,1.0);
	float4 Roughness = float4(RoughnessF,RoughnessF,RoughnessF,1.0);

	// Normal Computation
	float3x3 TBN = float3x3(normalize( Input.TangentW ), normalize( Input.BinormalW ), normalize( Input.NormalW ));
	float3 Normal = NormalMap.Sample( Sampler, Input.UV ).xyz;
	Normal.z = sqrt( 1.0 - dot( Normal.xy, Normal.xy ) );
	Normal = mul( Normal, TBN );

	//float3 N = Input.NormalW;
	float3 viewDir = normalize( cameraPos.xyz - Input.PosW );
	//float3 L = Frame.DirectionalLightDirection;

	float3 DiffuseColor = BaseColor.xyz - BaseColor.xyz * Metallic.xyz;	
	float3 SpecularColor = (DIELECTRIC_CONST - DIELECTRIC_CONST * Metallic.xyz) + BaseColor.xyz * Metallic.xyz;

	float4 Color=(float4)0;
	[unroll]
	for (int i = 0; i < lightCount; i++)
	{
		// Note: All vectors are in world space
		float3 lightVec = lights[i].PositionRadius.xyz - Input.PosW;
		float distance = length(lightVec);
		float3 lightDir = lightVec / distance;

		float attenuation = 1.0 - saturate(distance / lights[i].PositionRadius.w);
		attenuation *= attenuation;

		if (lights[i].Type == LIGHT_TYPE_SPOTLIGHT)
		{
			const float surfaceRatio = saturate(dot(-lightDir, lights[i].Direction));
			const float coneAtt = 1.0 - saturate((lights[i].ConeRatio.x - surfaceRatio) / (lights[i].ConeRatio.x - lights[i].ConeRatio.y));

			attenuation *= coneAtt;
		}

		float3 H = normalize( viewDir + lightDir );
		float NoH = saturate( dot( Normal, H ) );
		float3 A = Roughness.xyz * Roughness.xyz;
		float3 A_sq = A * A;
		float3 D = (NoH * A_sq - NoH) * NoH + 1;
		float3 D_Corr = A_sq / (4.0 * D*D);

		float3 lightColor = lights[i].Color.rgb * lights[i].Color.a;
		
		Color.rgb += attenuation * (DiffuseColor + D_Corr * SpecularColor) * lightColor;
	}

	Color.a = BaseColor.a;

	return (Input.Color * Color) / PI;
}
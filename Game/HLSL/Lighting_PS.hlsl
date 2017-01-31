
#include "ConstBufferPS.h"
#include "ShaderCommon.hlsli"

Texture2D DiffuseMap  : register(t0);
Texture2D NormalMap   : register(t1);
Texture2D SpecularMap : register(t2);
Texture2D EmissiveMap : register(t3);
SamplerState Sampler  : register(s0);

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

struct OUTPUT_PIXEL
{
	float4 Color		: SV_Target0;
	float4 Emissive		: SV_Target1;
};

OUTPUT_PIXEL main(OUTPUT_VERTEX Input)
{
	OUTPUT_PIXEL Out = (OUTPUT_PIXEL)0;

	float4 Diffuse = (float4)0;
	float4 Specular = (float4)0;
	float3 Normal = (float3)0;
	float4 Emissive = (float4)0;
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

	[unroll]
	for (int i = 0; i < lightCount; i++)
	{
		// Note: All vectors are in world space
		float3 lightVec = lights[i].PositionRadius.xyz - Input.PosW;
		float distance = length(lightVec);
		float3 lightDir = lightVec / distance;

		float attenuation = 1.0f - saturate(distance / lights[i].PositionRadius.w);
		attenuation *= attenuation;

		if (lights[i].Type == LIGHT_TYPE_SPOTLIGHT)
		{
			float surfaceRatio = saturate(dot(-lightDir, lights[i].Direction));
			float coneAtt = 1.0f - saturate((lights[i].ConeRatio.x - surfaceRatio) / (lights[i].ConeRatio.x - lights[i].ConeRatio.y));

			attenuation *= coneAtt;
		}

		float diffuseIntensity = saturate(dot(lightDir, Normal));

		//float3 lightReflect = reflect(-lightDir, Normal);
		//float specularIntensity = max(pow(saturate(dot(viewDir, lightReflect)), 2.0f), 0.0f);

		float3 halfVec = ComputeHalfVector(lightDir, viewDir);
		float specularIntensity = max(pow(saturate(dot(Normal, halfVec)), 128.0f), 0.0f);

		float3 lightColor = lights[i].Color.rgb * lights[i].Color.a;
		
		Diffuse.rgb += attenuation * diffuseIntensity * lightColor;
		Specular.rgb += attenuation * specularIntensity * lightColor;
	}

	Diffuse *= 0.3183f;
	Diffuse.a = 1.0f;
	Specular.a = 0.0f;

	// Apply specular map
	if (useSpecularMap)
	{
		Specular.rgb *= SpecularMap.Sample(Sampler, Input.UV).rgb;
	}

	Diffuse *= DiffuseMap.Sample(Sampler, Input.UV);

	if (useEmissiveMap)
	{
		Emissive = EmissiveMap.Sample(Sampler, Input.UV);
	}
	Emissive.a = 1.0f;

	Out.Color.rgb = Input.Color.rgb * Diffuse.rgb / PI + Specular.rgb;
	Out.Color.rgb += Emissive.rgb;
	Out.Color.a = Input.Color.a * Diffuse.a;
	Out.Emissive = Emissive;

	if (Input.PosW.y < 0.0 && any( Out.Color.rgb ))
	{
		float foggy = (-Input.PosW.y - 2.0f);
		if (foggy > 0.0)
		{
			float fogLerp = saturate( foggy / 2000.0 );
			Out.Color = lerp( Out.Color, float4(0.4, 0.4, 0.8, 1.0), fogLerp );
		}
	}

	//float fresnel = pow(1 - saturate(dot(viewDir, Normal)), 4.0f) * 0.2f;
	//FinalColor.rgb += fresnel;

	return Out;
}
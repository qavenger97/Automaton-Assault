
#include "ConstBufferPS.h"
#include "ShaderCommon.hlsli"

Texture2D AlphaMap : register(t0);
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

OUTPUT_PIXEL main(OUTPUT_VERTEX Input) : SV_TARGET
{
	float vs = Input.UV.y - time / 5;
	float2 uv = float2(Input.UV.x, frac(vs));
	
	float4 kBlue = float4(0.3, 0.80849, 60.0, 1.0);

	float4 Diffuse = (float4)0;
	float3 Normal = normalize( Input.NormalW );
	float3 viewDir = normalize( cameraPos.xyz - Input.PosW );

	[unroll]
	for (int i = 0; i < lightCount; i++)
	{
		// Note: All vectors are in world space
		float3 lightVec = lights[i].PositionRadius.xyz - Input.PosW;
		float distance = length( lightVec );
		float3 lightDir = lightVec / distance;

		float attenuation = 1.0f - saturate( distance / lights[i].PositionRadius.w );
		attenuation *= attenuation;

		if (lights[i].Type == LIGHT_TYPE_SPOTLIGHT)
		{
			float surfaceRatio = saturate( dot( -lightDir, lights[i].Direction ) );
			float coneAtt = 1.0f - saturate( (lights[i].ConeRatio.x - surfaceRatio) / (lights[i].ConeRatio.x - lights[i].ConeRatio.y) );

			attenuation *= coneAtt;
		}

		float diffuseIntensity = saturate( dot( lightDir, Normal ) );

		//float3 lightReflect = reflect(-lightDir, Normal);
		//float specularIntensity = max(pow(saturate(dot(viewDir, lightReflect)), 2.0f), 0.0f);

		float3 halfVec = ComputeHalfVector( lightDir, viewDir );

		float3 lightColor = lights[i].Color.rgb * lights[i].Color.a;

		Diffuse.rgb += attenuation * diffuseIntensity * lightColor;
	}

	Diffuse *= 0.3183f;
	Diffuse.a = 1.0f;

	OUTPUT_PIXEL Out = (OUTPUT_PIXEL)0;

	float4 Alpha = AlphaMap.Sample( Sampler, uv );
	float Opacity = Alpha.a * 0.3;
	float Nega = 0.3 - 0.5 * Alpha.a;
	Out.Color = Diffuse / PI;
	Out.Color *= kBlue * Alpha;
	Out.Color = saturate( Out.Color + Opacity * 2.0 * kBlue );
	Out.Color.a = Opacity;
	//Out.Color += Nega;
	Out.Emissive = Opacity * Opacity * 0.2;
	Out.Emissive *= kBlue;
	Out.Emissive += Nega * 0.2;

	
	

	//float fresnel = pow(1 - saturate(dot(viewDir, Normal)), 4.0f) * 0.2f;
	//FinalColor.rgb += fresnel;

	return Out;
	
}
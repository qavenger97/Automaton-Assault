#include "BRDF.hlsli"
#include "FwdLighting.hlsli"

Texture2D BaseColorMap: register(t0);
Texture2D NormalMap   : register(t1);
Texture2D MetallicMap : register(t2);
Texture2D RoughnessMap: register(t3);
Texture2D EmissiveMap : register(t4);

TextureCube Environment : register(t7);

SamplerState Sampler  : register(s0);

struct OUTPUT_VERTEX
{
	float4 Color  : COLOR;
	float4 PosH   : SV_POSITION;
	float2 UV   : TEXCOORD0;
	float3 PosW   : TEXCOORD1;  // World space position
	float3 NormalW  : TEXCOORD2;  // World space normal
	float3 TangentW  : TEXCOORD3;
	float3 BinormalW : TEXCOORD4;
};

struct OUTPUT_PIXEL
{
	float4 Color  : SV_Target0;
	float4 Emissive  : SV_Target1;
};

OUTPUT_PIXEL main( OUTPUT_VERTEX Input )
{
	float4 baseColor = saturate( BaseColorMap.Sample( Sampler, Input.UV ) );
	float4 emissive = saturate( EmissiveMap.Sample( Sampler, Input.UV ) );
	float4 Color = (float4)0;
	float3 envLight = (float3)0;
	float strength = 1.5;
	if (any( baseColor.xyz ))
	{
		baseColor.xyz = pow(baseColor.xyz, 2.2);
		emissive.xyz = pow(emissive.xyz, 2.2);

		float4 metallicMap = ( MetallicMap.Sample( Sampler, Input.UV ) );
		float metallic = metallicMap.r;
		float4 roughnessMap = ( RoughnessMap.Sample( Sampler, Input.UV ) );
		float roughness = 0.5 * roughnessMap.r * (roughnessMap.r + 1);	
		float3x3 TBN = float3x3(normalize( Input.TangentW ), normalize( Input.BinormalW ), normalize( Input.NormalW ));
		float3 normal = NormalMap.Sample( Sampler, Input.UV ).xyz;
		normal.z = sqrt( 1.0 - dot( normal.xy, normal.xy ) );
		normal = normalize( mul( normal, TBN ) );

		float3 viewDir = normalize( cameraPos.xyz - Input.PosW );
		float3 envDiff = Environment.SampleLevel(Sampler, normal, 4+ roughnessMap.r * 6).xyz;
		envLight = Environment.SampleLevel(Sampler, reflect(-viewDir, normal), 4+ roughnessMap.r * 6).xyz;
		
		float3 base = baseColor.xyz + envDiff*0.04;
		float3 diffuse_PI = base - base * metallic;
		float3 specular = (kDielectricConst - kDielectricConst * metallic) + baseColor.xyz * metallic;

		Color = max(StandardFwd( Input.PosW, diffuse_PI, specular, roughness, normal, viewDir ) + float4(envLight,0) * 0.04 * (1- roughnessMap.r), 0);	

		if (Input.PosW.y < -2.0)
		{
			float distBeneathFloor = (-Input.PosW.y - 2.0);
			if (distBeneathFloor > 0.0)
			{
				float fogLerp = saturate( distBeneathFloor / 2000.0 );
				Color = lerp( Color, float4(0.6, 0.6, 0.8, 1.0), fogLerp );
			}

			strength = 1.0 + min(0.75, 1.0 / (distBeneathFloor + 0.1));
		}		
	}

	OUTPUT_PIXEL Out = (OUTPUT_PIXEL)0;	

	Out.Emissive = (Color + emissive * Input.Color) * strength - float4(1.0, 1.0, 1.0, 0.0);
	Out.Color = saturate(Color + emissive / strength);

	/*Out.Emissive = float4(0.0, 0.0, 0.0, 0.0);
	Out.Color =float4( envLight, 1);
*/
	return Out;
}
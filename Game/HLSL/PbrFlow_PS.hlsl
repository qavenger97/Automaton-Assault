#include "BRDF.hlsli"
#include "FwdLighting.hlsli"

Texture2D BaseColorMap: register(t0);
Texture2D NormalMap   : register(t1);
Texture2D MetallicMap : register(t2);
Texture2D RoughnessMap: register(t3);
Texture2D EmissiveMap : register(t4);
Texture2D FlowMap	  : register(t5);

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

static float2 FLOW_DIR0 = float2(0, 0.48);
static float2 FLOW_DIR1 = float2(0.63, 0);

OUTPUT_PIXEL main( OUTPUT_VERTEX Input )
{	
	//Flow speed
	float speedModifier = time * 0.04;
	float4 baseColor = saturate( BaseColorMap.Sample( Sampler, Input.UV ) );
	float2 newUV0 = Input.UV.xy + speedModifier * FLOW_DIR0;
	//Flow scale
	newUV0 *= 1.7;
	newUV0 = frac(newUV0);

	float2 newUV1 = Input.UV.xy + speedModifier * FLOW_DIR1;
	newUV1 *= 1.3;
	newUV1 = frac(newUV1);

	float flow0 = FlowMap.Sample(Sampler, newUV0).x;
	float flow1 = FlowMap.Sample(Sampler, newUV1).x;

	//flow intensity
	float flow = (flow0 * flow1);
	flow *= flow * 2;

	float4 emissive = saturate(EmissiveMap.Sample(Sampler, Input.UV));
	emissive.xyz *= flow;

	float4 Color = (float4)0;
	float3 envLight = (float3)0;
	if (any( baseColor.xyz ))
	{
		baseColor.xyz = pow(baseColor.xyz, 2.2);
		emissive.xyz = pow(emissive.xyz, 2.2);

		float4 metallicMap = ( MetallicMap.Sample( Sampler, Input.UV ) );
		float metallic = metallicMap.r;
		float4 roughnessMap = ( RoughnessMap.Sample( Sampler, Input.UV ) );
		//float roughness = roughnessMap.r;
		float roughness = 0.5 * roughnessMap.r * (roughnessMap.r + 1);	
		//float roughness = roughnessMap.r*roughnessMap.r;
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

		Color = StandardFwd( Input.PosW, diffuse_PI, specular, roughness, normal, viewDir ) + float4(envLight,0) * 0.04 * (1- roughnessMap.r);
		

		if (Input.PosW.y < 2.0 && any( Color.rgb ))
		{
			float foggy = (-Input.PosW.y - 2.0f);
			if (foggy > 0.0)
			{
				float fogLerp = saturate( foggy / 2000.0 );
				Color = lerp( Color, float4(0.6, 0.6, 0.8, 1.0), fogLerp );
			}
		}		
	}

	OUTPUT_PIXEL Out = (OUTPUT_PIXEL)0;

	const float strength = 1.5;

	Out.Emissive = (Color + emissive * Input.Color) * strength - float4(1.0, 1.0, 1.0, 0.0);
	Out.Color = saturate(Color + emissive / strength);
	//Out.Emissive = float4(0.0, 0.0, 0.0, 0.0);
	//Out.Color =float4( (float3)flow, 1);

	return Out;
}
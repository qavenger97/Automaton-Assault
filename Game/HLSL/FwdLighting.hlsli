#ifndef _FORWARDLIGHTING_H
#define _FORWARDLIGHTING_H

#include "ConstBufferPS.h"
#include "ShaderCommon.hlsli"

const static float kIntensityModifier = 12;
const static float kRadiusModifier = 1;

//float4 FwdPlus(float3 worldPos, float3 diffuse_PI, float3 specular, float roughness, float3 N, float3 V) { }

void ApplyConeAtt( LightInfo light, float3 L, inout float attenuation )
{
	if (light.Type == LIGHT_TYPE_SPOTLIGHT)
	{
		float surfaceRatio = saturate( dot( -L, light.Direction ) );
		float coneAtt = 1.0 - saturate( (light.ConeRatio.x - surfaceRatio) / (light.ConeRatio.x - light.ConeRatio.y) );

		attenuation *= coneAtt;
	}
}

float4 StandardFwd(float3 worldPos, float3 diffuse_PI, float3 specular, float roughness, float3 N, float3 V)
{
	float N_V = saturate( dot( N, V ) );
	N_V = max( N_V, kEpsilon);

	float4 ColorPI = (float4)0;
	for (int i = 0; i < lightCount; i++)
	{
		float intensity = lights[i].Color.a * kIntensityModifier;
		float3 lightColor = lights[i].Color.rgb * intensity;

		float3 lightVec = lights[i].PositionRadius.xyz - worldPos;
		float lightDistSq = dot( lightVec, lightVec );

		float3 L = lightVec / sqrt( lightDistSq );
		float N_L = saturate( dot( N, L ) );

		float lrf_term0 = 1.0 / (lights[i].PositionRadius.w * kRadiusModifier);
		float lrf_term1 = lightDistSq * lrf_term0 * lrf_term0;
		float lrf_term2 = saturate( 1.0 - lrf_term1 * lrf_term1 );
		float lightRadiusFilter = lrf_term2 * lrf_term2;

		if (lightRadiusFilter > 0.0)
		{
			float attenuation = ((1.0 / (lightDistSq + 1.0)) * lightRadiusFilter);

			ApplyConeAtt( lights[i], L, attenuation );

			float3 contribution = lightColor * N_L * attenuation;
			float3 brdfPI = diffuse_PI + MicrofacetBRDF_PI( specular, roughness, L, V, N, N_V, N_L );
			ColorPI.rgb += contribution * brdfPI;
		}
	}

	return ColorPI / kPI;  // 1/PI returned to BRDF and diffuse lambert calculations
}

#endif
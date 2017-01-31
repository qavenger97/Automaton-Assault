#include "SharedDefines.h"
#include "ShaderCommon.hlsli"
#include "ParticleShared.hlsl"

//struct shapes
//{
//	float4 offsets[8];
//	float3 normal[8];
//};

[maxvertexcount(24)]
void main(point VS_OUTPUT input[1], inout TriangleStream<PS_INPUT> output)
{
	float size = abs(input[0].position.w);
	float4 color = input[0].color;
	const float3 offsets[8] =
	{
		float3(-1,  1, -1),
		float3( 1,  1, -1),
		float3(-1, -1, -1),
		float3( 1, -1, -1),
		float3(-1,  1,  1),
		float3( 1,  1,  1),
		float3(-1, -1,  1),
		float3( 1, -1,  1)
	};

	float4 vertices[8] = { (float4)0 , (float4)0 , (float4)0 , (float4)0, (float4)0 , (float4)0 , (float4)0 , (float4)0 };

	float4 posW = float4(input[0].position.xyz, 1);
	float3 forward = normalize(input[0].velocity);
	float3 right = normalize(cross(float3(0, 1, 0), forward));
	float3 up = normalize(cross(forward, right));
	float3x3 R = float3x3(right, up, forward);
	[unroll]
	for (int i = 0; i < 8; i++)
	{
		float3 posL = mul(offsets[i].xyz, R) * size;
		vertices[i] = mul(posW +  float4(posL, 0), g_viewProjection);
	}
	PS_INPUT p = (PS_INPUT)0;

	//front face
	p.position = vertices[0];
	p.uv = float2(0, 1);
	p.color = color;
	p.normal = float3(0, 0, -1);
	output.Append(p);

	p.position = vertices[1];
	p.uv = float2(1, 1);
	p.color = color;
	output.Append(p);

	p.position = vertices[2];
	p.uv = float2(0, 0);
	p.color = color;
	output.Append(p);

	p.position = vertices[3];
	p.uv = float2(1, 0);
	p.color = color;
	output.Append(p);
	output.RestartStrip();

	//left face
	p.position = vertices[1];
	p.uv = float2(0, 1);
	p.color = color;
	p.normal = float3(1, 0, 0);
	output.Append(p);

	p.position = vertices[5];
	p.uv = float2(1, 1);
	p.color = color;
	output.Append(p);

	p.position = vertices[3];
	p.uv = float2(0, 0);
	p.color = color;
	output.Append(p);

	p.position = vertices[7];
	p.uv = float2(1, 0);
	p.color = color;
	output.Append(p);
	output.RestartStrip();

	//back face
	p.position = vertices[5];
	p.uv = float2(0, 1);
	p.color = color;
	p.normal = float3(0, 0, 1);
	output.Append(p);

	p.position = vertices[4];
	p.uv = float2(1, 1);
	p.color = color;
	output.Append(p);

	p.position = vertices[7];
	p.uv = float2(0, 0);
	p.color = color;
	output.Append(p);

	p.position = vertices[6];
	p.uv = float2(1, 0);
	p.color = color;
	output.Append(p);
	output.RestartStrip();

	//right face
	p.position = vertices[4];
	p.uv = float2(0, 1);
	p.color = color;
	p.normal = float3(-1, 0, 0);
	output.Append(p);

	p.position = vertices[0];
	p.uv = float2(1, 1);
	p.color = color;
	output.Append(p);

	p.position = vertices[6];
	p.uv = float2(0, 0);
	p.color = color;
	output.Append(p);

	p.position = vertices[2];
	p.uv = float2(1, 0);
	p.color = color;
	output.Append(p);
	output.RestartStrip();
	
	//top face
	p.position = vertices[4];
	p.uv = float2(0, 1);
	p.color = color;
	p.normal = float3(0, 1, 0);
	output.Append(p);

	p.position = vertices[5];
	p.uv = float2(1, 1);
	p.color = color;
	output.Append(p);

	p.position = vertices[0];
	p.uv = float2(0, 0);
	p.color = color;
	output.Append(p);

	p.position = vertices[1];
	p.uv = float2(1, 0);
	p.color = color;
	output.Append(p);
	output.RestartStrip();

	//bottom face
	p.position = vertices[2];
	p.uv = float2(0, 1);
	p.color = color;
	p.normal = float3(0, -1, 0);
	output.Append(p);

	p.position = vertices[3];
	p.uv = float2(1, 1);
	p.color = color;
	output.Append(p);

	p.position = vertices[6];
	p.uv = float2(0, 0);
	p.color = color;
	output.Append(p);

	p.position = vertices[7];
	p.uv = float2(1, 0);
	p.color = color;
	output.Append(p);
	output.RestartStrip();
}
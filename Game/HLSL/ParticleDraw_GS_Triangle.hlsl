#include "SharedDefines.h"
#include "ShaderCommon.hlsli"
#include "ParticleShared.hlsl"

StructuredBuffer<GS_PRIMITIVE_DATA> g_primitiveDatas : register(t0);

[maxvertexcount(24)]
void main(point VS_OUTPUT input[1], inout TriangleStream<PS_INPUT> output)
{
	uint shape_offset = input[0].shapeInfo;
	uint billBoard = shape_offset >> 31;
	uint velocityFollow = (shape_offset >> 15) & 0x01;
	uint length = (shape_offset & ~0x80000000) >> 16 ;
	shape_offset &= 0x00007fff;

	float3 size = input[0].size;
	float4 color = input[0].color;

	float3 forward = normalize(input[0].velocity) * velocityFollow;
	forward.z += !any(forward);
	float3 right = normalize(cross(float3(0, 1, 0), forward));
	float3 up = normalize(cross(forward, right));

	float3x3 R = float3x3(right, up, forward);

	float4 posW = float4(input[0].position.xyz, 1);
	float4 rotation = g_viewOrientation * billBoard;
	rotation.w += !any(rotation);

	for (uint i = 0; i < length; i++)
	{
		GS_PRIMITIVE_DATA primitiveData = g_primitiveDatas[shape_offset + i];

		PS_INPUT p = (PS_INPUT)0;
		float3 posV = primitiveData.offset* size;
		/*posV = mul(posV, R);
		posV = MulQuaternionVector(rotation, posV );*/
		
		p.position = posW + float4(posV, 0);
		p.position = mul(p.position, g_viewProjection);

		p.normal = primitiveData.normal;
		/*p.normal = mul(p.normal, R);
		p.normal = MulQuaternionVector(rotation, p.normal);*/

		p.uv = float2(primitiveData.u, primitiveData.v);
		p.color = color;
		output.Append(p);

		if ( (i+1) % 4 == 0)output.RestartStrip();
	}
	output.RestartStrip();
}
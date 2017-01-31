#include "ShaderCommon.hlsli"
#include "ParticleShared.hlsl"

StructuredBuffer<Particle>	g_particlePool	: register(t0);
StructuredBuffer<float2>	g_aliveList		: register(t1);

VS_OUTPUT main(uint VertexID : SV_VertexID)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	uint index = g_aliveList[g_aliveCount - VertexID - 1].x;
	Particle p = g_particlePool[index];
	output.position.xyz = p.m_position;
	output.position.w = p.m_size;
	output.color = p.m_color;
	output.size = p.m_size;
	output.velocity = p.m_velocity;
	output.shapeInfo = p.m_shape;
	return output;
}
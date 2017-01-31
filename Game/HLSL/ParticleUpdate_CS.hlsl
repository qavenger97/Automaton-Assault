#include "SharedDefines.h"
#include "ShaderCommon.hlsli"
#include "ParticleShared.hlsl"

RWStructuredBuffer<Particle>			g_particles		: register(u0);
RWStructuredBuffer<ParticleAtt>			g_particleAtt	: register(u1);
AppendStructuredBuffer<uint>			g_deadList		: register(u2);
RWBuffer<uint>							g_drawArgs		: register(u3);
RWStructuredBuffer<float2>				g_sortList		: register(u4);

[numthreads(256,1,1)]
void main(uint3 id : SV_DispatchThreadID)
{
	if (id.x == 0)
	{
		g_drawArgs[0] = 0;
		g_drawArgs[1] = 1;
		g_drawArgs[2] = 0;
		g_drawArgs[3] = 0;
		g_drawArgs[4] = 0;
	}

	GroupMemoryBarrierWithGroupSync();

	Particle p = g_particles[id.x];
	ParticleAtt att = g_particleAtt[id.x];
	if (p.m_age > 0)
	{
		float ratio = 1 - p.m_age / p.m_lifeSpan;
		
		p.m_age -= g_dt;

		float speed = lerp(att.m_startSpeed, att.m_endSpeed, ratio);
		//p.m_velocity = g_position-p.m_position * g_dt;
		p.m_velocity += att.m_acceleration * g_dt;
		p.m_position += (p.m_velocity * g_dt) * speed;
		p.m_size = lerp(att.m_startSize, att.m_endSize, ratio);
		p.m_color = lerp(att.m_startColor, att.m_endColor, ratio);

		if (p.m_age <= 0)
		{
			p.m_age = -1;
			g_deadList.Append(id.x);
		}
		else
		{
			float3 pToView = p.m_position - g_viewPos;
			float distSq = dot(pToView, pToView);
			uint index = g_sortList.IncrementCounter();
			g_sortList[index] = float2(id.x, distSq);
			
			uint dstIdx = 0;
			InterlockedAdd(g_drawArgs[0], 1, dstIdx);
		}
	}
	g_particles[id.x] = p;
}
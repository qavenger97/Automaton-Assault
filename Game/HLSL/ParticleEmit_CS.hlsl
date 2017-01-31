#include "SharedDefines.h"
#include "ShaderCommon.hlsli"
#include "ParticleShared.hlsl"

RWStructuredBuffer<Particle>			particles		: register(u0);
RWStructuredBuffer<ParticleAtt>			particleAtts	: register(u1);
ConsumeStructuredBuffer<uint>			deadList		: register(u2);
groupshared uint						g_counter;

#define INTERLOCK
//#ifdef INTERLOCK
//	#undef INTERLOCK
//#endif
[numthreads(256,1,1)]
void main(uint3 dispatchID : SV_DispatchThreadID)
{
	g_counter = g_elapsedTimeLowPerEmitter;
	GroupMemoryBarrierWithGroupSync();

	if (dispatchID.x < g_spawnPerFrame && dispatchID.x < g_deadCount)
	{
		uint incre = dispatchID.x + 1;
		const float3 gravity = float3(0, -9.81, 0);
		Particle particle = (Particle)0;
		ParticleAtt particleAtt = (ParticleAtt)0;
		uint highPart = g_elapsedTimeHigh;

		float3 extent = RandomOnUnitSphere(dispatchID.xy, g_counter) * float3(g_extent.x, g_extent.y, g_extent.x);
#ifdef INTERLOCK
		InterlockedAdd(g_counter, incre);
#else
		g_counter += incre;
#endif 

		float radius = Random01(dispatchID.xy, g_counter) * abs(g_extent.x);

#ifdef INTERLOCK
		InterlockedAdd(g_counter, incre);
#else
		g_counter += incre;
#endif

		//extent = MulQuaternionVector(g_orientation, extent);
		particle.m_position = mad(extent, radius, g_position); //g_position + extent * radius;

		float speed = Random(g_minSpeed, g_maxSpeed, dispatchID.xy, g_counter);
#ifdef INTERLOCK
		InterlockedAdd(g_counter, incre);
#else
		g_counter += incre;
#endif

		float3 dirOffset = RandomVector3(g_directionOffsetMin, g_directionOffsetMax, dispatchID.xy, g_counter);
#ifdef INTERLOCK
		InterlockedAdd(g_counter, incre);
#else
		g_counter += incre;
#endif

		dirOffset = normalize(dirOffset)* Random(0, max(g_directionOffsetMax.x, max(g_directionOffsetMax.y, g_directionOffsetMax.z)), dispatchID.xy, g_counter);
#ifdef INTERLOCK
		InterlockedAdd(g_counter, incre);
#else
		g_counter += incre;
#endif


		float3 direction = MulQuaternionVector(g_orientation, float3(0, 0, 1));
		particle.m_velocity = mad(direction * g_extent.z, speed , dirOffset);

		particleAtt.m_mass = Random(g_minMass, g_maxMass, dispatchID.xy, g_counter);
#ifdef INTERLOCK
		InterlockedAdd(g_counter, incre);
#else
		g_counter += incre;
#endif
		particle.m_age = particle.m_lifeSpan = Random(g_age.x, g_age.y, dispatchID.xy, g_counter);
#ifdef INTERLOCK
		InterlockedAdd(g_counter, incre);
#else
		g_counter += incre;
#endif
		particle.m_color = RandomVector4(g_randomMinColor, g_randomMaxColor, dispatchID.xy, g_counter);
#ifdef INTERLOCK
		InterlockedAdd(g_counter, incre);
#else
		g_counter += incre;
#endif
		//InterlockedAdd(g_counter, incre);
		particleAtt.m_startColor = g_startColor * particle.m_color;
		particleAtt.m_endColor = g_endColor * particle.m_color;

		particle.m_size = RandomVector3(g_randomStartSize, g_randomEndSize, dispatchID.xy, g_counter);
	
		//InterlockedAdd(g_counter, incre);
		particleAtt.m_startSize = g_startSize * particle.m_size;
		particleAtt.m_endSize = g_endSize * particle.m_size;

		particleAtt.m_startSpeed = g_startSpeed ;
		particleAtt.m_endSpeed = g_endSpeed ;

		particleAtt.m_acceleration = gravity * g_gravityScaler;
		particle.m_shape = g_shape;
		uint index = deadList.Consume();

		particles[index] = particle;
		particleAtts[index] = particleAtt;
	}
}
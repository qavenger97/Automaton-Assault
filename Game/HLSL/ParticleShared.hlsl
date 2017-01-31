cbuffer ConstBufferPerFrame : register(b0)
{
	float4x4	g_viewProjection;
	float4x4	g_viewProjInv;
	float4x4	g_view;
	float4x4	g_viewInv;
	float4x4	g_projection;
	float4x4	g_projectionInv;
	float3		g_viewPos;
	float		g_dt;
	float4		g_viewOrientation;
	uint		g_elapsedTimeHigh;
	uint		g_elapsedTimeLow;
	float		g_uvStep;
	float		g_t;
}

cbuffer EmitterCB : register(b1)
{
	float3		g_position;
	uint		g_spawnPerFrame;

	float4		g_orientation;

	float3		g_directionOffsetMin;
	float		g_minMass;
	float3		g_directionOffsetMax;
	float		g_maxMass;

	float4		g_startColor;
	float4		g_endColor;

	float4		g_randomMinColor;
	float4		g_randomMaxColor;

	float3		g_extent;
	float		g_gravityScaler;

	float3		g_randomStartSize;
	float		g_minSpeed;;
	float3		g_randomEndSize;
	float		g_maxSpeed;
	float3		g_startSize;
	float		g_startSpeed;
	float3		g_endSize;
	float		g_endSpeed;

	float2		g_age;
	uint		g_shape;
	uint		g_elapsedTimeLowPerEmitter;
};

cbuffer DeadlistCount	:	register(b2)
{
	uint	g_deadCount;
	uint3	g_deadPad;
}

cbuffer DeadlistCount : register(b3)
{
	uint	g_aliveCount;
	uint3	g_alivePad;
}

struct Particle
{
	float3		m_position;
	float		m_age;
	float3		m_velocity;
	float		m_lifeSpan;
	float3		m_size;
	uint		m_shape;
	float4		m_color;
};

struct ParticleAtt
{
	float3		m_startSize;
	float		m_startSpeed;
	float3	    m_endSize;
	float		m_endSpeed;

	float4		m_startColor;
	float4		m_endColor;
	float3		m_acceleration;
	float		m_mass;
};

struct GS_PRIMITIVE_DATA
{
	float3	offset;
	float	u;
	float3	normal;
	float	v;
};

struct VS_OUTPUT
{
	float4 position		:	POSITION;
	float4 color		:	COLOR;
	float3 normal		:	NORMAL;
	uint   shapeInfo	:	TEXCOORD1;
	float3 size			:	TEXCOORD2;
	float3 velocity		:	TEXCOORD3;
};

struct PS_INPUT
{
	float4 position		:	SV_POSITION;
	float4 color		:	COLOR;
	float2 uv			:	TEXCOORD0;
	float3 normal		:	NORMAL;
};

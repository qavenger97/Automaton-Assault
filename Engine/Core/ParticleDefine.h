#pragma once
struct Particle
{
	XMFLOAT3	m_position;
	FLOAT		m_age;
	XMFLOAT3	m_velocity;
	FLOAT		m_lifeSpan;
	XMFLOAT3	m_size;
	UINT		m_shape;
	XMFLOAT4	m_color;
};

struct ParticleAtt
{
	XMFLOAT3	m_startSize;
	FLOAT		m_startSpeed;
	XMFLOAT3    m_endSize;
	FLOAT		m_endSpeed;

	XMFLOAT4	m_startColor;
	XMFLOAT4	m_endColor;
	XMFLOAT3	m_acceleration;
	FLOAT		m_mass;
};

struct EmitterData
{
	XMFLOAT3	m_position;
	uint32_t	m_numParticlePerFrame;

	XMFLOAT4	m_orientation;

	XMFLOAT3	m_directionOffsetMin;
	FLOAT		m_minMass;
	XMFLOAT3	m_directionOffsetMax;
	FLOAT		m_maxMass;

	XMFLOAT4	m_startColor;
	XMFLOAT4	m_endColor;

	XMFLOAT4	m_randomStartColor;
	XMFLOAT4	m_randomEndColor;

	XMFLOAT3    m_extent;
	FLOAT		m_gravityScaler;

	XMFLOAT3	m_randomStartSize;
	FLOAT		m_minSpeed;;
	XMFLOAT3	m_randomEndSize;
	FLOAT		m_maxSpeed;
	XMFLOAT3	m_startSize;
	FLOAT		m_startSpeed;
	XMFLOAT3	m_endSize;
	FLOAT		m_endSpeed;

	XMFLOAT2	m_age;
	uint32_t	m_shape;
	uint32_t	m_elapsedTimeLow;
};
struct LiveParticle
{
	uint32_t	index;
	FLOAT		distanceSq;
};

struct DeadParticleConstantBuffer
{
	UINT	count;
	UINT	pad[3];
};

struct AliveParticleConstantBuffer
{
	UINT	count;
	UINT	pad[3];
};

enum class EmitterSpawnType
{
	//spawn particle over time
	Normal,
	//fire all particles at once
	Burst,
	NumOfType
};

enum class EmitterParticleShape
{
	Quad = (0 | (4 << 14)),
	Triangle = (4 | 3 << 14),
	Cube = (7 | 24 << 14),
	Sphere = (32 | 120 << 14),
	Primitive = 0,
	Mesh = 1,
	NumOfShape
};


enum class EmitterParticleType
{
	Default,
	BillBoard,
	Velocity,
	BillBoardVelocity,
	NumOfType
};

struct ParticleConstantPerFrame
{
	XMMATRIX	g_viewProjection;
	XMMATRIX	g_viewProjInv;
	XMMATRIX	g_view;
	XMMATRIX	g_viewInv;
	XMMATRIX	g_projection;
	XMMATRIX	g_projectionInv;
	XMFLOAT3	g_viewPos;
	FLOAT		g_dt;
	XMFLOAT4	g_viewOrientation;
	UINT		g_elapsedTimeHigh;
	UINT		g_elapsedTimeLow;
	FLOAT		g_uvStep;
	FLOAT		g_pad;
};

struct MeshInfo
{
	bool operator==(const MeshInfo& rhs)const { return m_startOffset == rhs.m_startOffset && m_length == rhs.m_length; }
	UINT	m_billBoard : 1;
	UINT	m_startOffset : 15;
	UINT	m_velocityFollow : 1;
	UINT	m_length : 15;
};

struct Vertex_Primitive
{
	XMFLOAT3	m_offset;
	FLOAT		m_u;
	XMFLOAT3	m_normal;
	FLOAT		m_v;
};
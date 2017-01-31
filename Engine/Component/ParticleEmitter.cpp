#include "pch.h"
#include "ParticleEmitter.h"
#include "../Core/Entity.h"
#include "../Core/TimeManager.h"
#include "../Core/ComponentFactory.h"
#include "../Renderer/DebugRenderer.h"
#include "../Core/Math.h"
uint32_t hg::ParticleEmitter::s_TypeID = ComponentFactory::GetSystemComponentID();


void Hourglass::ParticleEmitter::LoadFromXML(tinyxml2::XMLElement* data)
{
	tinyxml2::XMLElement* emitter = nullptr;

	data->QueryFloatAttribute("duration", &m_duration);
	unsigned int max_particles;
	data->QueryUnsignedAttribute("maxParticle", &max_particles);
	SetMaxParticles(max_particles);
	float spawnRate;
	data->QueryFloatAttribute("spawnRate", &spawnRate);
	SetSpawnRate(spawnRate);

	emitter = data->FirstChildElement();

	while (emitter)
	{
		if (strcmp(emitter->Name(), "ParticleFlags") == 0)
		{
			bool value;
			emitter->QueryBoolAttribute("loop", &value);
			SetLoop(value);
			emitter->QueryBoolAttribute("randomRotation", &value);
			SetRandomRotation(value);
			emitter->QueryBoolAttribute("applyGravity", &value);
			ApplyGravity(value);
			emitter->QueryBoolAttribute("playOnStart", &value);
			m_playing = value;
			int i = 0;
		}
		else if (strcmp(emitter->Name(), "Extent") == 0)
		{
			emitter->QueryFloatAttribute("x", &m_extent.x);
			emitter->QueryFloatAttribute("y", &m_extent.y);
			emitter->QueryFloatAttribute("z", &m_extent.z);
		}
		else if (strcmp(emitter->Name(), "MinInitalVelocity") == 0)
		{
			emitter->QueryFloatAttribute("x", &m_minInitalVelocity.x);
			emitter->QueryFloatAttribute("y", &m_minInitalVelocity.y);
			emitter->QueryFloatAttribute("z", &m_minInitalVelocity.z);
		}
		else if (strcmp(emitter->Name(), "MaxInitalVelocity") == 0)
		{
			emitter->QueryFloatAttribute("x", &m_maxInitalVelocity.x);
			emitter->QueryFloatAttribute("y", &m_maxInitalVelocity.y);
			emitter->QueryFloatAttribute("z", &m_maxInitalVelocity.z);
		}
		else if (strcmp(emitter->Name(), "color") == 0)
		{
			emitter->QueryFloatAttribute("r", &m_color.x);
			emitter->QueryFloatAttribute("g", &m_color.y);
			emitter->QueryFloatAttribute("b", &m_color.z);
			emitter->QueryFloatAttribute("a", &m_color.w);
		}
		else
		{
			float minValue, maxValue;
			emitter->QueryFloatAttribute("min", &minValue);
			emitter->QueryFloatAttribute("max", &maxValue);
			if (strcmp(emitter->Name(), "LifeTime") == 0)
				SetLifeTime(minValue, maxValue);
			else if (strcmp(emitter->Name(), "Speed") == 0)
				SetSpeed(minValue, maxValue);
			else if (strcmp(emitter->Name(), "size") == 0)
				SetParticleSize(minValue, maxValue);
		}
		emitter = emitter->NextSiblingElement();
	}
	XMVECTOR minV = XMLoadFloat3(&m_minInitalVelocity);
	XMVECTOR maxV = XMLoadFloat3(&m_maxInitalVelocity);
	XMStoreFloat3(&m_minInitalVelocity, XMVectorMin(minV, maxV));
	XMStoreFloat3(&m_maxInitalVelocity, XMVectorMax(minV, maxV));
}


hg::IComponent * Hourglass::ParticleEmitter::MakeCopyDerived() const
{
	ParticleEmitter* copy = (ParticleEmitter*)IComponent::Create(SID(ParticleEmitter));
	*copy = *this;
	return copy;
}

Hourglass::ParticleEmitter::ParticleEmitter()
	:
	m_duration(0),
	m_extent({ 0.5,0.5,0.5 }),
	m_lifeTime({ 0,0 }),
	m_size({ 0,0 }),
	m_inv_spawnRate(0),
	m_maxParticles(0),
	m_timer(0),
	m_spawnTimer(0),
	m_emit(1)
{
	m_particles.SetMaxParticle(0);
}


void Hourglass::ParticleEmitter::Pause()
{
	m_playing = 0;
}

void Hourglass::ParticleEmitter::Update()
{
	if (m_playing)
	{
		float dt = g_Time.Delta();
		uint16_t currentSize = m_particles.Size();
		if (!m_loop && (m_timer >= m_duration && m_particles.m_liveCount == 0))
		{
			GetEntity()->Destroy();
			return;
		}
		m_particles.Update(m_applyGravity);

		m_timer += dt;
		if ((m_timer < m_duration || m_loop) && m_emit)
		{
			if (currentSize < m_maxParticles || m_loop)
			{
				if (m_spawnTimer >= m_inv_spawnRate)
				{
					Emit();
					m_spawnTimer = 0;
				}
			}
			m_spawnTimer += dt;
		}
	}

	for (uint16_t i = 0; i < m_particles.Size(); i++)
	{
		if(m_particles.m_alive[i])
			hg::DebugRenderer::DrawSphere(m_particles.m_position[i], m_particles.m_size[i], m_color, 4);
	}
}

uint16_t Hourglass::ParticleEmitter::GetLiveCount()
{
	return m_particles.m_liveCount;
}

void Hourglass::ParticleEmitter::Emit()
{
	uint16_t size = m_particles.Size();
	XMVECTOR Ext = XMLoadFloat3(&m_extent);
	Transform* entityTransform = GetEntity()->GetTransform();
	XMVECTOR spawnPos = Math::RNG::RandomInBoundingBox(entityTransform->GetWorldPosition(), Ext, entityTransform->GetWorldRotation());
	XMVECTOR spawnVel = entityTransform->Forward() * Math::RNG::Range(m_speed.x, m_speed.y);
	if(m_randomRotation)
		spawnVel += Math::RNG::RandomVectorRange(XMLoadFloat3(&m_minInitalVelocity), XMLoadFloat3(&m_maxInitalVelocity));

	if (size < m_maxParticles)
	{
		m_particles.AddParticle(spawnPos, spawnVel, Math::RNG::Range(m_lifeTime.x, m_lifeTime.y), Math::RNG::Range(m_size.x, m_size.y));
	}
	else
	{
		std::vector<bool>& isAlive = m_particles.m_alive;
		for (uint16_t t = 0; t < size; t++)
		{
			if (isAlive[t] == false)
			{
				m_particles.AddParticle(t, spawnPos, spawnVel, Math::RNG::Range(m_lifeTime.x, m_lifeTime.y), Math::RNG::Range(m_size.x, m_size.y));
			}
		}
	}
	//m_particles.
}

Hourglass::ParticleEmitter::~ParticleEmitter()
{

}

void Hourglass::ParticleEmitter::Init()
{
	m_particles.SetMaxParticle(m_maxParticles);
	m_particles.m_liveCount = 0;
	if (m_applyGravity)
		m_particles.Integrate = Math::Gravity;
	else
		m_particles.Integrate = Math::NoGravity;
}

void Hourglass::ParticleEmitter::SetWidth(float value)
{
	m_extent.x = value * 0.5f;
}

void Hourglass::ParticleEmitter::SetHeight(float value)
{
	m_extent.y = value*0.5f;
}

void Hourglass::ParticleEmitter::SetDepth(float value)
{
	m_extent.z = value * 0.5f;
}

void Hourglass::ParticleEmitter::SetVolume(float x, float y, float z)
{
	m_extent.x = x*0.5f;
	m_extent.y = y*0.5f;
	m_extent.z = z*0.5f;
}

void Hourglass::ParticleEmitter::SetSpawnRate(float value)
{
	m_inv_spawnRate = 1.0f / value;
}

void Hourglass::ParticleEmitter::SetMaxParticles(uint16_t amount)
{
	m_maxParticles = amount;
}

void Hourglass::ParticleEmitter::SetLoop(bool isLoop)
{
	m_loop = isLoop;
}

void Hourglass::ParticleEmitter::SetRandomRotation(bool isRandom)
{
	m_randomRotation = isRandom;
}

void Hourglass::ParticleEmitter::SetLifeTime(float minLifeTime, float maxLifeTime)
{
	m_lifeTime.x = minLifeTime;
	m_lifeTime.y = maxLifeTime;
	if (m_lifeTime.x > m_lifeTime.y)
	{
		std::swap(m_lifeTime.x, m_lifeTime.y);
	}
}

void Hourglass::ParticleEmitter::ApplyGravity(bool isApply)
{
	m_applyGravity = isApply;
}

void Hourglass::ParticleEmitter::SetSpeed(float minSpeed, float maxSpeed)
{
	if (minSpeed > maxSpeed)
	{
		std::swap(minSpeed, maxSpeed);
	}
	m_speed.x = minSpeed;
	m_speed.y = maxSpeed;
}

void Hourglass::ParticleEmitter::SetParticleSize(float minSize, float maxSize)
{
	if (minSize > maxSize)
	{
		std::swap(minSize, maxSize);
	}
	m_size.x = minSize;
	m_size.y = maxSize;
}

inline bool Hourglass::ParticleEmitter::IsLooping()
{
	return m_loop;
}

inline bool Hourglass::ParticleEmitter::IsPlaying()
{
	return m_playing;
}

void Hourglass::ParticleEmitter::Play()
{
	m_playing = true;
	m_emit = true;
}

void Hourglass::ParticleEmitter::Stop()
{
	m_playing = 0;
	m_particles.Clear();
}

void Hourglass::ParticleEmitter::StopEmit()
{
	m_emit = 0;
}

uint16_t Hourglass::Particles::Size() const
{
	return (uint16_t)m_alive.size();
}

void Hourglass::Particles::SetMaxParticle(uint16_t amount)
{
	m_position.reserve(amount);
	m_velocity.reserve(amount);
	m_lifeTime.reserve(amount);
	m_size.reserve(amount);
	m_alive.reserve(amount);
}

void Hourglass::Particles::AddParticle(XMVECTOR position, XMVECTOR velocity, float lifeTime, float size)
{
	XMFLOAT3 tmp;
	XMStoreFloat3(&tmp, position);
	m_position.push_back(tmp);

	XMStoreFloat3(&tmp, velocity);
	m_velocity.push_back(tmp);
	m_lifeTime.push_back(lifeTime);
	m_size.push_back(size);
	m_alive.push_back(true);
	m_liveCount++;
}

void Hourglass::Particles::AddParticle(uint16_t index, XMVECTOR position, XMVECTOR velocity, float lifeTime, float size)
{
	XMStoreFloat3(&m_position[index], position);
	XMStoreFloat3(&m_velocity[index], velocity);
	m_lifeTime[index] = std::move(lifeTime);
	m_size[index] = std::move(size);
	m_alive[index] = true;
}

void Hourglass::Particles::RemoveParticle(uint16_t index)
{
	m_alive[index] = false;
}

void Hourglass::Particles::Update(bool gravity)
{
	if (Integrate)
	{
		float dt = g_Time.Delta();
		int i;
		for (i = 0; i < Size(); i++)
		{
			if (m_alive[i])
			{
				XMVECTOR newPos = XMLoadFloat3(&m_position[i]), newVel = XMLoadFloat3(&m_velocity[i]);
				Integrate(newPos, newVel, dt, G_GRAVITY);
				XMStoreFloat3(&m_position[i], newPos);
				XMStoreFloat3(&m_velocity[i], newVel);
				m_lifeTime[i] -= dt;
				if (m_lifeTime[i] <= 0)
				{
					m_alive[i] = false;
					m_liveCount--;
				}
			}
		}
	}
}

void Hourglass::Particles::Clear()
{
	m_position.clear();
	m_velocity.clear();
	m_lifeTime.clear();
	m_size.clear();
	m_alive.clear();
}

#include "pch.h"
#include "Core/Entity.h"
#include "../Core/TimeManager.h"
#include "GPUParticleEmitter.h"
#include "../System/GPUParticleSystem.h"
#include "../Engine/Core/ComponentFactory.h"
#include "../Renderer/Material.h"
uint32_t hg::GPUParticleEmitter::s_TypeID = ComponentFactory::GetSystemComponentID();

Hourglass::GPUParticleEmitter::GPUParticleEmitter()
	:
	m_loop(0),
	m_duration(0),
	m_emitAmount(0),
	m_emiting(1),
	m_forceEmit(0),
	m_gravityScale(0),
	m_spawnRate(0),
	m_timer(0),
	m_spawnTimer(0),
	m_minMass(1),
	m_maxMass(1),
	m_spawnType((uint32_t)EmitterSpawnType::Normal),
	m_particleType((uint32_t)EmitterParticleType::BillBoard),
	m_particleShape((uint32_t)EmitterParticleShape::Quad),
	m_maxParticles(0),
	m_playing(0)
{
	
}

void Hourglass::GPUParticleEmitter::LoadFromXML(tinyxml2::XMLElement * data)
{
	tinyxml2::XMLElement* emitter = nullptr;

	std::string type = data->Attribute("EmitterSpawnType");
	if (type == "Normal")
	{
		m_spawnType = (uint32_t)EmitterSpawnType::Normal;
	}
	else if (type == "Burst")
	{
		m_spawnType = (uint32_t)EmitterSpawnType::Burst;
	}

	type = data->Attribute("EmitterParticleType");
	if (type == "Default")
	{
		m_particleType = (uint32_t)EmitterParticleType::Default;
	}
	else if (type == "BillBoard")
	{
		m_particleType = (uint32_t)EmitterParticleType::BillBoard;
		m_particleShape |= (1 << 31);
	}
	else if (type == "Velocity")
	{
		m_particleType = (uint32_t)EmitterParticleType::Velocity;
		m_particleShape |= (1 << 30);
	}
	else if (type == "BillBoardVelocity")
	{
		m_particleType = (uint32_t)EmitterParticleType::BillBoardVelocity;
		m_particleShape |= (1 << 31);
		m_particleShape |= (1 << 30);
	}

	type = data->Attribute("EmitterParticleShape");
	if (type == "Cube")
	{
		m_particleShape |= (uint32_t)EmitterParticleShape::Cube;
	}
	else if (type == "Mesh")
	{
		m_particleShape = (uint32_t)EmitterParticleShape::Mesh;
	}
	else if (type == "Quad")
	{
		m_particleShape |= (uint32_t)EmitterParticleShape::Quad;
	}
	else if (type == "Sphere")
	{
		m_particleShape |= (uint32_t)EmitterParticleShape::Sphere;
	}
	else if (type == "Triangle")
	{
		m_particleShape |= (uint32_t)EmitterParticleShape::Triangle;
	}

	if(m_particleShape == (uint32_t)EmitterParticleShape::Mesh)
		hg::g_particleSystemGPU.AddShape(EmitterParticleShape::Mesh);
	else
		hg::g_particleSystemGPU.AddShape(EmitterParticleShape::Primitive);

	data->QueryFloatAttribute("duration", &m_duration);
	unsigned int max_particles;
	data->QueryUnsignedAttribute("maxParticle", &max_particles);
	m_maxParticles = max_particles;
	float spawnRate;
	data->QueryFloatAttribute("spawnRate", &spawnRate);
	m_spawnRate = (uint32_t)spawnRate;
	m_invSpawnRate = 1 / spawnRate;

	data->QueryFloatAttribute("gravityScale", &m_gravityScale);

	emitter = data->FirstChildElement();

	while (emitter)
	{
		if (strcmp(emitter->Name(), "ParticleFlags") == 0)
		{
			bool value = false;
			emitter->QueryBoolAttribute("loop", &value);
			m_loop = value;
			value = false;
			emitter->QueryBoolAttribute("playOnStart", &value);
			m_playing = value;
			value = false;
			emitter->QueryBoolAttribute("keepAlive", &value);
			m_keepAlive = value;
			value = true;
			emitter->QueryBoolAttribute("local", &value);
			m_localSimulation = value;
		}
		else if (strcmp(emitter->Name(), "Extent") == 0)
		{
			emitter->QueryFloatAttribute("x", &m_extent.x);
			emitter->QueryFloatAttribute("y", &m_extent.y);
			emitter->QueryFloatAttribute("z", &m_extent.z);
		}
		else if (strcmp(emitter->Name(), "Speed") == 0)
		{
			emitter->QueryFloatAttribute("start", &m_startSpeed);
			emitter->QueryFloatAttribute("end", &m_endSpeed);
		}
		else if (strcmp(emitter->Name(), "RandomStartSpeed") == 0)
		{
			emitter->QueryFloatAttribute("min", &m_minSpeed);
			emitter->QueryFloatAttribute("max", &m_maxSpeed);
		}
		else if (strcmp(emitter->Name(), "RandomStartSizeMin") == 0)
		{
			emitter->QueryFloatAttribute("x", &m_minSize.x);
			emitter->QueryFloatAttribute("y", &m_minSize.y);
			emitter->QueryFloatAttribute("z", &m_minSize.z);
		}
		else if (strcmp(emitter->Name(), "RandomStartSizeMax") == 0)
		{
			emitter->QueryFloatAttribute("x", &m_maxSize.x);
			emitter->QueryFloatAttribute("y", &m_maxSize.y);
			emitter->QueryFloatAttribute("z", &m_maxSize.z);
		}
		else if (strcmp(emitter->Name(), "StartSize") == 0)
		{
			emitter->QueryFloatAttribute("x", &m_startSize.x);
			emitter->QueryFloatAttribute("y", &m_startSize.y);
			emitter->QueryFloatAttribute("z", &m_startSize.z);
		}
		else if (strcmp(emitter->Name(), "EndSize") == 0)
		{
			emitter->QueryFloatAttribute("x", &m_endSize.x);
			emitter->QueryFloatAttribute("y", &m_endSize.y);
			emitter->QueryFloatAttribute("z", &m_endSize.z);
		}
		else if (strcmp(emitter->Name(), "MinInitalVelocity") == 0)
		{
			emitter->QueryFloatAttribute("x", &m_minInitDirection.x);
			emitter->QueryFloatAttribute("y", &m_minInitDirection.y);
			emitter->QueryFloatAttribute("z", &m_minInitDirection.z);
		}
		else if (strcmp(emitter->Name(), "MaxInitalVelocity") == 0)
		{
			emitter->QueryFloatAttribute("x", &m_maxInitDirection.x);
			emitter->QueryFloatAttribute("y", &m_maxInitDirection.y);
			emitter->QueryFloatAttribute("z", &m_maxInitDirection.z);
		}
		else if (strcmp(emitter->Name(), "LifeTime") == 0)
		{
			emitter->QueryFloatAttribute("min", &m_minLifeTime);
			emitter->QueryFloatAttribute("max", &m_maxLifeTime);
		}
		else if (strcmp(emitter->Name(), "StartColor") == 0)
		{
			emitter->QueryFloatAttribute("r", &m_startColor.x);
			emitter->QueryFloatAttribute("g", &m_startColor.y);
			emitter->QueryFloatAttribute("b", &m_startColor.z);
			emitter->QueryFloatAttribute("a", &m_startColor.w);
		}
		else if (strcmp(emitter->Name(), "EndColor") == 0)
		{
			emitter->QueryFloatAttribute("r", &m_endColor.x);
			emitter->QueryFloatAttribute("g", &m_endColor.y);
			emitter->QueryFloatAttribute("b", &m_endColor.z);
			emitter->QueryFloatAttribute("a", &m_endColor.w);
		}
		else if (strcmp(emitter->Name(), "RandomStartColor") == 0)
		{
			emitter->QueryFloatAttribute("r", &m_minColor.x);
			emitter->QueryFloatAttribute("g", &m_minColor.y);
			emitter->QueryFloatAttribute("b", &m_minColor.z);
			emitter->QueryFloatAttribute("a", &m_minColor.w);
		}
		else if (strcmp(emitter->Name(), "RandomEndColor") == 0)
		{
			emitter->QueryFloatAttribute("r", &m_maxColor.x);
			emitter->QueryFloatAttribute("g", &m_maxColor.y);
			emitter->QueryFloatAttribute("b", &m_maxColor.z);
			emitter->QueryFloatAttribute("a", &m_maxColor.w);
		}
		else if (strcmp(emitter->Name(), "Mass") == 0)
		{
			emitter->QueryFloatAttribute("min", &m_minMass);
			emitter->QueryFloatAttribute("max", &m_maxMass);
		}
		else if (strcmp(emitter->Name(), "Material") == 0)
		{
			std::string path = emitter->Attribute("file");
			m_particleTexture = hg::g_particleSystemGPU.AddTexture(path);
			
		}
		emitter = emitter->NextSiblingElement();
	}
}

hg::IComponent * Hourglass::GPUParticleEmitter::MakeCopyDerived() const
{
	GPUParticleEmitter* copy = (GPUParticleEmitter*)IComponent::Create(SID(GPUParticleEmitter));
	*copy = *this;
	return copy;
}

void Hourglass::GPUParticleEmitter::Init()
{

}

void Hourglass::GPUParticleEmitter::Update()
{
	if (m_forceEmit)
	{
		m_spawnTimer = 0;
		m_forceEmit &= 0;
		return;
	}
	m_emitAmount = 0;
	if (m_playing)
	{
		assert(m_spawnType != (uint32_t)EmitterSpawnType::NumOfType &&
			m_particleShape != (uint32_t)EmitterParticleShape::NumOfShape &&
			m_particleType != (uint32_t)EmitterParticleType::NumOfType
		);
		
		if (m_timer >= m_duration)
		{
			m_playing = 0;
			if(!m_keepAlive)
				GetEntity()->Destroy();
			return;
		}
		float dt = hg::g_Time.Delta();
		
		if (m_timer < m_duration && m_emiting)
		{
			m_spawnTimer += dt;
			if (m_spawnType == (uint32_t)EmitterSpawnType::Normal && m_spawnTimer >= m_invSpawnRate)
			{
				m_emitAmount = (uint32_t)(m_spawnTimer * m_spawnRate);
				m_spawnTimer -= m_emitAmount * m_invSpawnRate;
			}
			else if (m_spawnType == (uint32_t)EmitterSpawnType::Burst)
			{
				m_emitAmount = m_maxParticles;
				m_emiting = 0;
				m_spawnTimer = 0;
			}
			
		}

		if(!m_loop)
			m_timer += dt;
	}
}

void Hourglass::GPUParticleEmitter::Emit(uint32_t numParticle)
{
	m_emitAmount = numParticle;
	m_forceEmit |= 1;
}

void Hourglass::GPUParticleEmitter::Play()
{
	m_playing |= 1;
	m_emiting |= 1;
}

bool Hourglass::GPUParticleEmitter::IsPlaying()
{
	return m_playing;
}

uint32_t Hourglass::GPUParticleEmitter::GetNumEmission() const
{
	return m_emitAmount;
}

EmitterData Hourglass::GPUParticleEmitter::GetData() const
{
	EmitterData data;
	auto* trans = GetEntity()->GetTransform();
	data.m_extent = m_extent;
	data.m_extent.z = m_localSimulation;
	data.m_numParticlePerFrame = m_emitAmount;
	XMStoreFloat4(&data.m_orientation, trans->GetWorldRotation());
	XMStoreFloat3(&data.m_position, trans->GetWorldPosition());

	data.m_minSpeed = m_minSpeed;
	data.m_maxSpeed = m_maxSpeed;
	data.m_startSpeed = m_startSpeed;
	data.m_endSpeed = m_endSpeed;

	data.m_age.x = m_minLifeTime;
	data.m_age.y = m_maxLifeTime;
	data.m_gravityScaler = m_gravityScale;

	data.m_startSize = m_startSize;
	data.m_endSize = m_endSize;
	data.m_randomStartSize = m_minSize;
	data.m_randomEndSize = m_maxSize;

	data.m_startColor = m_startColor;
	data.m_endColor = m_endColor;
	data.m_randomStartColor = m_minColor;
	data.m_randomEndColor = m_maxColor;

	data.m_directionOffsetMin = m_minInitDirection;
	data.m_directionOffsetMax = m_maxInitDirection;
	data.m_elapsedTimeLow = (uint32_t)hg::g_Time.Counter();
	data.m_minMass = m_minMass;
	data.m_maxMass = m_maxMass;
	data.m_shape = m_particleShape;
	return data;
}

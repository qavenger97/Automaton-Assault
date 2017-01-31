#include "pch.h"
#include "Core/Shader.h"
#include "ParticleSystem.h"

namespace Hourglass
{
	void ParticleSystem::Init()
	{
		ComponentPoolInit( SID(ParticleEmitter), m_ParticleEffects, &m_ParticleEffectPool, s_kMaxParticleEffects );
	}

	void ParticleSystem::Update()
	{
		for (unsigned int i = 0; i < s_kMaxParticleEffects; ++i)
		{
			if (m_ParticleEffects[i].IsAlive() && m_ParticleEffects[i].IsEnabled())
			{
				m_ParticleEffects[i].Update();
			}
		}
	}
}
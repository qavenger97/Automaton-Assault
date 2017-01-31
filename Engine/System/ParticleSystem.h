#pragma once

#include "Component/ParticleEmitter.h"
#include "Core/ComponentPool.h"
#include "System/ISystem.h"
#include "Renderer/RenderDevice.h"
namespace Hourglass
{
	//struct ParticleEmitterCB
	//{
	//	XMFLOAT3 m_position;
	//	float m_age;

	//	XMFLOAT3 m_extent;
	//	int m_spawnAmount;

	//	XMFLOAT3 m_velocity;
	//	float m_speed;

	//	XMFLOAT3 m_acceleration;
	//	float m_gravityScaler;

	//	float m_startSize;
	//	float m_endSize;
	//	float m_speedVariance;
	//	float m_sizeVariance;

	//};

	class ParticleSystem : public ISystem
	{
	public:

		void Init();

		/*
		* Update all particle effect components for current frame
		*/
		void Update();
	private:

		static const unsigned int s_kMaxParticleEffects = 2048;
		
		ParticleEmitter m_ParticleEffects[s_kMaxParticleEffects];
		ComponentPool<ParticleEmitter> m_ParticleEffectPool;
	};
}
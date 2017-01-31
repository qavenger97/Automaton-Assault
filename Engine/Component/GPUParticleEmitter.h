#pragma once
#include "../Engine/Component/IComponent.h"
#include "../Core/ParticleDefine.h"
class GPUParticleSystem;
namespace Hourglass
{
	class Material;
	class GPUParticleEmitter : public IComponent
	{
	public:
		GPUParticleEmitter();
		virtual int GetTypeID() const { return s_TypeID; }

		void LoadFromXML(tinyxml2::XMLElement * data);

		/**
		* Make a copy of this component
		*/
		virtual IComponent* MakeCopyDerived() const;

		void Init()override;
		void Update()override;
		void Emit(uint32_t numParticle);
		void Play();
		bool IsPlaying();
		//void Pause();
		//void Stop();
		uint32_t GetNumEmission()const;
		EmitterData GetData()const;
	public:
		static uint32_t s_TypeID;

	private:
		friend GPUParticleSystem;
		uint16_t m_loop : 1;
		uint16_t m_playing : 1;
		uint16_t m_emiting : 1;
		uint16_t m_forceEmit : 1;
		uint16_t m_keepAlive : 1;
		uint16_t m_localSimulation : 1;
		uint32_t m_spawnType;
		uint32_t m_particleType;
		// 31			BillBoard
		// 20 - 16		length
		// 15			VelocityFollow
		// 14 - 0		offset
		uint32_t m_particleShape;
		uint32_t m_particleTexture;
		// Particle spawn area
		XMFLOAT3 m_extent;
		//// Gravity modifier : 0 = no gravity
		float m_gravityScale;
		//// How long the emitter spawner particles
		float m_duration;
		//// How many particles the emitter spawns per second
		uint32_t m_spawnRate;
		float m_invSpawnRate;
		//// Particle size from begin to end
		XMFLOAT3 m_startSize;
		XMFLOAT3 m_endSize;
		
		XMFLOAT3 m_minSize;
		XMFLOAT3 m_maxSize;

		float m_minLifeTime;
		float m_maxLifeTime;

		float m_minSpeed;
		float m_maxSpeed;
		float m_startSpeed;
		float m_endSpeed;

		float m_minMass;
		float m_maxMass;

		uint32_t m_maxParticles;

		XMFLOAT4 m_minColor;
		XMFLOAT4 m_maxColor;

		XMFLOAT4 m_startColor;
		XMFLOAT4 m_endColor;

		XMFLOAT3 m_minInitDirection;
		XMFLOAT3 m_maxInitDirection;
		float m_timer;
		float m_spawnTimer;

		uint32_t m_emitAmount;
	};
}

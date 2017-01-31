#pragma once
#include "IComponent.h"
namespace Hourglass
{
	//struct Particle
	//{
	//	XMFLOAT3	m_position;
	//	FLOAT		m_lifeTime;

	//	XMFLOAT3	m_velocity;
	//	FLOAT		m_age;

	//	XMFLOAT4	m_color;

	//	XMFLOAT3	m_acceleration;
	//	FLOAT		m_mass;
	//};

	//struct LiveParticle
	//{
	//	uint32_t	m_index;
	//	FLOAT		m_distanceSq;
	//};

	//struct EmitterData
	//{
	//	XMFLOAT3	m_position;
	//	UINT		m_spawnPerFrame;

	//	XMFLOAT4	m_orientation;

	//	XMFLOAT3	m_extent;
	//	FLOAT		m_gravityScaler;
	//	
	//	XMFLOAT2	m_speed;
	//	XMFLOAT2	m_size;

	//	XMFLOAT2	m_age;
	//	XMFLOAT2	pad;
	//};

	struct Particles
	{
		std::vector<XMFLOAT3> m_position;
		std::vector<float> m_lifeTime;
		std::vector<XMFLOAT3> m_velocity;
		std::vector<float> m_size;
		std::vector<bool> m_alive;
		uint16_t Size()const;
		uint16_t m_liveCount;
		void SetMaxParticle(uint16_t amount);
		void AddParticle(XMVECTOR position, XMVECTOR velocity, float lifeTime, float size);
		void AddParticle(uint16_t index, XMVECTOR position, XMVECTOR velocity, float lifeTime, float size);
		void RemoveParticle(uint16_t index);
		void Update(bool gravity);
		void Clear();
		void(*Integrate)(XMVECTOR&, XMVECTOR&, float, const XMVECTOR&);
	};

	class ParticleEmitter : public IComponent
	{
	public:
		virtual int GetTypeID() const { return s_TypeID; }

		void LoadFromXML(tinyxml2::XMLElement * data);

		/**
		* Make a copy of this component
		*/
		virtual IComponent* MakeCopyDerived() const;

		ParticleEmitter();
		~ParticleEmitter();
		void Init();
		void SetWidth(float value);
		void SetHeight(float value);
		void SetDepth(float value);
		void SetVolume(float x, float y, float z);
		//particle per second
		void SetSpawnRate(float value);
		void SetMaxParticles(uint16_t amount);
		void SetLoop(bool isLoop);
		void SetRandomRotation(bool isRandom);
		void SetLifeTime(float minLifeTime, float maxLifeTime);
		void ApplyGravity(bool isApply);
		void SetSpeed(float minSpeed, float maxSpeed);
		void SetParticleSize(float minSize, float maxSize);
		inline bool IsLooping();
		inline bool IsPlaying();
		void Play();
		void Stop();
		void StopEmit();
		void Pause();
		void Update();
		uint16_t GetLiveCount();
	public:
		static uint32_t s_TypeID;
	private:
		unsigned char m_loop : 1;
		unsigned char m_randomRotation : 1;
		unsigned char m_applyGravity : 1;
		unsigned char m_playing : 1;
		unsigned char m_emit : 1;

		float m_duration;
		XMFLOAT3 m_extent;
		XMFLOAT3 m_minInitalVelocity;
		XMFLOAT3 m_maxInitalVelocity;
		XMFLOAT2 m_lifeTime;
		XMFLOAT2 m_speed;
		XMFLOAT2 m_size;
		XMFLOAT4 m_color;
		float m_inv_spawnRate;
		uint16_t m_maxParticles;
		Particles m_particles;

		float m_timer;
		float m_spawnTimer;
		//uint16_t m_firstAvailible;
		//uint16_t m_first;
	private:
		void Emit();
	};

}

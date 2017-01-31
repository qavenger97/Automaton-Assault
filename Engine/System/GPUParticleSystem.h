#pragma once

#include "ISystem.h"
#include "../Core/ComponentFactory.h"
#include "../Component/GPUParticleEmitter.h"
#include "../Core/ParticleDefine.h"
#include <unordered_map>
#include "GPUSort.h"
namespace Hourglass
{
#define _MAX_PARTICLE_SQRT	256
#define _SHAPE_PRIMITIVE_SIZE  151
	class GPUSort;
	class GPUParticleSystem : public ISystem
	{
	public:

		void Init();

		/*
		* Update all particle effect components for current frame
		*/
		void Update();
		void Start();
		void Render();
		void Reset();
		void AddShape(EmitterParticleShape shape);
		UINT AddTexture(std::string path);
	private:
		void InitDeadList();
		HRESULT InitShader();
		HRESULT CreateBuffer();
		void Emit();
		void Simulate();
		HRESULT CreateMeshBuffer();
		HRESULT CreateTextureAtlas();
#if 0
		uint32_t ReadAliveCount();
		uint32_t ReadCount(ID3D11UnorderedAccessView*);
#endif
	private:
		static const unsigned int s_kMaxParticleEmitters = 2048;
		static const unsigned int s_kMaxParticles = _MAX_PARTICLE_SQRT * _MAX_PARTICLE_SQRT;
		std::unordered_map<EmitterParticleShape, UINT>	m_particleShapes;
		std::unordered_map<std::string, UINT>			m_particleTextures;
		GPUParticleEmitter								m_ParticleEmitters[s_kMaxParticleEmitters];
		ComponentPool<GPUParticleEmitter>				m_ParticleEmitterPool;

		ComPtr<ID3D11ComputeShader>						m_pCSParticleEmit;
		ComPtr<ID3D11ComputeShader>						m_pCSParticleUpdate;
		ComPtr<ID3D11ComputeShader>						m_pCSInitDeadList;

		ComPtr<ID3D11VertexShader>						m_pVSDraw;
		std::vector<ComPtr<ID3D11GeometryShader>>		m_pGSDraw;
		ComPtr<ID3D11PixelShader>						m_pPSDraw;

		std::vector<ComPtr<ID3D11Buffer>>				m_particleBuffer;
		std::vector<ComPtr<ID3D11Buffer>>				m_particleAttBuffer;
		std::vector<ComPtr<ID3D11Buffer>>				m_deadListBuffer;
		std::vector<ComPtr<ID3D11Buffer>>				m_sortListBuffer;
		std::vector<ComPtr<ID3D11Buffer>>				m_deadListCB;

		ComPtr<ID3D11Buffer>							m_meshDataBuffer;
		ComPtr<ID3D11ShaderResourceView>				m_meshData_SRV;
		std::vector<ComPtr<ID3D11Texture2D>>			m_texturesBuffer;
		std::vector<ComPtr<ID3D11ShaderResourceView>>	m_textures_SRV;

		ComPtr<ID3D11Buffer>							m_emitterCB;
		ComPtr<ID3D11Buffer>							m_particleSystemCB;
		std::vector<ComPtr<ID3D11Buffer>>				m_drawArgs;
		std::vector<ComPtr<ID3D11Buffer>>				m_sortListCB;

		std::vector<ComPtr<ID3D11ShaderResourceView>>	m_particlePool_SRV;
		std::vector<ComPtr<ID3D11ShaderResourceView>>	m_sortList_SRV;

		std::vector<ComPtr<ID3D11UnorderedAccessView>>	m_particlePool_UAV;
		std::vector<ComPtr<ID3D11UnorderedAccessView>>	m_particleAttPool_UAV;
		std::vector<ComPtr<ID3D11UnorderedAccessView>>	m_deadList_UAV;
		std::vector<ComPtr<ID3D11UnorderedAccessView>>	m_sortList_UAV;
		std::vector<ComPtr<ID3D11UnorderedAccessView>>	m_drawArgs_UAV;
		GPUSort											m_sort;
	};
	extern GPUParticleSystem g_particleSystemGPU;
}

#include "pch.h"
#include "GPUParticleSystem.h"
#include "Renderer/RenderDevice.h"
#include "Core/Shader.h"
#include "Core/TimeManager.h"
#include "System/CameraSystem.h"
#include "Core/EntityAssembler.h"
#include "../Renderer/Material.h"
#include "../Renderer/Texture.h"
Hourglass::GPUParticleSystem Hourglass::g_particleSystemGPU;

void Hourglass::GPUParticleSystem::Init()
{
	ComponentPoolInit(SID(GPUParticleEmitter), m_ParticleEmitters, &m_ParticleEmitterPool, s_kMaxParticleEmitters);

}

#include "../Core/Profiler.h"
void Hourglass::GPUParticleSystem::Update()
{
	for (uint32_t i = 0; i < s_kMaxParticleEmitters; i++)
	{
		GPUParticleEmitter& particle = m_ParticleEmitters[i];
		if (particle.IsAlive() && particle.IsEnabled())
			particle.Update();
	}
	auto* main = CameraSystem::GetMainCamera();
	if (main)
	{
		BEGIN_PROFILER_BLOCK("Mapping System Constant");
		D3D11_MAPPED_SUBRESOURCE mr;
		hg::g_RenderDev.deviceContext->Map(m_particleSystemCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
		ParticleConstantPerFrame* cb = (ParticleConstantPerFrame*)mr.pData;
		cb->g_dt = g_Time.Delta();
		cb->g_pad = g_Time.GameElapsed();
		ULONGLONG eT = (ULONGLONG)g_Time.Counter();
		cb->g_elapsedTimeHigh = (UINT)(eT >> 32);
		cb->g_elapsedTimeLow = (UINT)(eT & 0xffffffff);
		cb->g_projection = main->GetProjectionMatrix();
		cb->g_projectionInv = main->GetInverseProjectionMatrix();
		cb->g_view = main->GetViewMatrix();
		cb->g_viewInv = main->GetViewMatrixInverse();
		XMStoreFloat3(&cb->g_viewPos, main->GetEntity()->GetTransform()->GetWorldPosition());
		XMStoreFloat4(&cb->g_viewOrientation, main->GetEntity()->GetTransform()->GetWorldRotation());
		cb->g_viewProjection = main->GetViewProjectionMatrix();
		cb->g_viewProjInv = main->GetViewProjectionInverseMatrix();
		cb->g_uvStep = 0;
		if(m_particleTextures.size())
			cb->g_uvStep = 1.0f / (float)m_particleTextures.size();
		hg::g_RenderDev.deviceContext->Unmap(m_particleSystemCB.Get(), 0);

		hg::g_RenderDev.deviceContext->CSSetConstantBuffers(0, 1, m_particleSystemCB.GetAddressOf());

		END_PROFILER_BLOCK("Mapping System Constant");

		Emit();

			
		BEGIN_PROFILER_BLOCK("Simulate Particles");
		Simulate();
		END_PROFILER_BLOCK("Simulate Particles");
		for (UINT i = 0; i < m_particleShapes.size(); i++)
		{
			hg::g_RenderDev.deviceContext->CopyStructureCount(m_sortListCB[i].Get(), 0, m_sortList_UAV[i].Get());
			m_sort.Execute(s_kMaxParticles, m_sortList_UAV[i].Get(), m_sortListCB[i].Get());
		}
	}
}

void Hourglass::GPUParticleSystem::Start()
{

	UINT count = (UINT)m_particleShapes.size();

	//CreateTextureAtlas();
	
	m_particleBuffer.resize(count);
	m_particleAttBuffer.resize(count);
	m_particleAttPool_UAV.resize(count);
	m_deadListBuffer.resize(count);
	m_sortListBuffer.resize(count);
	m_deadListCB.resize(count);
	m_sortListCB.resize(count);

	m_drawArgs.resize(count);

	m_sortList_SRV.resize(count);
	m_particlePool_SRV.resize(count);
	
	m_particlePool_UAV.resize(count);
	m_deadList_UAV.resize(count);
	m_sortList_UAV.resize(count);
	m_drawArgs_UAV.resize(count);

	m_pGSDraw.resize(count);


	if (FAILED(InitShader()))
	{
		return;
	}

	if (FAILED(CreateBuffer()))
	{
		return;
	}

	InitDeadList();
	m_sort.Init();
}

void Hourglass::GPUParticleSystem::Render()
{
	for (int i = 0; i < (int)m_particleShapes.size(); i++)
	{
		ID3D11ShaderResourceView* srvs[] = { m_particlePool_SRV[i].Get(), m_sortList_SRV[i].Get() };
		ID3D11ShaderResourceView* srvsGeo[] = { m_meshData_SRV.Get()};
		//hg::g_RenderDev.deviceContext->GSSetShaderResources(0)
		ID3D11DeviceContext* dc = hg::g_RenderDev.deviceContext;
		ID3D11Buffer* vb = nullptr;
		UINT stride = 0;
		UINT offset = 0;
		dc->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
		dc->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
		dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		ID3D11Buffer* cbs[] = { m_particleSystemCB.Get(), nullptr, m_deadListCB[i].Get(), m_sortListCB[i].Get() };
		dc->GSSetConstantBuffers(0, 1, m_particleSystemCB.GetAddressOf());
		dc->GSSetShaderResources(0, ARRAYSIZE(srvsGeo), srvsGeo);
		dc->VSSetConstantBuffers(0, ARRAYSIZE(cbs), cbs);
		dc->VSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
		dc->VSSetShader(m_pVSDraw.Get(), 0, 0);
		dc->GSSetShader(m_pGSDraw[i].Get(), 0, 0);
		dc->PSSetShader(m_pPSDraw.Get(), 0, 0);
		const float factor[] = { 0,0,0,0 };
		dc->OMSetBlendState(hg::g_BlendState[kBlend_AlphaBlend].Get(), factor, 0xffffffff);
		dc->DrawInstancedIndirect(m_drawArgs[i].Get(), 0);
		
		ZeroMemory(srvs, sizeof(srvs));
		ZeroMemory(srvsGeo, sizeof(srvsGeo));
		dc->GSSetShaderResources(0, ARRAYSIZE(srvsGeo), srvsGeo);
		dc->VSSetShaderResources(0, ARRAYSIZE(srvs), srvs);
		dc->GSSetShader(nullptr, 0, 0);
	}
}

void Hourglass::GPUParticleSystem::Reset()
{
	InitDeadList();
}

void Hourglass::GPUParticleSystem::AddShape(EmitterParticleShape shape)
{
	if (m_particleShapes.find(shape) == m_particleShapes.end())
	{
		m_particleShapes[shape] = (UINT)m_particleShapes.size();
	}
}

UINT Hourglass::GPUParticleSystem::AddTexture(std::string path)
{
	UINT index = 0;
	auto itr = m_particleTextures.find(path);
	if (itr == m_particleTextures.end())
	{
		index = (UINT)m_particleTextures.size();
		m_particleTextures[path] = index;
	}
	else
	{
		index = itr->second;
	}
	return index;
}

void Hourglass::GPUParticleSystem::Emit()
{
	BEGIN_PROFILER_BLOCK("Emit");

	hg::g_RenderDev.deviceContext->CSSetShader(m_pCSParticleEmit.Get(), nullptr, 0);

	for (uint32_t i = 0; i < s_kMaxParticleEmitters; i++)
	{
		if (m_ParticleEmitters[i].IsAlive() && m_ParticleEmitters[i].IsEnabled())
		{
			uint32_t amount = m_ParticleEmitters[i].GetNumEmission();
			if (amount > 0)
			{
				D3D11_MAPPED_SUBRESOURCE mr;
				hg::g_RenderDev.deviceContext->Map(m_emitterCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mr);
				EmitterData data = m_ParticleEmitters[i].GetData();

				*(EmitterData*)mr.pData = data;
				hg::g_RenderDev.deviceContext->Unmap(m_emitterCB.Get(), 0);

				uint32_t index = m_particleShapes[(EmitterParticleShape)data.m_shape == EmitterParticleShape::Mesh ? EmitterParticleShape::Mesh : EmitterParticleShape::Primitive];

				UINT initalCount[] = { (UINT)-1 , (UINT)-1, (UINT)-1 };
				ID3D11UnorderedAccessView* views[] = { m_particlePool_UAV[index].Get(), m_particleAttPool_UAV[index].Get(),  m_deadList_UAV[index].Get() };
				hg::g_RenderDev.deviceContext->CSSetUnorderedAccessViews(0, ARRAYSIZE(views), views, initalCount);


				
				hg::g_RenderDev.deviceContext->CopyStructureCount(m_deadListCB[index].Get(), 0, m_deadList_UAV[index].Get());
#if 0
				int num = ReadCount(m_deadList_UAV.Get());
#endif
				ID3D11Buffer* cbs[] = { m_emitterCB.Get(), m_deadListCB[index].Get() };

				hg::g_RenderDev.deviceContext->CSSetConstantBuffers(1, ARRAYSIZE(cbs), cbs);

				UINT dispatchAmount = Math::Alignment(amount, 256) / 256;
				hg::g_RenderDev.deviceContext->Dispatch(dispatchAmount, 1, 1);
			}
		}

	}
	END_PROFILER_BLOCK("Emit");
}

void Hourglass::GPUParticleSystem::Simulate()
{
	hg::g_RenderDev.deviceContext->CSSetShader(m_pCSParticleUpdate.Get(), nullptr, 0);
	UINT count = (UINT)m_particleShapes.size();
	UINT i;
	for (i = 0; i < count; i++)
	{
		ID3D11UnorderedAccessView* views[] = { m_particlePool_UAV[i].Get(), m_particleAttPool_UAV[i].Get(),  m_deadList_UAV[i].Get(), m_drawArgs_UAV[i].Get(), m_sortList_UAV[i].Get() };
		UINT initalCount[] = { (UINT)-1 ,(UINT)-1 , (UINT)-1, (UINT)-1, (UINT)0 };
		hg::g_RenderDev.deviceContext->CSSetUnorderedAccessViews(0, ARRAYSIZE(views), views, initalCount);

		hg::g_RenderDev.deviceContext->Dispatch(Math::Alignment(s_kMaxParticles, 256) / 256, 1, 1);
	}
	ID3D11UnorderedAccessView* uavs[] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	hg::g_RenderDev.deviceContext->CSSetUnorderedAccessViews(0, ARRAYSIZE(uavs), uavs, nullptr);
}

HRESULT Hourglass::GPUParticleSystem::CreateMeshBuffer()
{
#pragma region VertsDef
	{
		Vertex_Primitive verts[_SHAPE_PRIMITIVE_SIZE] = {};
		//Quad
		verts[0].m_offset = XMFLOAT3(-1, 1, 0);
		verts[0].m_normal = XMFLOAT3(0, 0, -1);
		verts[0].m_u = 0;
		verts[0].m_v = 1;

		verts[1].m_offset = XMFLOAT3(1, 1, 0);
		verts[1].m_normal = XMFLOAT3(0, 0, -1);
		verts[1].m_u = 1;
		verts[1].m_v = 1;

		verts[2].m_offset = XMFLOAT3(-1, -1, 0);
		verts[2].m_normal = XMFLOAT3(0, 0, -1);
		verts[2].m_u = 0;
		verts[2].m_v = 0;

		verts[3].m_offset = XMFLOAT3(1, -1, 0);
		verts[3].m_normal = XMFLOAT3(0, 0, -1);
		verts[3].m_u = 1;
		verts[3].m_v = 0;

		//Triangle
		verts[4].m_offset = XMFLOAT3(0, 1, 0);
		verts[4].m_normal = XMFLOAT3(0, 0, -1);
		verts[4].m_u = 0.5;
		verts[4].m_v = 1;

		verts[5].m_offset = XMFLOAT3(1, -1, 0);
		verts[5].m_normal = XMFLOAT3(0, 0, -1);
		verts[5].m_u = 1;
		verts[5].m_v = 0;

		verts[6].m_offset = XMFLOAT3(-1, -1, 0);
		verts[6].m_normal = XMFLOAT3(0, 0, -1);
		verts[6].m_u = 0;
		verts[6].m_v = 0;

		const XMFLOAT3 offsets[8] =
		{
			XMFLOAT3(-1,  1, -1),
			XMFLOAT3(1,  1, -1),
			XMFLOAT3(-1, -1, -1),
			XMFLOAT3(1, -1, -1),
			XMFLOAT3(-1,  1,  1),
			XMFLOAT3(1,  1,  1),
			XMFLOAT3(-1, -1,  1),
			XMFLOAT3(1, -1,  1)
		};

		//Cube Front Face
		verts[7].m_offset = offsets[0];
		verts[7].m_normal = XMFLOAT3(0, 0, -1);
		verts[7].m_u = 0;
		verts[7].m_v = 1;

		verts[8].m_offset = offsets[1];
		verts[8].m_normal = XMFLOAT3(0, 0, -1);
		verts[8].m_u = 1;
		verts[8].m_v = 1;

		verts[9].m_offset = offsets[2];
		verts[9].m_normal = XMFLOAT3(0, 0, -1);
		verts[9].m_u = 0;
		verts[9].m_v = 0;

		verts[10].m_offset = offsets[3];
		verts[10].m_normal = XMFLOAT3(0, 0, -1);
		verts[10].m_u = 1;
		verts[10].m_v = 0;

		//Cube Right Face
		verts[11].m_offset = offsets[1];
		verts[11].m_normal = XMFLOAT3(1, 0, 0);
		verts[11].m_u = 0;
		verts[11].m_v = 1;

		verts[12].m_offset = offsets[5];
		verts[12].m_normal = XMFLOAT3(1, 0, 0);
		verts[12].m_u = 1;
		verts[12].m_v = 1;

		verts[13].m_offset = offsets[3];
		verts[13].m_normal = XMFLOAT3(1, 0, 0);
		verts[13].m_u = 0;
		verts[13].m_v = 0;

		verts[14].m_offset = offsets[7];
		verts[14].m_normal = XMFLOAT3(1, 0, 0);
		verts[14].m_u = 1;
		verts[14].m_v = 0;

		//Cube Back Face
		verts[15].m_offset = offsets[5];
		verts[15].m_normal = XMFLOAT3(0, 0, 1);
		verts[15].m_u = 0;
		verts[15].m_v = 1;

		verts[16].m_offset = offsets[4];
		verts[16].m_normal = XMFLOAT3(0, 0, 1);
		verts[16].m_u = 1;
		verts[16].m_v = 1;

		verts[17].m_offset = offsets[7];
		verts[17].m_normal = XMFLOAT3(0, 0, 1);
		verts[17].m_u = 0;
		verts[17].m_v = 0;

		verts[18].m_offset = offsets[6];
		verts[18].m_normal = XMFLOAT3(0, 0, 1);
		verts[18].m_u = 1;
		verts[18].m_v = 0;

		//Cube Left Face
		verts[19].m_offset = offsets[4];
		verts[19].m_normal = XMFLOAT3(-1, 0, 0);
		verts[19].m_u = 0;
		verts[19].m_v = 1;

		verts[20].m_offset = offsets[0];
		verts[20].m_normal = XMFLOAT3(-1, 0, 0);
		verts[20].m_u = 1;
		verts[20].m_v = 1;

		verts[21].m_offset = offsets[6];
		verts[21].m_normal = XMFLOAT3(-1, 0, 0);
		verts[21].m_u = 0;
		verts[21].m_v = 0;

		verts[22].m_offset = offsets[2];
		verts[22].m_normal = XMFLOAT3(-1, 0, 0);
		verts[22].m_u = 1;
		verts[22].m_v = 0;

		//Cube Top Face
		verts[23].m_offset = offsets[4];
		verts[23].m_normal = XMFLOAT3(0, 1, 0);
		verts[23].m_u = 0;
		verts[23].m_v = 1;

		verts[24].m_offset = offsets[5];
		verts[24].m_normal = XMFLOAT3(0, 1, 0);
		verts[24].m_u = 1;
		verts[24].m_v = 1;

		verts[25].m_offset = offsets[0];
		verts[25].m_normal = XMFLOAT3(0, 1, 0);
		verts[25].m_u = 0;
		verts[25].m_v = 0;

		verts[26].m_offset = offsets[1];
		verts[26].m_normal = XMFLOAT3(0, 1, 0);
		verts[26].m_u = 1;
		verts[26].m_v = 0;

		//Cube Top Face
		verts[27].m_offset = offsets[2];
		verts[27].m_normal = XMFLOAT3(0, -1, 0);
		verts[27].m_u = 0;
		verts[27].m_v = 1;

		verts[28].m_offset = offsets[3];
		verts[28].m_normal = XMFLOAT3(0, -1, 0);
		verts[28].m_u = 1;
		verts[28].m_v = 1;

		verts[29].m_offset = offsets[6];
		verts[29].m_normal = XMFLOAT3(0, -1, 0);
		verts[29].m_u = 0;
		verts[29].m_v = 0;

		verts[30].m_offset = offsets[7];
		verts[30].m_normal = XMFLOAT3(0, -1, 0);
		verts[30].m_u = 1;
		verts[30].m_v = 0;
#pragma endregion 
		HRESULT rs = Shader::CreateStructuredBuffer(hg::g_RenderDev.device, sizeof Vertex_Primitive, _SHAPE_PRIMITIVE_SIZE, m_meshDataBuffer.GetAddressOf(), verts);
		if (FAILED(rs))return rs;
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementWidth = _SHAPE_PRIMITIVE_SIZE;
	return hg::g_RenderDev.device->CreateShaderResourceView(m_meshDataBuffer.Get(), &srvDesc, m_meshData_SRV.GetAddressOf());
}
HRESULT Hourglass::GPUParticleSystem::CreateTextureAtlas()
{
	
	std::vector<Material*> materials;

	for (auto& o : m_particleTextures)
	{
		materials.push_back(hg::g_MaterialManager.GetMaterial(o.first.c_str()));
	}
	m_texturesBuffer.resize(3);
	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.ArraySize = 1;
	tDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	tDesc.Height = 512;
	tDesc.Width = 512 * (UINT)materials.size();
	tDesc.SampleDesc.Count = 1;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	tDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
	
	std::vector<ID3D11ShaderResourceView*> srvs;
	for (UINT textureType = 0; textureType < 3; textureType++)
	{
		HRESULT rs = hg::g_RenderDev.device->CreateTexture2D(&tDesc, nullptr, m_texturesBuffer[textureType].GetAddressOf());
		if (FAILED(rs))
		{
			return rs;
		}
		for (UINT i = 0; i < (UINT)materials.size(); i++)
		{
			srvs.push_back(materials[i]->m_Textures[textureType]->ShaderResourceView.Get());
			hg::g_RenderDev.deviceContext->CSSetShaderResources(0, (UINT)srvs.size(), srvs.data());
			hg::g_RenderDev.deviceContext->CSSetShader(nullptr, 0, 0);
			hg::g_RenderDev.deviceContext->CSSetSamplers(0, 1, hg::g_SamplerState[kSamplerState_Point].GetAddressOf());
			hg::g_RenderDev.deviceContext->CSSetConstantBuffers(0, 1, m_particleSystemCB.GetAddressOf());
		}
	}
	
	return E_NOTIMPL;
}
#if 0
uint32_t Hourglass::GPUParticleSystem::ReadAliveCount()
{
	uint32_t count;
	hg::g_RenderDev.deviceContext->CopyStructureCount(m_aliveCountBuffer.Get(), 0, m_sortList_UAV.Get());

	// Map the staging resource
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	hg::g_RenderDev.deviceContext->Map(m_aliveCountBuffer.Get(), 0, D3D11_MAP_READ, 0, &MappedResource);

	// Read the data
	count = *(int*)MappedResource.pData;

	hg::g_RenderDev.deviceContext->Unmap(m_aliveCountBuffer.Get(), 0);
	return count;
}

uint32_t Hourglass::GPUParticleSystem::ReadCount(ID3D11UnorderedAccessView * uav)
{
	uint32_t count;
	hg::g_RenderDev.deviceContext->CopyStructureCount(m_aliveCountBuffer.Get(), 0, uav);

	// Map the staging resource
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	hg::g_RenderDev.deviceContext->Map(m_aliveCountBuffer.Get(), 0, D3D11_MAP_READ, 0, &MappedResource);

	// Read the data
	count = *(int*)MappedResource.pData;

	hg::g_RenderDev.deviceContext->Unmap(m_aliveCountBuffer.Get(), 0);
	return count;
}
#endif
void Hourglass::GPUParticleSystem::InitDeadList()
{
	hg::g_RenderDev.deviceContext->CSSetShader(m_pCSInitDeadList.Get(), nullptr, 0);
	for (UINT i = 0; i < (UINT)m_particleShapes.size(); i++)
	{
		UINT initalCount[] = { 0 };
		hg::g_RenderDev.deviceContext->CSSetUnorderedAccessViews(0, 1, m_deadList_UAV[i].GetAddressOf(), initalCount);
		UINT dispatchNum = Math::Alignment(s_kMaxParticles, 256) / 256;
		hg::g_RenderDev.deviceContext->Dispatch(dispatchNum, 1, 1);
	}
	
	hg::g_RenderDev.deviceContext->CSSetShader(nullptr, nullptr, 0);
#if 0
	uint32_t *count;
	ComPtr<ID3D11Buffer> m_pDebugCounterBuffer = nullptr;
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = s_kMaxParticles * sizeof uint32_t;
	desc.StructureByteStride = sizeof uint32_t;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	HRESULT rs = hg::g_RenderDev.device->CreateBuffer(&desc, nullptr, m_pDebugCounterBuffer.GetAddressOf());
	if (FAILED(rs))
		return;


	// Copy the UAV counter to a staging resource
	hg::g_RenderDev.deviceContext->CopyResource(m_pDebugCounterBuffer.Get(), m_deadListBuffer.Get());

	// Map the staging resource
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	hg::g_RenderDev.deviceContext->Map(m_pDebugCounterBuffer.Get(), 0, D3D11_MAP_READ, 0, &MappedResource);

	// Read the data
	count = (uint32_t*)MappedResource.pData;

	hg::g_RenderDev.deviceContext->Unmap(m_pDebugCounterBuffer.Get(), 0);
	int i = 0;
#endif
}

HRESULT Hourglass::GPUParticleSystem::InitShader()
{

	ID3DBlob* shaderCode = nullptr;
	HRESULT rs = -1;
	LPCSTR profile = (hg::g_RenderDev.device->GetFeatureLevel() >= D3D_FEATURE_LEVEL_11_0) ? "cs_5_0" : nullptr;
	if (!profile)
	{
		PostQuitMessage(-1);
		return rs;
	}
	rs = Shader::CompileShader(L"HLSL/ParticleUpdate_CS.hlsl", hg::g_RenderDev.device, &shaderCode, profile);
	if (FAILED(rs))
	{
		if (shaderCode)
		{
			shaderCode->Release();
		}
		return rs;
	}


	rs = hg::g_RenderDev.device->CreateComputeShader(shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(), 0, &m_pCSParticleUpdate);
	if (FAILED(rs))
	{
		if (shaderCode)
		{
			shaderCode->Release();
		}
		return rs;
	}


	shaderCode->Release();
	rs = Shader::CompileShader(L"HLSL/ParticleEmit_CS.hlsl", hg::g_RenderDev.device, &shaderCode, profile);
	if (FAILED(rs))
	{
		if (shaderCode)
		{
			shaderCode->Release();
		}
		return rs;
	}


	rs = hg::g_RenderDev.device->CreateComputeShader(shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(), 0, &m_pCSParticleEmit);
	if (FAILED(rs))
	{
		if (shaderCode)
		{
			shaderCode->Release();
		}
		return rs;
	}


	shaderCode->Release();
	rs = Shader::CompileShader(L"HLSL/InitDeadList_CS.hlsl", hg::g_RenderDev.device, &shaderCode, profile);
	if (FAILED(rs))
	{
		if (shaderCode)
		{
			shaderCode->Release();
		}
		return rs;
	}

	rs = hg::g_RenderDev.device->CreateComputeShader(shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(), 0, &m_pCSInitDeadList);
	if (FAILED(rs))
	{
		if (shaderCode)
		{
			shaderCode->Release();
		}
		return rs;
	}
	shaderCode->Release();

	rs = Shader::CompileShader(L"HLSL/ParticleDraw_VS.hlsl", hg::g_RenderDev.device, &shaderCode, "vs_5_0");
	if (FAILED(rs))
	{
		if (shaderCode)
		{
			shaderCode->Release();
		}
		return rs;
	}

	rs = hg::g_RenderDev.device->CreateVertexShader(shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(), 0, &m_pVSDraw);
	if (FAILED(rs))
	{
		if (shaderCode)
		{
			shaderCode->Release();
		}
		return rs;
	}
	shaderCode->Release();

	rs = Shader::CompileShader(L"HLSL/ParticleDraw_PS.hlsl", hg::g_RenderDev.device, &shaderCode, "ps_5_0");
	if (FAILED(rs))
	{
		if (shaderCode)
		{
			shaderCode->Release();
		}
		return rs;
	}

	rs = hg::g_RenderDev.device->CreatePixelShader(shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(), 0, &m_pPSDraw);
	if (FAILED(rs))
	{
		if (shaderCode)
		{
			shaderCode->Release();
		}
		return rs;
	}
	shaderCode->Release();
	int counter = 0;
	for (auto itr = m_particleShapes.begin(); itr != m_particleShapes.end(); ++itr)
	{
		bool finish = true;
		switch (itr->first)
		{
		case EmitterParticleShape::Primitive:
		{
			rs = Shader::CompileShader(L"HLSL/ParticleDraw_GS.hlsl", hg::g_RenderDev.device, &shaderCode, "gs_5_0");
			if (FAILED(rs))
			{
				if (shaderCode)
				{
					shaderCode->Release();
				}
				return rs;
			}
		}
		break;
		case EmitterParticleShape::Mesh:
		{
			rs = Shader::CompileShader(L"HLSL/ParticleDraw_GS_Triangle.hlsl", hg::g_RenderDev.device, &shaderCode, "gs_5_0");
			if (FAILED(rs))
			{
				if (shaderCode)
				{
					shaderCode->Release();
				}
				return rs;
			}
		}
		break;
		default: 
			finish = false;
			break;
		}
		
		if (finish)
		{
			rs = hg::g_RenderDev.device->CreateGeometryShader(shaderCode->GetBufferPointer(), shaderCode->GetBufferSize(), 0, m_pGSDraw[counter].GetAddressOf());
			if (FAILED(rs))
			{
				if (shaderCode)
				{
					shaderCode->Release();
				}
				return rs;
			}
		}
		counter++;
		shaderCode->Release();
	}

	return rs;
}

HRESULT Hourglass::GPUParticleSystem::CreateBuffer()
{
	D3D11_BUFFER_DESC cbdesc = {};
	HRESULT rs;

	cbdesc.ByteWidth = 5 * sizeof UINT;
	cbdesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	cbdesc.Usage = D3D11_USAGE_DEFAULT;
	cbdesc.CPUAccessFlags = 0;
	cbdesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	for (int i = 0; i < (int)m_particleShapes.size(); i++)
	{
		rs = hg::g_RenderDev.device->CreateBuffer(&cbdesc, nullptr, m_drawArgs[i].GetAddressOf());
		if (FAILED(rs))
			return rs;
	}
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.NumElements = 5;
		uavDesc.Format = DXGI_FORMAT_R32_UINT;
		for (int i = 0; i < (int)m_particleShapes.size(); i++)
		{
			rs = hg::g_RenderDev.device->CreateUnorderedAccessView(m_drawArgs[i].Get(), &uavDesc, m_drawArgs_UAV[i].GetAddressOf());
			if (FAILED(rs))
				return rs;
			rs = Shader::CreateStructuredBuffer(hg::g_RenderDev.device, sizeof Particle, s_kMaxParticles, m_particleBuffer[i].GetAddressOf());
			if (FAILED(rs))
			{
				return rs;
			}
			rs = Shader::CreateStructuredBuffer(hg::g_RenderDev.device, sizeof ParticleAtt, s_kMaxParticles, m_particleAttBuffer[i].GetAddressOf());
			if (FAILED(rs))
			{
				return rs;
			}
		}
	}
	
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = s_kMaxParticles;
	for (UINT i = 0; i < m_particleShapes.size(); i++)
	{
		rs = hg::g_RenderDev.device->CreateUnorderedAccessView(m_particleBuffer[i].Get(), &uavDesc, m_particlePool_UAV[i].GetAddressOf());
		if (FAILED(rs))
		{
			return rs;
		}

		rs = hg::g_RenderDev.device->CreateUnorderedAccessView(m_particleAttBuffer[i].Get(), &uavDesc, m_particleAttPool_UAV[i].GetAddressOf());
		if (FAILED(rs))
		{
			return rs;
		}

		rs = Shader::CreateStructuredBuffer(hg::g_RenderDev.device, sizeof uint32_t, s_kMaxParticles, m_deadListBuffer[i].GetAddressOf(), nullptr);
		if (FAILED(rs))
		{
			return rs;
		}
	}
	for (int i = 0; i < (int)m_particleShapes.size(); i++)
	{
		rs = Shader::CreateStructuredBuffer(hg::g_RenderDev.device, sizeof LiveParticle, s_kMaxParticles, m_sortListBuffer[i].GetAddressOf());
		if (FAILED(rs))
		{
			return rs;
		}
	}

	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementWidth = s_kMaxParticles;

	for (int i = 0; i < (int)m_particleShapes.size(); i++)
	{
		rs = hg::g_RenderDev.device->CreateUnorderedAccessView(m_sortListBuffer[i].Get(), &uavDesc, m_sortList_UAV[i].GetAddressOf());
		if (FAILED(rs))
		{
			return rs;
		}

		rs = hg::g_RenderDev.device->CreateShaderResourceView(m_particleBuffer[i].Get(), &srvDesc, m_particlePool_SRV[i].GetAddressOf());
		if (FAILED(rs))
		{
			return rs;
		}
	}

	for (int i = 0; i < (int)m_particleShapes.size(); i++)
	{
		rs = hg::g_RenderDev.device->CreateShaderResourceView(m_sortListBuffer[i].Get(), &srvDesc, m_sortList_SRV[i].GetAddressOf());
		if (FAILED(rs))
		{
			return rs;
		}
	}

	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = 4 * sizeof UINT;

	for (int i = 0; i < (int)m_particleShapes.size(); i++)
	{
		rs = hg::g_RenderDev.device->CreateUnorderedAccessView(m_deadListBuffer[i].Get(), &uavDesc, m_deadList_UAV[i].GetAddressOf());
		if (FAILED(rs))
		{
			return rs;
		}
		rs = hg::g_RenderDev.device->CreateBuffer(&desc, nullptr, m_deadListCB[i].GetAddressOf());
		if (FAILED(rs))
		{
			return rs;
		}
		rs = hg::g_RenderDev.device->CreateBuffer(&desc, nullptr, m_sortListCB[i].GetAddressOf());
		if (FAILED(rs))
		{
			return rs;
		}
	}
	
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof EmitterData;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	rs = hg::g_RenderDev.device->CreateBuffer(&desc, nullptr, m_emitterCB.GetAddressOf());
	if (FAILED(rs))
	{
		return rs;
	}
	desc.ByteWidth = sizeof ParticleConstantPerFrame;
	rs = hg::g_RenderDev.device->CreateBuffer(&desc, nullptr, m_particleSystemCB.GetAddressOf());
	
	if (FAILED(rs))
	{
		return rs;
	}

	return CreateMeshBuffer();
}
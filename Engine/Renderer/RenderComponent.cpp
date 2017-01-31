/*!
 * \file RenderComponent.cpp
 *
 * \author Shiyang Ao
 * \date November 2016
 *
 * 
 */
#include "pch.h"

#include "RenderComponent.h"
#include "RenderQueue.h"
#include "RenderDevice.h"
#include "VertexDeclaration.h"
#include "RenderSystem.h"
#include "Core/Entity.h"
#include "Core/ComponentFactory.h"
#include "Texture.h"
#include "Material.h"
#include "DevTextRenderer.h"

bool g_UsePreDepthPass = true;

IMPLEMENT_COMPONENT_TYPEID(Hourglass::PrimitiveRenderer)

void Hourglass::RenderComponent::DrawCall_FetchLightingInfo(DrawCallData& drawCallData, const std::vector<Light*>& lightList) const
{
	int lightCount = 0;

	if (lightList.size() > 0)
	{
		DirectX::BoundingBox bound = GetRenderAabb().GetDirectXBoundingBox();

		for (int i = 0; i < lightList.size() && lightCount < MAX_POINT_LIGHT_PER_OBJECT; i++)
		{
			Light* light = lightList[i];

			switch (light->GetLightType())
			{
			case kLightType_Point:
				if (bound.Contains(light->GetBoundingSphere()))
					drawCallData.shape.lights[lightCount++] = light;
				break;

			case kLightType_Spotlight:
				if (bound.Contains(light->GetBoundingFrustum()))
					drawCallData.shape.lights[lightCount++] = light;
				break;

			default:
				break;
			}
		}
	}

	drawCallData.shape.lightCount = lightCount;

	//char buf[1024];
	//sprintf_s(buf, "L: %d", lightCount);
	//DevTextRenderer::DrawText_WorldSpace(buf, GetEntity()->GetPosition());
}

void Hourglass::RenderComponent::DrawCall_AssignMaterial(DrawCallData& drawCallData, const Material* material) const
{
	if (!material)
	{
		// Use default one if material is not specified
		drawCallData.material.m_SamplerState = kSamplerState_Point;
		drawCallData.material.m_ShaderResourceView[0] = g_MaterialManager.GetDefaultMaterial()->m_Textures[0]->ShaderResourceView.Get();

		return;
	}

	drawCallData.material.m_BlendMode = material->m_BlendMode;
	drawCallData.material.m_SamplerState = kSamplerState_Anisotropic;
	drawCallData.material.m_UseUniformColor = true;
	drawCallData.material.m_UniformColor *= material->m_UniformColor;

	for (int i = 0; i < 8; i++)
	{
		if (material->m_Textures[i])
			drawCallData.material.m_ShaderResourceView[i] = material->m_Textures[i]->ShaderResourceView.Get();
	}
}

void Hourglass::PrimitiveRenderer::LoadFromXML(tinyxml2::XMLElement * data)
{
	tinyxml2::XMLElement* meshXML = data->FirstChildElement( "ColoredBox" );

	XMFLOAT4 boxColors = { 0.0f, 0.0f, 0.0f, 1.0f };
	meshXML->QueryFloatAttribute( "r", &boxColors.x );
	meshXML->QueryFloatAttribute( "g", &boxColors.y );
	meshXML->QueryFloatAttribute( "b", &boxColors.z );

	CreateColoredBox( boxColors );
}

void Hourglass::PrimitiveRenderer::AddToRenderQueue(RenderQueue& renderQueue, const std::vector<Light*>& lightList)
{
	if (!m_Mesh)
		return;

	Entity* ent = GetEntity();

	DrawCallData drawCallData;
	ZeroMemory(&drawCallData, sizeof(DrawCallData));
	drawCallData.technique.m_VertexShader = g_RenderShaders[kRenderShader_Lighting].VertexShader.Get();
	drawCallData.technique.m_PixelShader = g_RenderShaders[kRenderShader_Lighting].PixelShader.Get();
	drawCallData.material.m_UseUniformColor = true;
	drawCallData.material.m_UniformColor = m_MeshColor;

	DrawCall_FetchLightingInfo(drawCallData, lightList);
	DrawCall_AssignMaterial(drawCallData, nullptr);

	drawCallData.shape.buffer = m_Mesh->GetRenderBuffer();
	drawCallData.shape.useMatrix = true;
	if (ent)
	{
		XMStoreFloat4x4(&drawCallData.shape.matrix, ent->GetTransform()->GetMatrix() * XMMatrixScaling(m_Scale, m_Scale, m_Scale));
	}
	else
	{
		XMStoreFloat4x4(&drawCallData.shape.matrix, XMMatrixScaling(m_Scale, m_Scale, m_Scale));
	}

	renderQueue.AddRenderShape(kRenderPass_Opaque, drawCallData);
}

Hourglass::Aabb Hourglass::PrimitiveRenderer::GetRenderAabb() const
{
	if (!m_Mesh)
		return Aabb();

	return m_Mesh->GetAabb().GetTransformedAabb(GetEntity()->GetTransform()->GetMatrix());
}

void Hourglass::PrimitiveRenderer::CreateColoredBox(const Color& color, float scale/*=1.0f*/)
{
	m_Mesh = g_MeshManager.GetMesh("Assets/Mesh/Cube1x1x1.hmdl");
	m_MeshColor = color;
	m_Scale = scale;
}

Hourglass::IComponent* Hourglass::PrimitiveRenderer::MakeCopyDerived() const
{
	PrimitiveRenderer* copy = (PrimitiveRenderer*)IComponent::Create( SID(PrimitiveRenderer) );
	copy->CreateColoredBox( Vector4( rand() % 100 / 101.0f, rand() % 100 / 101.0f, rand() % 100 / 101.0f, 1.0f ), m_Scale );

	return copy;
}

uint32_t Hourglass::MeshRenderer::s_TypeID = ComponentFactory::GetSystemComponentID();

void Hourglass::MeshRenderer::LoadFromXML( tinyxml2::XMLElement * data )
{
	data->QueryFloatAttribute( "color_r", &m_MeshColor.x );
	data->QueryFloatAttribute( "color_g", &m_MeshColor.y );
	data->QueryFloatAttribute( "color_b", &m_MeshColor.z );
	data->QueryFloatAttribute( "color_a", &m_MeshColor.w );

	tinyxml2::XMLElement* meshXML = data->FirstChildElement( "Mesh" );

	const char* meshFile = meshXML->Attribute( "file");

	m_Mesh = g_MeshManager.GetMesh( meshFile );

	m_Material = nullptr;
	tinyxml2::XMLElement* matXML = data->FirstChildElement("Material");
	if (matXML)
	{
		const char* matFile = matXML->Attribute("file");
		m_Material = g_MaterialManager.GetMaterial(matFile);
	}

	m_RenderType = kRenderType_Default;
	m_RenderFunc = MeshRenderer::DefaultRenderFunc;

	const char* renderFuncName = data->Attribute( "RenderType" );
	if (renderFuncName)		
	{
		if (strcmp( renderFuncName, "PBR" ) == 0)
		{
			m_RenderType = kRenderType_Pbr;
			m_RenderFunc = MeshRenderer::PbrRenderFunc;
		}
		else if (strcmp( renderFuncName, "BARRIER" ) == 0)
		{
			// TODO: incorporate unlit
			m_RenderType = kRenderType_Barrier;
			m_RenderFunc = MeshRenderer::BarrierFunc;
		}
		else if (strcmp( renderFuncName, "UNLIT" ) == 0)
		{
			// TODO: incorporate unlit
			m_RenderType = kRenderType_Unlit;
			m_RenderFunc = MeshRenderer::PbrRenderFunc;
		}
		else if (strcmp( renderFuncName, "PBR_FLOW") == 0)
		{
			m_RenderType = kRenderType_PbrFlow;
			m_RenderFunc = MeshRenderer::PbrFlowFunc;
		}
	}
}

void Hourglass::MeshRenderer::Init()
{
	m_MeshColor = Color(1.0f, 1.0f, 1.0f);
}

void Hourglass::MeshRenderer::SetMesh(Mesh* mesh)
{
	m_Mesh = mesh;
}

void Hourglass::MeshRenderer::SetMaterial(Material* mat)
{
	m_Material = mat;
}

void Hourglass::MeshRenderer::AddToRenderQueue(RenderQueue& renderQueue, const std::vector<Light*>& lightList)
{
	if (!m_Mesh)
		return;	

	Entity* ent = GetEntity();
	
	DrawCallData drawCallData;
	ZeroMemory( &drawCallData, sizeof( DrawCallData ) );

	m_RenderFunc( this, &drawCallData );
	
	drawCallData.shape.transform = ent ? ent->GetTransform() : &Transform::Identity();

	// Set up light info 
	if (m_RenderType == kRenderType_Unlit)
	{
		// No lighting for unlit material
		static const std::vector<Light*> emptyLightList;
		DrawCall_FetchLightingInfo(drawCallData, emptyLightList);
	}
	else
	{
		DrawCall_FetchLightingInfo(drawCallData, lightList);
	}

	DrawCall_AssignMaterial(drawCallData, m_Material);

	RenderPassType pass = (!m_Material || m_Material->m_BlendMode == kBlend_Opaque) ? kRenderPass_Opaque : kRenderPass_Transparent;

	if (g_UsePreDepthPass && pass == kRenderPass_Opaque)
	{
		for (int i = 0; i < m_Mesh->Submeshes.size(); i++)
		{
			drawCallData.shape.submesh = &m_Mesh->Submeshes[i];
			renderQueue.AddDepthPassShape(drawCallData.shape);
		}
	}

	for (int i = 0; i < m_Mesh->Submeshes.size(); i++)
	{
		drawCallData.shape.submesh = &m_Mesh->Submeshes[i];
		renderQueue.AddRenderShape(pass, drawCallData);
	}
}

/*static*/ void Hourglass::MeshRenderer::PbrRenderFunc( MeshRenderer* renderer, DrawCallData* drawCallData )
{
	drawCallData->technique.m_VertexShader = g_RenderShaders[kRenderShader_Lighting].VertexShader.Get();
	drawCallData->technique.m_PixelShader = g_RenderShaders[kRenderShader_PbrLighting].PixelShader.Get();
	drawCallData->material.m_UseUniformColor = true;
	drawCallData->material.m_UniformColor = renderer->m_MeshColor;
	drawCallData->shape.buffer = renderer->m_Mesh->GetRenderBuffer();
}

/*static*/ void Hourglass::MeshRenderer::BarrierFunc( MeshRenderer* renderer, DrawCallData* drawCallData )
{
	drawCallData->technique.m_VertexShader = g_RenderShaders[kRenderShader_Lighting].VertexShader.Get();
	drawCallData->technique.m_PixelShader = g_RenderShaders[kRenderShader_Barrier].PixelShader.Get();
	drawCallData->material.m_UseUniformColor = true;
	drawCallData->material.m_UniformColor = renderer->m_MeshColor;
	drawCallData->shape.buffer = renderer->m_Mesh->GetRenderBuffer();
}

/*static*/ void Hourglass::MeshRenderer::DefaultRenderFunc( MeshRenderer* renderer, DrawCallData* drawCallData )
{	
	drawCallData->technique.m_VertexShader = g_RenderShaders[kRenderShader_Lighting].VertexShader.Get();
	drawCallData->technique.m_PixelShader = g_RenderShaders[kRenderShader_Lighting].PixelShader.Get();
	drawCallData->material.m_UseUniformColor = true;
	drawCallData->material.m_UniformColor = renderer->m_MeshColor;
	drawCallData->shape.buffer = renderer->m_Mesh->GetRenderBuffer();
}

void Hourglass::MeshRenderer::PbrFlowFunc(MeshRenderer * renderer, DrawCallData * drawCallData)
{
	drawCallData->technique.m_VertexShader = g_RenderShaders[kRenderShader_Lighting].VertexShader.Get();
	drawCallData->technique.m_PixelShader = g_RenderShaders[kRenderShader_PbrFlow].PixelShader.Get();
	drawCallData->material.m_UseUniformColor = true;
	drawCallData->material.m_UniformColor = renderer->m_MeshColor;
	drawCallData->shape.buffer = renderer->m_Mesh->GetRenderBuffer();
}

Hourglass::Aabb Hourglass::MeshRenderer::GetRenderAabb() const
{
	if (!m_Mesh)
		return Aabb();

	return m_Mesh->GetAabb().GetTransformedAabb(GetEntity()->GetTransform()->GetMatrix());
}

Hourglass::IComponent* Hourglass::MeshRenderer::MakeCopyDerived() const
{
	Hourglass::MeshRenderer* cpy = (Hourglass::MeshRenderer*)IComponent::Create(SID(MeshRenderer));

	cpy->m_MeshColor = m_MeshColor;
	cpy->m_Mesh = m_Mesh;
	cpy->m_Material = m_Material;
	cpy->m_RenderFunc = m_RenderFunc;
	cpy->m_RenderType = m_RenderType;

	return cpy;
}

uint32_t Hourglass::SkinnedMeshRenderer::s_TypeID = ComponentFactory::GetSystemComponentID();

void Hourglass::SkinnedMeshRenderer::AddToRenderQueue( RenderQueue& renderQueue, const std::vector<Light*>& lightList )
{
	if (!m_Mesh)
		return;	

	Entity* ent = GetEntity();

	DrawCallData drawCallData;
	ZeroMemory( &drawCallData, sizeof( DrawCallData ) );
	drawCallData.technique.m_VertexShader = g_RenderShaders[m_lightWeight? kRenderShader_LightingSkinned_LightWeight : kRenderShader_LightingSkinned].VertexShader.Get();
	drawCallData.technique.m_PixelShader = g_RenderShaders[kRenderShader_PbrLighting].PixelShader.Get();
	drawCallData.material.m_UseUniformColor = true;
	drawCallData.material.m_UniformColor = m_MeshColor;
	drawCallData.shape.buffer = m_Mesh->GetRenderBuffer();
	drawCallData.shape.transform = ent ? ent->GetTransform() : &Transform::Identity();
	drawCallData.shape.useAnimation = true;
	drawCallData.shape.animationId = m_AnimationId;

	DrawCall_FetchLightingInfo(drawCallData, lightList);
	DrawCall_AssignMaterial(drawCallData, m_Material);

	for (int i = 0; i < m_Mesh->Submeshes.size(); i++)
	{
		drawCallData.shape.submesh = &m_Mesh->Submeshes[i];
		renderQueue.AddRenderShape( kRenderPass_OpaqueSkinned, drawCallData );
	}
}

void Hourglass::SkinnedMeshRenderer::LoadFromXML(tinyxml2::XMLElement * data)
{
	m_lightWeight = false;
	data->QueryBoolAttribute("isLightWeight", &m_lightWeight);
	MeshRenderer::LoadFromXML(data);
}

Hourglass::IComponent* Hourglass::SkinnedMeshRenderer::MakeCopyDerived() const
{
	Hourglass::SkinnedMeshRenderer* cpy = (Hourglass::SkinnedMeshRenderer*)IComponent::Create( SID( SkinnedMeshRenderer ) );

	cpy->m_MeshColor = m_MeshColor;
	cpy->m_Mesh = m_Mesh;
	cpy->m_Material = m_Material;
	cpy->m_RenderFunc = m_RenderFunc;
	cpy->m_lightWeight = m_lightWeight;

	return cpy;
}

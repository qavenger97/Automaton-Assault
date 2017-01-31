/*!
 * \file Light.cpp
 *
 * \author Shiyang Ao
 * \date November 2016
 *
 * 
 */

#include "pch.h"

#include "Light.h"
#include "Core/ComponentFactory.h"
#include "Math/Aabb.h"
#include "Component/IComponent.h"
#include "Core/Entity.h"
#include "DebugRenderer.h"

IMPLEMENT_COMPONENT_TYPEID(Hourglass::Light)

void Hourglass::Light::LoadFromXML(tinyxml2::XMLElement* data)
{
	tinyxml2::XMLElement* lightColorXML = data->FirstChildElement("Color");
	
	// Load light color
	m_Color = Color(1.0f, 1.0f, 1.0f);
	if (lightColorXML)
	{
		lightColorXML->QueryFloatAttribute("r", &m_Color.x);
		lightColorXML->QueryFloatAttribute("g", &m_Color.y);
		lightColorXML->QueryFloatAttribute("b", &m_Color.z);
		lightColorXML->QueryFloatAttribute("a", &m_Color.w);
	}

	// Load light type
	m_LightType = kLightType_Point;
	const char* type = data->Attribute("Type");
	if (type)
	{
		if (strcmp(type, "Point") == 0)
		{
			m_LightType = kLightType_Point;
		}
		else if (strcmp(type, "Spot") == 0)
		{
			m_LightType = kLightType_Spotlight;
		}
	}

	// Load light radius
	m_Radius = 1.0f;
	data->QueryFloatAttribute("Range", &m_Radius);

	// Load light intensity
	m_Intensity = 1.0f;
	data->QueryFloatAttribute("Intensity", &m_Intensity);

	m_InnerConeRatio = 0.8f;
	data->QueryFloatAttribute("InnerRatio", &m_InnerConeRatio);

	m_OuterConeRatio = 0.75f;
	data->QueryFloatAttribute("OuterRatio", &m_OuterConeRatio);
}

Hourglass::IComponent* Hourglass::Light::MakeCopyDerived() const
{
	Light* copy = (Light*)IComponent::Create(SID(Light));
	copy->m_LightType = m_LightType;
	copy->m_Color = m_Color;
	copy->m_Radius = m_Radius;
	copy->m_Intensity = m_Intensity;

	return copy;
}

void Hourglass::Light::Init(LightType type, Color color, float radius, float intensity /*= 1.0f*/)
{
	m_LightType = type;
	m_Color = color;
	m_Radius = radius;
	m_Intensity = intensity;
}

void Hourglass::Light::DrawDebugLightInfo()
{
	if (m_LightType == kLightType_Point)
	{
		Vector3 center = GetEntity()->GetPosition();
		DebugRenderer::DrawSphere(center, m_Radius, Color(1.0f, 1.0f, 0.0f));
	}
	else if (m_LightType == kLightType_Spotlight)
	{
		DebugRenderer::DrawFrustum(GetBoundingFrustum(), Color(1, 1, 0));
	}
}

Vector3 Hourglass::Light::GetPosition() const
{
	return GetEntity()->GetPosition();
}

DirectX::SimpleMath::Vector3 Hourglass::Light::GetDirection() const
{
	return GetEntity()->GetTransform()->Forward();
}

void Hourglass::Light::CacheBoundingShapes()
{
	switch (m_LightType)
	{
	case kLightType_Point:
		m_CachedBoundingSphere = BoundingSphere(GetPosition(), m_Radius);
		break;

	case kLightType_Spotlight:
		Vector3 pos = GetPosition();
		Vector3 dir = GetDirection();

		m_CachedBoundingFrustum.Origin = pos;
		XMStoreFloat4(&m_CachedBoundingFrustum.Orientation, Transform::BuildLookAtRotation(pos, pos + dir));
		m_CachedBoundingFrustum.Near = 0.1f;
		m_CachedBoundingFrustum.Far = m_Radius;

		float slope = 1.0f / tanf(XM_PIDIV2 - acosf(m_OuterConeRatio));
		m_CachedBoundingFrustum.TopSlope = m_CachedBoundingFrustum.RightSlope = slope;
		m_CachedBoundingFrustum.BottomSlope = m_CachedBoundingFrustum.LeftSlope = -slope;
		break;
	}
}

const DirectX::BoundingSphere& Hourglass::Light::GetBoundingSphere() const
{
	return m_CachedBoundingSphere;
}

const DirectX::BoundingFrustum& Hourglass::Light::GetBoundingFrustum() const
{
	return m_CachedBoundingFrustum;
}

bool Hourglass::Light::TestContainedByFrustum(const BoundingFrustum& frustum) const
{
	switch (m_LightType)
	{
	case kLightType_Point:
		return GetBoundingSphere().Contains(frustum) != DISJOINT;

	case kLightType_Spotlight:
		return GetBoundingFrustum().Contains(frustum) != DISJOINT;
	}

	// Shouldn't be here
	assert(0);
	return false;
}

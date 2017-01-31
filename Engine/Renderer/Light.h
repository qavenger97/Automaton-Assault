/*!
 * \file Light.h
 *
 * \author Shiyang Ao
 * \date November 2016
 *
 * 
 */
#pragma once

#include "Component\IComponent.h"

namespace Hourglass
{
	enum LightType
	{
		kLightType_Point,
		kLightType_Spotlight,

		kLightTypeCount,
	};

	class Light : public IComponent
	{
		DECLARE_COMPONENT_TYPEID
	public:
		//////////////////////////////////////////////////////////////////////////
		// Override IComponent functions
		//////////////////////////////////////////////////////////////////////////
		void LoadFromXML(tinyxml2::XMLElement* data);
		virtual IComponent* MakeCopyDerived() const;
		void Init() {}

		//////////////////////////////////////////////////////////////////////////
		// Light functions
		//////////////////////////////////////////////////////////////////////////
		void Init(LightType type, Color color, float radius, float intensity = 1.0f);

		void		DrawDebugLightInfo();

		void		SetIntensity(float intensity)	{ m_Intensity = intensity; }
		void		SetRadius(float radius)			{ m_Radius = radius; }
		void		SetColor(Color color)			{ m_Color = color; }

		Vector3		GetPosition() const;
		Vector3		GetDirection() const;
		LightType	GetLightType() const			{ return m_LightType; }
		float		GetRadius() const				{ return m_Radius; }
		float		GetIntensity() const			{ return m_Intensity; }
		Color		GetColor() const				{ return m_Color; }
		float		GetInnerConeRatio() const		{ return m_InnerConeRatio; }
		float		GetOuterConeRatio() const		{ return m_OuterConeRatio; }

		// Update bounding shape for this frame
		void		CacheBoundingShapes();

		const BoundingSphere&	GetBoundingSphere() const;
		const BoundingFrustum&	GetBoundingFrustum() const;

		bool		TestContainedByFrustum(const BoundingFrustum& frustum) const;

	private:
		LightType		m_LightType;
		float			m_Radius;
		float			m_Intensity;
		Color			m_Color;
		float			m_InnerConeRatio;
		float			m_OuterConeRatio;

		BoundingSphere	m_CachedBoundingSphere;
		BoundingFrustum m_CachedBoundingFrustum;
	};
}
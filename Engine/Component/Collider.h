/*!
 * \file Collider.h
 *
 * \author Shiyang Ao
 * \date October 2016
 *
 * Base collider class for all physics system components
 */
#pragma once

#include "IComponent.h"
#include "Math\Aabb.h"
#include "Collision\Cylinder.h"
#include "Math\Ray.h"

namespace Hourglass
{
#define COLLISION_GROUP_DEFAULT			0
#define COLLISION_GROUP_MASK_DEFAULT	(1 << 0)
#define COLLISION_GROUP_ALL				0xFFFF

	struct AabbData
	{
		float pMin[3];
		float pMax[3];
	};

	struct CylinderData
	{
		float Center[3];
		float Radius;
		float Height;
	};

	class Collider : public IComponent
	{
	public:
		void LoadFromXML(tinyxml2::XMLElement* data);

		// Set the collision group id for this collider (id: 0 ~ 15)
		void SetCollisionGroup(uint8_t groupId)	{ m_CollisionGroupId = groupId; }
		uint8_t GetCollisionGroup() const		{ return m_CollisionGroupId; }

		void SetAabb(const Aabb& aabb);

		// Get a world bounding box encapsules shape of collider
		const Aabb& GetColliderWorldAabb() const;

		int GetCollisionShapeType() const { return m_CollisionShapeType; }

		// Get aabb collider shape in local space
		Aabb GetLocalAabbShape() const;

		// Get cylinder collider shape in local space
		Cylinder GetLocalCylinderShape() const;

		// Get cylinder collider shape in world space
		Cylinder GetWorldCylinderShape() const;

		bool TestIntersectionWithRay(const Ray& ray, float* t = nullptr, Vector3* outNormal = nullptr) const;

		virtual bool IsStaticCollider() const { return false; }

		bool HasColliderRotation() const { return m_IsRotated; }

		bool IsTrigger() const { return m_IsTrigger; }
	protected:

		// Calculate world aabb for static collider
		//    Note: Unlike dynamic colliders, static ones will not get updated every frame.
		//          So update world aabb every time we change bounding box.
		void CalculateWorldStaticAabb();

	protected:
		uint8_t m_CollisionGroupId;

		int m_CollisionShapeType;
		union
		{
			AabbData		m_Aabb;
			CylinderData	m_Cylinder;
		};

		Aabb	m_WorldAabb;

		bool	m_IsRotated;				// Indicates this collider contains rotation
		bool	m_IsTrigger;
	};
}

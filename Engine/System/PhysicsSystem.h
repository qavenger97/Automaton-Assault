#pragma once

#include "Math\Ray.h"
#include "..\Component\DynamicCollider.h"
#include "..\Component\StaticCollider.h"
#include "..\Component\Motor.h"
#include "..\System\ISystem.h"
#include "..\Core\Message.h"

namespace Hourglass
{
	class ISpatial;

	class CollisionMessage : public Message
	{
	public:
		int GetType() const { return MessageType::kCollision; }

		Entity* otherEntity;
	};

	class TriggerEnterMessage : public Message
	{
	public:
		int GetType() const { return MessageType::kOnTriggerEnter; }

		Entity* sourceEntity;
	};

	class TriggerExitMessage : public Message
	{
	public:
		int GetType() const { return MessageType::kOnTriggerExit; }

		Entity* sourceEntity;
	};

	class PhysicsSystem : public ISystem
	{
	public:

		void Init();

		void BuildQuadtree();

		void Shutdown();

		void Update();

		void SetCollisionBetweenGroups(uint8_t group1, uint8_t group2, bool canCollide);
		bool GetCollisionBetweenGroups(uint8_t group1, uint8_t group2) const;

		// Cast a ray and return closest hit point of all colliders
		bool RayCast(const Ray& ray, Entity** outEntity = nullptr, Vector3* outHit = nullptr, Vector3* outNormal = nullptr, uint16_t collisionGroupMasks = COLLISION_GROUP_ALL);
		bool LinePlaneTest(const FXMVECTOR& origin, const FXMVECTOR& direction, const FXMVECTOR& normal, const float offset, float *t = nullptr);
		// Get entities overlapped with given box
		std::vector<Entity*> TestBoxOverlapping(const Aabb& aabb, uint16_t collisionGroupMasks = COLLISION_GROUP_ALL);
	private:

		DECLARE_COMPONENT_POOL( DynamicCollider, 1024 );
		DECLARE_COMPONENT_POOL( StaticCollider,	1024 );
		DECLARE_COMPONENT_POOL( Motor, 1024 );

		bool		m_GroupCollisionState[256];		// Collision filters indicating whether objects in two collision group will collide
		ISpatial*	m_StaticQuadtree;
	};

	extern PhysicsSystem g_Physics;
}
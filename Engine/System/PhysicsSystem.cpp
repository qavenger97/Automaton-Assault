#include "pch.h"

#include "PhysicsSystem.h"
#include "../Component/DynamicCollider.h"
#include "../Component/StaticCollider.h"
#include "../Component/Motor.h"
#include "../Core/Entity.h"
#include "../Core/ComponentPool.h"
#include "Renderer/DebugRenderer.h"
#include "Core/Profiler.h"
#include "Collision/Collision.h"
#include "Core/Quadtree.h"
#include "Core/DevMenu.h"

namespace Hourglass
{
	PhysicsSystem g_Physics;

	static bool s_bUseQuadtree = true;

	Aabb GetStaticColliderAabb(const StaticCollider* col)
	{
		return col->GetColliderWorldAabb();
	}

	bool TestRayColliderIntersection(const StaticCollider* col, const Ray& ray, float *t, Vector3* outNormal, uint16_t collisionGroupMasks)
	{
		if (!col->IsAlive())
			return false;

		if (!col->IsEnabled())
			return false;

		int groupId = (1 << col->GetCollisionGroup());
		if ((groupId & collisionGroupMasks) == 0)
			return false;

		return col->TestIntersectionWithRay(ray, t, outNormal);
	}

	typedef Quadtree<
		StaticCollider,
		GetStaticColliderAabb,
		TestRayColliderIntersection
	> StaticQuadtree;


	void PhysicsSystem::Init()
	{
		INIT_COMPONENT_POOL( DynamicCollider );
		INIT_COMPONENT_POOL( StaticCollider );
		INIT_COMPONENT_POOL( Motor );

		ZeroMemory(m_GroupCollisionState, sizeof(m_GroupCollisionState));

		// Set collision with everything
		for (uint8_t i = 0; i < 16; i++)
		{
			for (uint8_t j = i; j < 16; j++)
			{
				SetCollisionBetweenGroups(i, j, true);
			}
		}

		g_DevMenu.AddMenuVarBool("Use quadtree: ", &s_bUseQuadtree);

		m_StaticQuadtree = nullptr;
	}

	void PhysicsSystem::BuildQuadtree()
	{
		std::vector<StaticCollider*> staticColliders;
		for (int i = 0; i < s_kMaxStaticColliders; i++)
		{
			StaticCollider& collider = m_StaticColliders[i];

			if (!collider.IsAlive())
				continue;

			if (!collider.IsEnabled())
				continue;

			staticColliders.push_back(&collider);
		}

		// Release current quadtree
		if (m_StaticQuadtree)
			delete m_StaticQuadtree;

		// Build quadtree for static colliders
		m_StaticQuadtree = new StaticQuadtree();
		static_cast<StaticQuadtree*>(m_StaticQuadtree)->Init(staticColliders);
	}
	
	void PhysicsSystem::Shutdown()
	{
		delete m_StaticQuadtree;
		m_StaticQuadtree = nullptr;
	}

	void PhysicsSystem::Update()
	{
		m_DynamicColliderPool.UpdatePooled();
		m_StaticColliderPool.UpdatePooled();
		m_MotorPool.UpdatePooled();

		//m_StaticQuadtree->DebugRender();

		typedef std::pair<Entity*, Entity*> CollisionPair;
		std::set<CollisionPair> collisionPairs;

		// Handle dynamic collider collisions
		for (uint32_t i = 0; i < s_kMaxDynamicColliders; ++i)
		{
			DynamicCollider& dynamicCollider = m_DynamicColliders[i];

			if (!dynamicCollider.IsAlive() || !dynamicCollider.IsEnabled())
				continue;

			//if (!dynamicCollider.IsEnabled())
			//	continue;

			// Ignore trigger movements
			if (dynamicCollider.IsTrigger())
			{
				dynamicCollider.DebugDrawCollisionShape();
				continue;
			}

			// If collider didn't move since last frame, ignore it
			// TODO: Unless we want to push a dynamic collider
			if (dynamicCollider.IsStationary())
			{
				dynamicCollider.DebugDrawCollisionShape();
				continue;
			}

			Aabb aabb = dynamicCollider.GetColliderWorldAabb();
			Vector3 actualMoveVec = dynamicCollider.GetMoveVector();

			std::vector<StaticCollider*> collidingList;
			Aabb sweptAabb = dynamicCollider.GetSweptAabb();

			// Three colliding objects on three axises
			Entity* colEntities[3] = { nullptr };
			Vector3 minVector = dynamicCollider.GetMoveVector();

			for (uint32_t j = 0; j < s_kMaxStaticColliders; ++j)
			{
				StaticCollider& staticCollider = m_StaticColliders[j];

				if (!staticCollider.IsAlive() || !staticCollider.IsEnabled())
					continue;
				
			/*	if (!staticCollider.IsEnabled())
					continue;*/

				if (!GetCollisionBetweenGroups(dynamicCollider.GetCollisionGroup(), staticCollider.GetCollisionGroup()))
					continue;

				if (sweptAabb.Intersects(staticCollider.GetColliderWorldAabb()))
				{
					if (dynamicCollider.TestDynamicCollision(actualMoveVec, &staticCollider, &actualMoveVec))
					{
						if (fabsf(actualMoveVec.x) < fabsf(minVector.x))
						{
							minVector.x = actualMoveVec.x;
							colEntities[0] = staticCollider.GetEntity();
						}
						else if (fabsf(actualMoveVec.y) < fabsf(minVector.y))
						{
							minVector.y = actualMoveVec.y;
							colEntities[1] = staticCollider.GetEntity();
						}
						else if (fabsf(actualMoveVec.z) < fabsf(minVector.z))
						{
							minVector.z = actualMoveVec.z;
							colEntities[2] = staticCollider.GetEntity();
						}
					}
					collidingList.push_back(&staticCollider);
				}
			}

			// Test a second time in a reversed order to avoid stuck on neighbor colliders
			{
				Entity* newColEntities[3] = { nullptr };
				Vector3 newMinVector = dynamicCollider.GetMoveVector();

				Vector3 newMoveVec = dynamicCollider.GetMoveVector();
				for (int32_t j = (int32_t)collidingList.size() - 1; j >= 0; j--)
				{
					if (dynamicCollider.TestDynamicCollision(newMoveVec, collidingList[j], &newMoveVec))
					{
						if (fabsf(actualMoveVec.x) < fabsf(newMinVector.x))
						{
							newMinVector.x = actualMoveVec.x;
							newColEntities[0] = collidingList[j]->GetEntity();
						}
						else if (fabsf(actualMoveVec.y) < fabsf(newMinVector.y))
						{
							newMinVector.y = actualMoveVec.y;
							newColEntities[1] = collidingList[j]->GetEntity();
						}
						else if (fabsf(actualMoveVec.z) < fabsf(newMinVector.z))
						{
							newMinVector.z = actualMoveVec.z;
							newColEntities[2] = collidingList[j]->GetEntity();
						}
					}
				}

				float newMoveLengthSq;
				XMStoreFloat(&newMoveLengthSq, XMVector3LengthSq(XMLoadFloat3(&newMoveVec)));
				float actualMoveLengthSq;
				XMStoreFloat(&actualMoveLengthSq, XMVector3LengthSq(XMLoadFloat3(&actualMoveVec)));

				if (newMoveLengthSq > actualMoveLengthSq)
				{
					actualMoveVec = newMoveVec;
					memcpy(colEntities, newColEntities, sizeof(colEntities));
					minVector = newMinVector;
				}
			}

			// Test collision with dynamic colliders
			// TODO: This checking may cause moving priority issue

			sweptAabb = aabb.GetSweptAabb(actualMoveVec);

			for (uint32_t j = 0; j < s_kMaxDynamicColliders; ++j)
			{
				DynamicCollider& otherDynamicCollider = m_DynamicColliders[j];

				if (&dynamicCollider == &otherDynamicCollider)
					continue;

				if (!otherDynamicCollider.IsAlive())
					continue;

				if (!otherDynamicCollider.IsEnabled())
					continue;

				if (otherDynamicCollider.IsTrigger())
					continue;

				if (!GetCollisionBetweenGroups(dynamicCollider.GetCollisionGroup(), otherDynamicCollider.GetCollisionGroup()))
					continue;

				if (sweptAabb.Intersects(otherDynamicCollider.GetColliderWorldAabb()))
				{
					if (dynamicCollider.TestDynamicCollision(actualMoveVec, &otherDynamicCollider, &actualMoveVec))
					{
						if (fabsf(actualMoveVec.x) < fabsf(minVector.x))
						{
							minVector.x = actualMoveVec.x;
							colEntities[0] = otherDynamicCollider.GetEntity();
						}
						else if (fabsf(actualMoveVec.y) < fabsf(minVector.y))
						{
							minVector.y = actualMoveVec.y;
							colEntities[1] = otherDynamicCollider.GetEntity();
						}
						else if (fabsf(actualMoveVec.z) < fabsf(minVector.z))
						{
							minVector.z = actualMoveVec.z;
							colEntities[2] = otherDynamicCollider.GetEntity();
						}
					}
				}
			}

			Cylinder cylinder;

			if (dynamicCollider.GetCollisionShapeType() == kCylinder)
			{
				cylinder = dynamicCollider.GetWorldCylinderShape();
			}

			dynamicCollider.GetEntity()->GetTransform()->Translate(actualMoveVec);
			dynamicCollider.ClearMoveVector();

			// Update aabb for debug rendering
			dynamicCollider.Update();
			dynamicCollider.DebugDrawCollisionShape();

			// Handle trigger events
			// Note: only cylinder shape is supported at this point
			if (dynamicCollider.GetCollisionShapeType() == kCylinder)
			{
				Cylinder newCylinder = dynamicCollider.GetWorldCylinderShape();
				bool hasCollision = false;

				for (uint32_t j = 0; j < s_kMaxDynamicColliders; ++j)
				{
					DynamicCollider& trigger = m_DynamicColliders[j];

					if (&dynamicCollider == &trigger)
						continue;

					if (!trigger.IsAlive())
						continue;

					if (!trigger.IsEnabled())
						continue;

					if (!trigger.IsTrigger())
						continue;

					if (!GetCollisionBetweenGroups(dynamicCollider.GetCollisionGroup(), trigger.GetCollisionGroup()))
						continue;

					Aabb triggerLocalBound = trigger.GetLocalAabbShape();
					Matrix triggerTransform = trigger.GetEntity()->GetTransform()->GetMatrix();

					//DebugRenderer::DrawAabb(triggerLocalBound, triggerTransform, Color(1.0f, 1.0f, 0.0f));

					bool triggeredLastFrame = false;
					bool triggeredThisFrame = false;

					if (cylinder.TestInteresctionWithTransformedAabb(triggerLocalBound, triggerTransform))
					{
						triggeredLastFrame = true;
					}

					if (newCylinder.TestInteresctionWithTransformedAabb(triggerLocalBound, triggerTransform))
					{
						triggeredThisFrame = true;
					}

					// Handle trigger enter/exit event based on entity to trigger intersection status
					if (!triggeredLastFrame && triggeredThisFrame)
					{
						TriggerEnterMessage msg;
						msg.sourceEntity = dynamicCollider.GetEntity();
						trigger.GetEntity()->SendMsg(&msg);
					}
					else if (triggeredLastFrame && !triggeredThisFrame)
					{
						TriggerExitMessage msg;
						msg.sourceEntity = dynamicCollider.GetEntity();
						trigger.GetEntity()->SendMsg(&msg);
					}
				}

				//if (hasCollision)
				//	DebugRenderer::DrawCylinder(newCylinder, Color(1.0f, 0.0f, 0.0f));
				//else
				//	DebugRenderer::DrawCylinder(newCylinder, Color(1.0f, 1.0f, 0.0f));
			}

			for (int p = 0; p < 3; p++)
			{
				if (colEntities[p])
				{
					Entity* entThis = dynamicCollider.GetEntity();
					Entity* entOther = colEntities[p];
					CollisionPair colPair = entThis < entOther ? std::make_pair(entThis, entOther) : std::make_pair(entOther, entThis);
					collisionPairs.insert(colPair);
				}
			}
		}

		// Notify all colliding entities about collision event
		for (std::set<CollisionPair>::iterator iter = collisionPairs.begin(); iter != collisionPairs.end(); iter++)
		{
			CollisionMessage col_msg1;
			col_msg1.otherEntity = iter->second;
			iter->first->SendMsg(&col_msg1);

			CollisionMessage col_msg2;
			col_msg2.otherEntity = iter->first;
			iter->second->SendMsg(&col_msg2);
		}
	}

	void PhysicsSystem::SetCollisionBetweenGroups(uint8_t group1, uint8_t group2, bool canCollide)
	{
		if (group1 > group2)
			std::swap(group1, group2);

		uint32_t index = (group1 << 4) | group2;
		m_GroupCollisionState[index] = canCollide;
	}

	bool PhysicsSystem::GetCollisionBetweenGroups(uint8_t group1, uint8_t group2) const
	{
		if (group1 > group2)
			std::swap(group1, group2);

		uint32_t index = (group1 << 4) | group2;
		return m_GroupCollisionState[index];
	}

	bool PhysicsSystem::RayCast(const Ray& ray, Entity** outEntity /*= nullptr*/, Vector3* outHit /*= nullptr*/, Vector3* outNormal /*= nullptr*/, uint16_t collisionGroupMasks /*= COLLISION_GROUP_ALL*/)
	{
		AUTO_PROFILER_BLOCK("Ray casting");
		Ray testRay = ray;
		bool result = false;
		Vector3 hitNormal;
		Entity* ent = nullptr;

		// Test ray intersection with all static colliders
		if (s_bUseQuadtree)
		{
			float t = 0;
			Vector3 normal;
			StaticCollider* hitCollider;
			if (static_cast<StaticQuadtree*>(m_StaticQuadtree)->RayCast(testRay, &hitCollider, &t, outNormal ? &normal : nullptr, collisionGroupMasks))
			{
				hitNormal = normal;
				testRay.Distance = t;
				result = true;
				ent = hitCollider->GetEntity();
			}
		}
		else
		{
			for (uint32_t j = 0; j < s_kMaxStaticColliders; ++j)
			{
				if (!m_StaticColliders[j].IsAlive())
					continue;

				if (!m_StaticColliders[j].IsEnabled())
					continue;

				int groupId = (1 << m_StaticColliders[j].GetCollisionGroup());
				if ((groupId & collisionGroupMasks) == 0)
					continue;

				float t = 0;
				Vector3 normal;

				if (m_StaticColliders[j].TestIntersectionWithRay(testRay, &t, outNormal ? &normal : nullptr))
				{
					hitNormal = normal;
					testRay.Distance = t;
					result = true;
					ent = m_StaticColliders[j].GetEntity();
				}
			}
		}

		// Test ray intersection with all dynamic colliders
		for (uint32_t j = 0; j < s_kMaxDynamicColliders; ++j)
		{
			if (!m_DynamicColliders[j].IsAlive())
				continue;

			if (!m_DynamicColliders[j].IsEnabled())
				continue;

			int groupId = (1 << m_DynamicColliders[j].GetCollisionGroup());
			if ((groupId & collisionGroupMasks) == 0)
				continue;
			
			float t = 0;
			Vector3 normal;

			if (m_DynamicColliders[j].TestIntersectionWithRay(testRay, &t, outNormal ? &normal : nullptr))
			{
				hitNormal = normal;
				testRay.Distance = t;
				result = true;
				ent = m_DynamicColliders[j].GetEntity();
			}
		}

		// Output hit entity
		if (result && outEntity)
		{
			*outEntity = ent;
		}

		// Output hit point and hit normal
		if (result && (outHit || outNormal))
		{
			Vector3 hitPoint = testRay.GetEndPoint();

			if (outHit)
			{
				*outHit = hitPoint;
			}

			if (outNormal)
			{
				*outNormal = hitNormal;
			}
		}

		return result;
	}

	bool PhysicsSystem::LinePlaneTest(const FXMVECTOR & origin, const FXMVECTOR & direction, const FXMVECTOR & normal, const float offset, float *t)
	{
		float angle = XMVectorGetX(XMVector3Dot(direction, normal));
		if (fabsf(angle)<= FLT_EPSILON) return false;
		float height;
		XMStoreFloat(&height, XMVector3Dot(origin, normal));
		if (t)
		{
			*t = ( height - offset) / angle;
		}
		return true;
	}

	std::vector<Entity*> PhysicsSystem::TestBoxOverlapping(const Aabb& aabb, uint16_t collisionGroupMasks /*= COLLISION_GROUP_ALL*/)
	{
		std::vector<Entity*> overlaps;

		for (uint32_t i = 0; i < s_kMaxStaticColliders; ++i)
		{
			if (!m_StaticColliders[i].IsAlive())
				continue;

			if (!m_StaticColliders[i].IsEnabled())
				continue;

			int groupId = (1 << m_StaticColliders[i].GetCollisionGroup());
			if ((groupId & collisionGroupMasks) == 0)
				continue;

			if (m_StaticColliders[i].GetColliderWorldAabb().Intersects(aabb))
			{
				overlaps.push_back(m_StaticColliders[i].GetEntity());
			}
		}

		for (uint32_t i = 0; i < s_kMaxDynamicColliders; ++i)
		{
			if (!m_DynamicColliders[i].IsAlive())
				continue;

			if (!m_DynamicColliders[i].IsEnabled())
				continue;

			int groupId = (1 << m_DynamicColliders[i].GetCollisionGroup());
			if ((groupId & collisionGroupMasks) == 0)
				continue;
	
			if (m_DynamicColliders[i].GetColliderWorldAabb().Intersects(aabb))
			{
				overlaps.push_back(m_DynamicColliders[i].GetEntity());
			}
		}

		return overlaps;
	}

}
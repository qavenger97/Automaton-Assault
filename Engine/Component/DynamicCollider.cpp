#include "pch.h"

#include "DynamicCollider.h"
#include "..\System\PhysicsSystem.h"
#include "Core\Entity.h"
#include "Renderer\DebugRenderer.h"
#include "Collision\Collision.h"

bool g_bDrawDebugDynamicCollider = false;

namespace Hourglass
{
	void DynamicCollider::Init()
	{
		m_MoveVector = Vector3(0.0f, 0.0f, 0.0f);
	}

	uint32_t DynamicCollider::s_TypeID = ComponentFactory::GetSystemComponentID();

	void DynamicCollider::Update()
	{
		Entity* ent = GetEntity();

		switch (m_CollisionShapeType)
		{
		case kAabb:
			m_WorldAabb = GetLocalAabbShape().GetTransformedAabb(ent->GetTransform()->GetMatrix());
			break;

		case kCylinder:
			m_WorldAabb = GetWorldCylinderShape().GetAabb();
			break;

		default:
			break;
		}
	}
	

	void DynamicCollider::DebugDrawCollisionShape() const
	{
		if (!g_bDrawDebugDynamicCollider)
			return;

		//if (m_IsRotated)
		//	DevTextRenderer::DrawText_WorldSpace("Rotated Col", GetEntity()->GetPosition());

		XMVECTOR vUp = GetEntity()->GetTransform()->Up();
		vUp = XMVector3Normalize(vUp);

		// Draw wired debug collision shape for collider
		switch (m_CollisionShapeType)
		{
		case kAabb:
			{
				Color aabbColor = Color(0, 1, 0);

				// Draw aabb with object's transform
				Matrix m = GetEntity()->GetTransform()->GetMatrix();
				if (XMVectorGetX(XMVector3Dot(vUp, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))) != 1.0f)
					aabbColor = Color(1, 0, 0);

				if (IsTrigger())
					aabbColor = Color(0, 0.5f, 1.0f);

				DebugRenderer::DrawAabb(GetLocalAabbShape(), m, aabbColor);
			}
			break;
			
		case kCylinder:
			{
				Cylinder worldCylinder = GetWorldCylinderShape();
				DebugRenderer::DrawCylinder(worldCylinder, Color(1, 1, 1));
			}
			break;

		default:
			break;
		}
	}

	Hourglass::Aabb DynamicCollider::GetSweptAabb() const
	{
		return GetColliderWorldAabb().GetSweptAabb(GetMoveVector());
	}

	void DynamicCollider::Move(const Vector3& v)
	{
		m_MoveVector += v;
	}

	DirectX::SimpleMath::Vector3 DynamicCollider::GetGroundedPosition(float dist/*=2.0f*/)
	{
		Entity* ent = GetEntity();
		assert(ent);

		Vector3 ent_pos = ent->GetPosition();
		Vector3 offset = Vector3(0.0f, 0.1f, 0.0f);			// In case starting point intersects ground point
		Ray ray(ent_pos + offset, ent_pos + Vector3(0, -dist, 0));
		Vector3 hit;

		if (!g_Physics.RayCast(ray, nullptr, &hit))
			return ent_pos;

		float dy = 0.0f;

		switch (m_CollisionShapeType)
		{
		case kAabb:
			dy = -GetLocalAabbShape().pMin.y;
			return hit + Vector3(0, dy, 0);

		case kCylinder:
			dy = -GetLocalCylinderShape().GetBottomY();
			return hit + Vector3(0, dy, 0);

		}

		return ent_pos;
	}

	const float DynamicCollider::GetGroundOffset() const
	{
		return m_Cylinder.Height * 0.5f;
	}

	bool DynamicCollider::TestDynamicCollision(const Vector3& moveVec, const Collider* otherCollider, Vector3* outVec) const
	{
		switch (m_CollisionShapeType)
		{
		case kAabb:
			return GetColliderWorldAabb().TestDynamicCollisionWithAabb(moveVec, otherCollider->GetColliderWorldAabb(), outVec);

		case kCylinder:
			// TODO: Handle collisions between cylinder and other shapes
			if (otherCollider->GetCollisionShapeType() == kAabb)
			{
				if (otherCollider->HasColliderRotation())
					return GetWorldCylinderShape().TestDynamicCollisionWithTransformedAabb(
						moveVec, otherCollider->GetLocalAabbShape(), otherCollider->GetEntity()->GetTransform()->GetMatrix(), outVec);
				else
					return GetWorldCylinderShape().TestDynamicCollisionWithAabb(moveVec, otherCollider->GetColliderWorldAabb(), outVec);
			}
			else if (otherCollider->GetCollisionShapeType() == kCylinder)
				return GetWorldCylinderShape().TestDynamicCollisionWithCylinder(moveVec, otherCollider->GetWorldCylinderShape(), outVec);

		default:
			// Ignore unspecified collision shapes
			return false;
		}
	}

	IComponent* DynamicCollider::MakeCopyDerived() const
	{		
		DynamicCollider* copy = (DynamicCollider*)IComponent::Create( SID(DynamicCollider) );

		copy->m_CollisionGroupId = m_CollisionGroupId;
		copy->m_CollisionShapeType = m_CollisionShapeType;

		if (m_CollisionShapeType == kAabb)
			copy->m_Aabb = m_Aabb;
		else if (m_CollisionShapeType == kCylinder)
			copy->m_Cylinder = m_Cylinder;

		copy->m_IsRotated = m_IsRotated;
		copy->m_IsTrigger = m_IsTrigger;

		copy->m_WorldAabb = m_WorldAabb;
		return copy;
	}
}
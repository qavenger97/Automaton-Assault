#include "pch.h"

#include "StaticCollider.h"
#include "..\System\PhysicsSystem.h"
#include "Renderer/DebugRenderer.h"
#include "Collision\Collision.h"

bool g_bDrawDebugStaticCollider = false;

namespace Hourglass
{

	void StaticCollider::Init()
	{
	}

	void StaticCollider::Update()
	{
		if (g_bDrawDebugStaticCollider)
		{
			DebugRenderer::DrawAabb(m_WorldAabb, Vector4(1.0f, 1.0f, 0.0f, 1.0f));

			//char buf[1024];
			//sprintf_s(buf, "Col group: %d", m_CollisionGroupId);
			//DevTextRenderer::DrawText_WorldSpace(buf, GetEntity()->GetPosition());
		}
	}

	uint32_t StaticCollider::s_TypeID = ComponentFactory::GetSystemComponentID();

	IComponent* StaticCollider::MakeCopyDerived() const
	{		
		StaticCollider* copy = (StaticCollider*)IComponent::Create( SID(StaticCollider) );

		copy->m_CollisionGroupId = m_CollisionGroupId;
		copy->m_CollisionShapeType = m_CollisionShapeType;

		if (m_CollisionShapeType == kAabb)
			copy->m_Aabb = m_Aabb;
		else if (m_CollisionShapeType == kCylinder)
			copy->m_Cylinder = m_Cylinder;

		copy->m_IsRotated = m_IsRotated;
		copy->m_IsTrigger = m_IsTrigger;

		copy->SetAabb(m_WorldAabb);
		return copy;
	}
}
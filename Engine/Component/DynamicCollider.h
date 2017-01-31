#pragma once

#include "Collider.h"
#include "Collision/Cylinder.h"
#include "Core/ComponentRegister.h"
#include "Math/Aabb.h"

namespace Hourglass
{
	class DynamicCollider : public Collider
	{
	public:
		void Init();
		void Update();

		virtual int GetTypeID() const { return s_TypeID; }

		// Debug draw collision shape for this collider
		void DebugDrawCollisionShape() const;

		// Get a swept aabb base on movement of collider
		Aabb GetSweptAabb() const;

		// Move alone a vector
		void Move(const Vector3& v);

		// Drop current object to ground
		Vector3 GetGroundedPosition(float dist=2.0f);

		// Component system methods for movement calculation
		void SetMoveVector(const Vector3& v) { m_MoveVector = v; }
		void ClearMoveVector() { m_MoveVector = Vector3::Zero; }
		const Vector3& GetMoveVector() const { return m_MoveVector; }

		const float GetGroundOffset() const;

		// true if dynamic collider didn't move since last frame
		bool IsStationary() const {	return m_MoveVector == Vector3::Zero; }

		bool TestDynamicCollision(const Vector3& moveVec, const Collider* otherCollider, Vector3* outVec) const;

		/**
		* Make a copy of this component
		*/
		virtual IComponent* MakeCopyDerived() const;

		static uint32_t s_TypeID;

	private:
		Vector3 m_MoveVector;
	};
}
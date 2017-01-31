#pragma once

#include "../Math/Aabb.h"

namespace Hourglass
{
	// An upright cylinder that is used for character colliders
	class Cylinder
	{
	public:
		Vector3 Center;
		float Radius;
		float Height;

		Cylinder();

		float GetTopY() const		{ return Center.y + Height * 0.5f; }
		float GetBottomY() const	{ return Center.y - Height * 0.5f; }

		Aabb GetAabb() const;
		Aabb GetSweptAabb(const Vector3& moveVec) const;

		bool TestIntersectionWithEdgeList				(const Vector2* edgeList, int edgeCount, float top, float bottom) const;
		bool TestInteresctionWithTransformedAabb		(const Aabb& other, const Matrix& transform) const;

		bool TestDynamicCollisionWithEdgeList			(const Vector3& moveVec, const Vector2* edgeList, int edgeCount, float top, float bottom, Vector3* outVec) const;
		bool TestDynamicCollisionWithAabb				(const Vector3& moveVec, const Aabb& other, Vector3* outVec) const;
		bool TestDynamicCollisionWithTransformedAabb	(const Vector3& moveVec, const Aabb& other, const Matrix& transform, Vector3* outVec) const;
		bool TestDynamicCollisionWithCylinder			(const Vector3& moveVec, const Cylinder& other, Vector3* outVec) const;
	};
}
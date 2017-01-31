/*!
 * \file Aabb.h
 *
 * \author Stanley
 * \date October 2016
 *
 * Axis-aligned bounding box
 */
#pragma once

namespace Hourglass
{
	class Aabb
	{
	public:
		Vector3 pMin, pMax;

		// ctor: Initialize aabb to a invalid size
		Aabb();
		Aabb(const float* p);
		Aabb(FXMVECTOR vMin, FXMVECTOR vMax);
		Aabb(const Vector3& vMin, const Vector3& vMax);

		BoundingBox GetDirectXBoundingBox() const;

		// Get center of current aabb;
		Vector3 GetCenter() const;

		// Expand size of aabb by a point
		inline void Expand(const Vector3& p)
		{
			if (p.x < pMin.x) pMin.x = p.x;
			if (p.y < pMin.y) pMin.y = p.y;
			if (p.z < pMin.z) pMin.z = p.z;
			if (p.x > pMax.x) pMax.x = p.x;
			if (p.y > pMax.y) pMax.y = p.y;
			if (p.z > pMax.z) pMax.z = p.z;
		}

		// Expand size of aabb by another aabb
		inline void Expand(const Aabb& aabb)
		{
			Expand(aabb.pMin);
			Expand(aabb.pMax);
		}

		// Expand size of aabb by a sphere
		inline void ExpandBySphere(const Vector3& center, float radius)
		{
			if (center.x - radius < pMin.x) pMin.x = center.x - radius;
			if (center.y - radius < pMin.y) pMin.y = center.y - radius;
			if (center.z - radius < pMin.z) pMin.z = center.z - radius;
			if (center.x + radius > pMax.x) pMax.x = center.x + radius;
			if (center.y + radius > pMax.y) pMax.y = center.y + radius;
			if (center.z + radius > pMax.z) pMax.z = center.z + radius;
		}


		// Return true if (pMax >= pMin) on all three axises
		bool IsValid() const;

		// Make aabb invalid so it can be used for a new encapsulation
		void Invalidate();

		// Get a new aabb moved to target position
		Aabb GetTranslatedAabb(const FXMVECTOR& v) const;

		// Get a new aabb completely encapsulates current aabb with a transform
		Aabb GetTransformedAabb(const Matrix& m) const;

		// Get a swept aabb contains current aabb with a moving vector
		Aabb GetSweptAabb(const Vector3& moveVec) const;

		// Get surface normal of the closest face to a point
		Vector3 GetSurfaceNormalFromPoint(const Vector3& point) const;

		// Test if point lies inside aabb (excludes point-to-surface intersections)
		bool Contains(const Vector3& point) const;

		// Test if another aabb completely encapsulated by this aabb
		bool Contains(const Aabb& aabb) const;

		// Test if aabb intersects with another aabb (excludes surface-to-surface and edge-to-edge intersections)
		bool Intersects(const Aabb& aabb) const;

		// Test if aabb collides with another aabb alone moving vector
		//		outVec: modified moving vector that avoids collision
		bool TestDynamicCollisionWithAabb(const Vector3& moveVec, const Aabb& other, Vector3* outVec) const;
	};
}
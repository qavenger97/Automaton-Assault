#pragma once

#include "Aabb.h"

namespace Hourglass
{
	class Ray
	{
	public:
		Vector3 Origin;		// Start point of ray
		Vector3 Direction;		// Normalized direction of ray
		float Distance;		// Distance the ray can reach
		Vector3 InvDir;		// Inverse of direction for aabb intersection test

		Ray();
		Ray(const Vector3& origin, const Vector3& dir, float dist);
		Ray(const Vector3& start, const Vector3& end);

		// Get end point of a ray
		Vector3 GetEndPoint() const;

		// Transform ray by a matrix
		Ray Transform(const Matrix& m) const;

		// Return if ray hits an aabb.
		//		t: Output parameter for closest hit distance
		bool Intersects(const Aabb& aabb, float* t = nullptr) const;
	};
}
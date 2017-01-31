#pragma once

#include "../Math/Aabb.h"
#include "Cylinder.h"

namespace Hourglass
{
	enum CollisionShape
	{
		kCollisionShapeUndefined = -1,
		kAabb = 0,							// Axis-aligned bounding box
		kCylinder,							// Upright cylinder (usually for characters)

		kCollisionShapeCount,
	};

	template <typename T> inline int sgn(T val) {
		return (T(0) < val) - (val < T(0));
	}

	namespace Collision
	{
		// Test if two circle overlaps
		bool TestCollision2D_CircleToCircle(const Vector2& c1, float r1, const Vector2& c2, float r2);

		Vector2 ResolveCollision2D_CircleToCircle(const Vector2& c1, float r1, const Vector2& c2, float r2);

		Vector2 GetClosestPointOnLine(const Vector2& pt, const Vector2& start, const Vector2& end);
		Vector2 GetClosestPointOnLineSegment(const Vector2& pt, const Vector2& start, const Vector2& end);

		// Get point position to line segment
		// Return:  1 - point on the front side of line segment
		//			0 - point on the line segment
		//		   -1 - point on the back side of line segment
		inline int TestPointToLineSegment(const Vector2& pt, const Vector2& start, const Vector2& end)
		{
			return sgn((start.x - pt.x)*(end.y - pt.y) - (start.y - pt.y)*(end.x - pt.x));
		}

		// Test if a circle lies behind of or is intersecting with a line segment
		bool TestCollision2D_CircleToLineSegment(const Vector2& c, float r, const Vector2& start, const Vector2& end);

		// Resolve the state of collision between circle and line segment and return available position for the circle
		Vector2 ResolveCollision2D_CircleToLineSegment(const Vector2& c, float r, const Vector2& start, const Vector2& end);

	}
}
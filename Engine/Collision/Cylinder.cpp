#include "pch.h"

#include "Cylinder.h"
#include "Collision.h"
#include "Renderer\DebugRenderer.h"

Hourglass::Cylinder::Cylinder()
{

}

Hourglass::Aabb Hourglass::Cylinder::GetAabb() const
{
	return Aabb(Center - Vector3(Radius, Height * 0.5f, Radius), Center + Vector3(Radius, Height * 0.5f, Radius));
}

Hourglass::Aabb Hourglass::Cylinder::GetSweptAabb(const Vector3& moveVec) const
{
	return GetAabb().GetSweptAabb(moveVec);
}

bool Hourglass::Cylinder::TestIntersectionWithEdgeList(const Vector2* edgeList, int edgeCount, float top, float bottom) const
{
	float bottomY = GetBottomY();
	float topY = GetTopY();

	if (GetBottomY() > top)
		return false;

	if (GetTopY() < bottom)
		return false;

	Vector2 xzCenter = Vector2(Center.x, Center.z);

	for (int i = 0; i < edgeCount; i++)
	{
		const Vector2& e1 = edgeList[i];
		const Vector2& e2 = edgeList[(i + 1) % edgeCount];

		// Test if circle lies in front of line
		if (Collision::TestPointToLineSegment(xzCenter, e1, e2) > 0)
		{
			Vector2 pt = Collision::GetClosestPointOnLine(xzCenter, e1, e2);
			if ((pt - xzCenter).LengthSquared() > Radius * Radius)
				return false;
		}
	}

	return true;
}

bool Hourglass::Cylinder::TestInteresctionWithTransformedAabb(const Aabb& other, const Matrix& transform) const
{
	float top = other.pMax.y * transform._22 + transform._42;
	float bottom = other.pMin.y * transform._22 + transform._42;

	XMFLOAT4 pt[4];
	XMStoreFloat4(&pt[0], XMVector4Transform(XMVectorSet(other.pMin.x, 0.0f, other.pMin.z, 1.0f), transform));
	XMStoreFloat4(&pt[1], XMVector4Transform(XMVectorSet(other.pMin.x, 0.0f, other.pMax.z, 1.0f), transform));
	XMStoreFloat4(&pt[2], XMVector4Transform(XMVectorSet(other.pMax.x, 0.0f, other.pMax.z, 1.0f), transform));
	XMStoreFloat4(&pt[3], XMVector4Transform(XMVectorSet(other.pMax.x, 0.0f, other.pMin.z, 1.0f), transform));

	Vector2 lineSeg[] =
	{
		Vector2(pt[0].x, pt[0].z),
		Vector2(pt[1].x, pt[1].z),
		Vector2(pt[2].x, pt[2].z),
		Vector2(pt[3].x, pt[3].z),
	};

	return TestIntersectionWithEdgeList(lineSeg, 4, top, bottom);
}

bool Hourglass::Cylinder::TestDynamicCollisionWithEdgeList(const Vector3& moveVec, const Vector2* edgeList, int edgeCount, float top, float bottom, Vector3* outVec) const
{
	// Note: Assume swept aabb test has been done before actual dynamic collision testing,
	//       we don't bother doing any extra checking here.

	Vector2 xzMoveVec(moveVec.x, moveVec.z);
	Vector2 xzCenter = Vector2(Center.x, Center.z);

	int n = 0;
	for (int i = 0; i < edgeCount; i++)
	{
		if (Collision::TestCollision2D_CircleToLineSegment(xzCenter, Radius, edgeList[i], edgeList[(i + 1) % edgeCount]))
			n++;
	}

	bool hasYCol = false;
	Vector3 yColMoveVec;
	float colDepthY = 0.0f;		// How much depth in Y axis cylinder goes into aabb

	if (n > 0)
	{
		// Circle is intersecting with box, check y direction for overlapping
		if (GetBottomY() + moveVec.y < top)
		{
			yColMoveVec = moveVec;
			yColMoveVec.y = top - GetBottomY();
			colDepthY = fabsf(moveVec.y - yColMoveVec.y);
			hasYCol = true;
		}
		else if (GetTopY() + moveVec.y > bottom)
		{
			yColMoveVec = moveVec;
			yColMoveVec.y = bottom - GetTopY();
			colDepthY = fabsf(moveVec.y - yColMoveVec.y);
			hasYCol = true;
		}
	}

	Vector2 xzNewCenter = Vector2(Center.x, Center.z) + xzMoveVec;

	// Resolve circle-line segment collision with all side faces of aabb
	for (int i = 0; i < edgeCount; i++)
	{
		const Vector2& p1 = edgeList[i];
		const Vector2& p2 = edgeList[(i + 1) % edgeCount];
#if 0
		// Ignore edges with same direction as moving vector
		float dx = p2.x - p1.x;
		float dy = p2.y - p1.y;
		Vector3 normal(-dy, 0.0f, dx);

		if (normal.Dot(moveVec) >= 0)
			continue;
#endif

		xzNewCenter = Collision::ResolveCollision2D_CircleToLineSegment(xzNewCenter, Radius, p1, p2);
	}

	if (hasYCol && (xzNewCenter - xzCenter).LengthSquared() > colDepthY * colDepthY)
	{
		*outVec = yColMoveVec;
	}
	else
	{
		*outVec = Vector3(xzNewCenter.x, 0.0f, xzNewCenter.y) - Center;
		outVec->y = 0.0f;
	}

	return true;
}

bool Hourglass::Cylinder::TestDynamicCollisionWithAabb(const Vector3& moveVec, const Aabb& other, Vector3* outVec) const
{
	Vector2 lineSeg[] =
	{
		Vector2(other.pMin.x, other.pMin.z),
		Vector2(other.pMin.x, other.pMax.z),
		Vector2(other.pMax.x, other.pMax.z),
		Vector2(other.pMax.x, other.pMin.z),
	};

	return TestDynamicCollisionWithEdgeList(moveVec, lineSeg, 4, other.pMax.y, other.pMin.y, outVec);
}

bool Hourglass::Cylinder::TestDynamicCollisionWithTransformedAabb(const Vector3& moveVec, const Aabb& other, const Matrix& transform, Vector3* outVec) const
{
	float top = other.pMax.y * transform._22 + transform._42;
	float bottom = other.pMin.y * transform._22 + transform._42;

	XMFLOAT4 pt[4];
	XMStoreFloat4(&pt[0], XMVector4Transform(XMVectorSet(other.pMin.x, 0.0f, other.pMin.z, 1.0f), transform));
	XMStoreFloat4(&pt[1], XMVector4Transform(XMVectorSet(other.pMin.x, 0.0f, other.pMax.z, 1.0f), transform));
	XMStoreFloat4(&pt[2], XMVector4Transform(XMVectorSet(other.pMax.x, 0.0f, other.pMax.z, 1.0f), transform));
	XMStoreFloat4(&pt[3], XMVector4Transform(XMVectorSet(other.pMax.x, 0.0f, other.pMin.z, 1.0f), transform));

	Vector2 lineSeg[] =
	{
		Vector2(pt[0].x, pt[0].z),
		Vector2(pt[1].x, pt[1].z),
		Vector2(pt[2].x, pt[2].z),
		Vector2(pt[3].x, pt[3].z),
	};

	return TestDynamicCollisionWithEdgeList(moveVec, lineSeg, 4, top, bottom, outVec);
}

bool Hourglass::Cylinder::TestDynamicCollisionWithCylinder(const Vector3& moveVec, const Cylinder& other, Vector3* outVec) const
{
	Vector2 xzMoveVec(moveVec.x, moveVec.z);
	Vector2 xzCenter = Vector2(Center.x, Center.z);
	Vector2 xzOtherCenter = Vector2(other.Center.x, other.Center.z);

	bool hasYCol = false;
	Vector3 yColMoveVec;
	float colDepthY = 0.0f;		// How much depth in Y axis cylinder goes into aabb

	if (Collision::TestCollision2D_CircleToCircle(xzCenter, Radius, xzOtherCenter, other.Radius))
	{
		if (GetBottomY() + moveVec.y < other.GetTopY())
		{
			yColMoveVec = moveVec;
			yColMoveVec.y = other.GetTopY() - GetBottomY();
			colDepthY = fabsf(moveVec.y - yColMoveVec.y);
			hasYCol = true;
		}
		else if (GetTopY() + moveVec.y > other.GetBottomY())
		{
			yColMoveVec = moveVec;
			yColMoveVec.y = other.GetBottomY() - GetTopY();
			colDepthY = fabsf(moveVec.y - yColMoveVec.y);
			hasYCol = true;
		}
	}
		
	Vector2 xzNewCenter = Vector2(Center.x, Center.z) + xzMoveVec;

	xzNewCenter = Collision::ResolveCollision2D_CircleToCircle(xzNewCenter, Radius, xzOtherCenter, other.Radius);

	if (hasYCol && (xzNewCenter - xzCenter).LengthSquared() > colDepthY * colDepthY)
	{
		*outVec = yColMoveVec;
	}
	else
	{
		*outVec = Vector3(xzNewCenter.x, 0.0f, xzNewCenter.y) - Center;
		outVec->y = 0.0f;
	}

	return true;
}

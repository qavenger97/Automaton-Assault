#include "pch.h"

#include "Collision.h"

bool Hourglass::Collision::TestCollision2D_CircleToCircle(const Vector2& c1, float r1, const Vector2& c2, float r2)
{
	return (c1 - c2).LengthSquared() < (r1 + r2) * (r1 + r2);
}

DirectX::SimpleMath::Vector2 Hourglass::Collision::ResolveCollision2D_CircleToCircle(const Vector2& c1, float r1, const Vector2& c2, float r2)
{
	if (!TestCollision2D_CircleToCircle(c1, r1, c2, r2))
		return c1;

	Vector2 d = c1 - c2;
	d.Normalize();
	return c2 + d * (r1 + r2);
}

DirectX::SimpleMath::Vector2 Hourglass::Collision::GetClosestPointOnLine(const Vector2& pt, const Vector2& start, const Vector2& end)
{
	// Reference: http://ericleong.me/research/circle-line/#the-closest-point-on-a-line-to-a-point-algorithm
	float A1 = end.y - start.y;
	float B1 = start.x - end.x;
	float C1 = (end.y - start.y)*start.x + (start.x - end.x)*start.y;
	float C2 = -B1*pt.x + A1*pt.y;
	float det = A1*A1 - -B1*B1;
	float cx = 0;
	float cy = 0;
	if (det != 0) {
		cx = (A1*C1 - B1*C2) / det;
		cy = (A1*C2 - -B1*C1) / det;
	}
	else {
		cx = pt.x;
		cy = pt.y;
	}

	return Vector2(cx, cy);
}

DirectX::SimpleMath::Vector2 Hourglass::Collision::GetClosestPointOnLineSegment(const Vector2& pt, const Vector2& start, const Vector2& end)
{
	float l = (end - start).Length();
	Vector2 n = end - start;
	n.Normalize();
	float dp = (pt - start).Dot(n);
	if (dp > l)
		return end;

	if (dp < 0)
		return start;

	// Reference: http://ericleong.me/research/circle-line/#the-closest-point-on-a-line-to-a-point-algorithm
	float A1 = end.y - start.y;
	float B1 = start.x - end.x;
	float C1 = (end.y - start.y)*start.x + (start.x - end.x)*start.y;
	float C2 = -B1*pt.x + A1*pt.y;
	float det = A1*A1 - -B1*B1;
	float cx = 0;
	float cy = 0;
	if (det != 0) {
		cx = (A1*C1 - B1*C2) / det;
		cy = (A1*C2 - -B1*C1) / det;
	}
	else {
		cx = pt.x;
		cy = pt.y;
	}

	return Vector2(cx, cy);
}

bool Hourglass::Collision::TestCollision2D_CircleToLineSegment(const Vector2& c, float r, const Vector2& start, const Vector2& end)
{
	if (TestPointToLineSegment(c, start, end) < 0)
		return true;

	Vector2 p = GetClosestPointOnLineSegment(c, start, end);
	Vector2 d = c - p;
	return d.LengthSquared() < r * r;
}

DirectX::SimpleMath::Vector2 Hourglass::Collision::ResolveCollision2D_CircleToLineSegment(const Vector2& c, float r, const Vector2& start, const Vector2& end)
{
	if (TestPointToLineSegment(c, start, end) < 0)
		return c;

	// note: normal of line segment, 90 degree counter-clockwise to line segment direction
	//Vector2 n = end - start;
	//n.Normalize();
	//n = Vector2(-n.y, n.x);

	// p: closest point on line segment to center of circle
	Vector2 p = GetClosestPointOnLineSegment(c, start, end);
	Vector2 d = c - p;
	if (d.LengthSquared() < r * r)
	{
		d.Normalize();
		return p + d * r;
	}

	// No collision, keep original circle position
	return c;
}

#include "pch.h"

#include "Ray.h"

Hourglass::Ray::Ray()
{
}

DirectX::SimpleMath::Vector3 Hourglass::Ray::GetEndPoint() const
{
	return Origin + Direction * Distance;
}

Hourglass::Ray::Ray(const Vector3& origin, const Vector3& dir, float dist)
	: Origin(origin), Distance(dist)
{
	XMStoreFloat3(&Direction, XMVector3Normalize(XMLoadFloat3(&dir)));
	InvDir.x = 1.0f / Direction.x;
	InvDir.y = 1.0f / Direction.y;
	InvDir.z = 1.0f / Direction.z;
}

Hourglass::Ray::Ray(const Vector3& start, const Vector3& end)
	: Origin(start)
{
	XMVECTOR dir = XMLoadFloat3(&end) - XMLoadFloat3(&start);
	XMStoreFloat3(&Direction, XMVector3Normalize(dir));
	XMStoreFloat(&Distance, XMVector3Length(dir));
	InvDir.x = 1.0f / Direction.x;
	InvDir.y = 1.0f / Direction.y;
	InvDir.z = 1.0f / Direction.z;
}

Hourglass::Ray Hourglass::Ray::Transform(const Matrix& m) const
{
	XMVECTOR origin = XMVectorSet(Origin.x, Origin.y, Origin.z, 1.0f);
	XMVECTOR dirVec = XMVectorSet(Direction.x, Direction.y, Direction.z, 0.0f) * Distance;
	XMMATRIX transform = XMLoadFloat4x4(&m);
	origin = XMVector4Transform(origin, transform);
	dirVec = XMVector4Transform(dirVec, transform);

	Ray ray;
	XMStoreFloat3(&ray.Origin, origin);
	XMStoreFloat3(&ray.Direction, XMVector3Normalize(dirVec));
	XMStoreFloat(&ray.Distance, XMVector3Length(dirVec));
	ray.InvDir.x = 1.0f / ray.Direction.x;
	ray.InvDir.y = 1.0f / ray.Direction.y;
	ray.InvDir.z = 1.0f / ray.Direction.z;

	return ray;
}

bool Hourglass::Ray::Intersects(const Aabb& aabb, float* t /*= nullptr*/) const
{
	if (!aabb.IsValid())
		return false;

	if (aabb.Contains(Origin))
	{
		if (t) *t = 0.0f;
		return true;
	}

	// Reference: http://people.csail.mit.edu/amy/papers/box-jgt.pdf

	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	if (Direction.x >= 0.0f)
	{
		tmin = (aabb.pMin.x - Origin.x) * InvDir.x;
		tmax = (aabb.pMax.x - Origin.x) * InvDir.x;
	}
	else
	{
		tmin = (aabb.pMax.x - Origin.x) * InvDir.x;
		tmax = (aabb.pMin.x - Origin.x) * InvDir.x;
	}

	if (Direction.y >= 0.0f)
	{
		tymin = (aabb.pMin.y - Origin.y) * InvDir.y;
		tymax = (aabb.pMax.y - Origin.y) * InvDir.y;
	}
	else
	{
		tymin = (aabb.pMax.y - Origin.y) * InvDir.y;
		tymax = (aabb.pMin.y - Origin.y) * InvDir.y;
	}

	if ((tmin > tymax) || (tymin > tmax))
		return false;
	if (tymin > tmin)
		tmin = tymin;
	if (tymax < tmax)
		tmax = tymax;

	if (Direction.z >= 0.0f)
	{
		tzmin = (aabb.pMin.z - Origin.z) * InvDir.z;
		tzmax = (aabb.pMax.z - Origin.z) * InvDir.z;
	}
	else
	{
		tzmin = (aabb.pMax.z - Origin.z) * InvDir.z;
		tzmax = (aabb.pMin.z - Origin.z) * InvDir.z;
	}

	if ((tmin > tzmax) || (tzmin > tmax))
		return false;
	if (tzmin > tmin)
		tmin = tzmin;
	if (tzmax < tmax)
		tmax = tzmax;

	if (tmax > tmin && tmin >= 0 && tmin <= Distance)
	{
		if (t)
			*t = tmin;
		return true;
	}

	return false;
}


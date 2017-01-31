/*!
 * \file Aabb.cpp
 *
 * \author Shiyang
 * \date October 2016
 *
 * Axis-aligned bounding box
 */
#include "pch.h"

#include "Aabb.h"

Hourglass::Aabb::Aabb()
	: pMin(FLT_MAX, FLT_MAX, FLT_MAX),
	  pMax(-FLT_MAX, -FLT_MAX, -FLT_MAX)
{
}

DirectX::BoundingBox Hourglass::Aabb::GetDirectXBoundingBox() const
{
	return BoundingBox((pMin + pMax) * 0.5f, (pMax - pMin) * 0.5f);
}

DirectX::SimpleMath::Vector3 Hourglass::Aabb::GetCenter() const
{
	return (pMin + pMax) * 0.5f;
}

Hourglass::Aabb::Aabb(const float* p)
{
	pMin = Vector3(p);
	pMax = Vector3(p + 3);
}

Hourglass::Aabb::Aabb(FXMVECTOR vMin, FXMVECTOR vMax)
{
	XMStoreFloat3(&pMin, vMin);
	XMStoreFloat3(&pMax, vMax);
}

Hourglass::Aabb::Aabb(const Vector3& vMin, const Vector3& vMax)
	: pMin(vMin), pMax(vMax)
{

}

bool Hourglass::Aabb::IsValid() const
{
	return pMax.x >= pMin.x && pMax.y >= pMin.y && pMax.z >= pMin.z;
}

void Hourglass::Aabb::Invalidate()
{
	pMin = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);
	pMax = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
}

Hourglass::Aabb Hourglass::Aabb::GetTranslatedAabb(const FXMVECTOR& v) const
{
	XMVECTOR vec = v,
			 vMin = XMLoadFloat3(&pMin),
			 vMax = XMLoadFloat3(&pMax);

	Aabb box;
	XMStoreFloat3(&box.pMin, vMin + vec);
	XMStoreFloat3(&box.pMax, vMax + vec);

	return box;
}

Hourglass::Aabb Hourglass::Aabb::GetTransformedAabb(const Matrix& m) const
{
	XMFLOAT4X4 fm;
	XMStoreFloat4x4( &fm, m );
	Aabb aabb;
	aabb.pMin = aabb.pMax = Vector3(fm._41, fm._42, fm._43);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			float e = fm.m[j][i] * ((float*)&pMin)[j];
			float f = fm.m[j][i] * ((float*)&pMax)[j];
			if (e < f)
			{
				((float*)&aabb.pMin)[i] += e;
				((float*)&aabb.pMax)[i] += f;
			}
			else
			{
				((float*)&aabb.pMin)[i] += f;
				((float*)&aabb.pMax)[i] += e;
			}
		}
	}

	return aabb;
}

Hourglass::Aabb Hourglass::Aabb::GetSweptAabb(const Vector3& moveVec) const
{
	Aabb sweptAabb;
	sweptAabb.pMin = pMin;
	sweptAabb.pMax = pMax;
	Vector3 aabbExpand;
	sweptAabb.Expand(GetTranslatedAabb(XMLoadFloat3(&moveVec)));

	return sweptAabb;
}

DirectX::SimpleMath::Vector3 Hourglass::Aabb::GetSurfaceNormalFromPoint(const Vector3& point) const
{
	float minDistToBound = FLT_MAX;
	float f;
	Vector3 normal;

	if ((f = fabsf(point.x - pMax.x)) < minDistToBound)
	{
		minDistToBound = f;
		normal = Vector3(1, 0, 0);
	}

	if ((f = fabsf(point.x - pMin.x)) < minDistToBound)
	{
		minDistToBound = f;
		normal = Vector3(-1, 0, 0);
	}

	if ((f = fabsf(point.y - pMax.y)) < minDistToBound)
	{
		minDistToBound = f;
		normal = Vector3(0, 1, 0);
	}

	if ((f = fabsf(point.y - pMin.y)) < minDistToBound)
	{
		minDistToBound = f;
		normal = Vector3(0, -1, 0);
	}

	if ((f = fabsf(point.z - pMax.z)) < minDistToBound)
	{
		minDistToBound = f;
		normal = Vector3(0, 0, 1);
	}

	if ((f = fabsf(point.z - pMin.z)) < minDistToBound)
	{
		minDistToBound = f;
		normal = Vector3(0, 0, -1);
	}

	return normal;
}

bool Hourglass::Aabb::Contains(const Vector3& point) const
{
	if (pMax.x <= point.x || pMin.x >= point.x)
		return false;
	if (pMax.y <= point.y || pMin.y >= point.y)
		return false;
	if (pMax.z <= point.z || pMin.z >= point.z)
		return false;

	return true;
}

bool Hourglass::Aabb::Contains(const Aabb& aabb) const
{
	return Contains(aabb.pMin) && Contains(aabb.pMax);
}

bool Hourglass::Aabb::Intersects(const Aabb& aabb) const
{
	if (pMax.x <= aabb.pMin.x || pMin.x >= aabb.pMax.x)
		return false;
	if (pMax.y <= aabb.pMin.y || pMin.y >= aabb.pMax.y)
		return false;
	if (pMax.z <= aabb.pMin.z || pMin.z >= aabb.pMax.z)
		return false;

	return true;
}

bool Hourglass::Aabb::TestDynamicCollisionWithAabb(const Vector3& moveVec, const Aabb& other, Vector3* outVec) const
{
	static const float tolerance = 0.001f;

	// Reduce size of aabb by the amount of tolerance
	Aabb aabb = other;
	aabb.pMin.x += tolerance;
	aabb.pMin.y += tolerance;
	aabb.pMin.z += tolerance;
	aabb.pMax.x -= tolerance;
	aabb.pMax.y -= tolerance;
	aabb.pMax.z -= tolerance;
	
	// Make sure we still have a valid aabb
	assert(aabb.IsValid());

	float tx = 2, ty = 2, tz = 2;

	// Check initial colliding
	if (pMax.x > aabb.pMin.x && pMin.x < aabb.pMax.x)
	{
		tx = -1;
	}
	else
	{
		if (pMax.x <= aabb.pMin.x && pMax.x + moveVec.x > aabb.pMin.x)
			tx = (aabb.pMin.x - pMax.x) / moveVec.x;
		else if (pMin.x >= aabb.pMax.x && pMin.x + moveVec.x < aabb.pMax.x)
			tx = (aabb.pMax.x - pMin.x) / moveVec.x;
	}

	if (pMax.y > aabb.pMin.y && pMin.y < aabb.pMax.y)
	{
		ty = -1;
	}
	else
	{
		if (pMax.y <= aabb.pMin.y && pMax.y + moveVec.y > aabb.pMin.y)
			ty = (aabb.pMin.y - pMax.y) / moveVec.y;
		else if (pMin.y >= aabb.pMax.y && pMin.y + moveVec.y < aabb.pMax.y)
			ty = (aabb.pMax.y - pMin.y) / moveVec.y;
	}

	if (pMax.z > aabb.pMin.z && pMin.z < aabb.pMax.z)
	{
		tz = -1;
	}
	else
	{
		if (pMax.z <= aabb.pMin.z && pMax.z + moveVec.z > aabb.pMin.z)
			tz = (aabb.pMin.z - pMax.z) / moveVec.z;
		else if (pMin.z >= aabb.pMax.z && pMin.z + moveVec.z < aabb.pMax.z)
			tz = (aabb.pMax.z - pMin.z) / moveVec.z;
	}

	// No collision on any of three axises, moving vector is good
	if (tx > 1 || ty > 1 || tz > 1)
		return false;

#define FLT_SGN(x) (x > 0.0f) ? 1.0f : ((x < 0.0f) ? -1.0f : 0.0f);

	Vector3 newVec = moveVec;
	if (tx >= 0 && tx >= ty && tx >= tz)
	{
		float s = FLT_SGN(newVec.x);
		newVec.x = s * max(fabs(newVec.x * tx) - tolerance, 0.0f);
		if (outVec) *outVec = newVec;
		return true;
	}
	else if (ty >= 0 && ty >= tx && ty >= tz)
	{
		float s = FLT_SGN(newVec.y);
		newVec.y = s * max(fabs(newVec.y * ty) - tolerance, 0.0f);
		if (outVec) *outVec = newVec;
		return true;
	}
	else if (tz >= 0 && tz >= tx && tz >= ty)
	{
		float s = FLT_SGN(newVec.z);
		newVec.z = s * max(fabs(newVec.z * tz) - tolerance, 0.0f);
		if (outVec) *outVec = newVec;
		return true;
	}

	if (outVec) *outVec = Vector3(-moveVec.x, -moveVec.y, -moveVec.z);
	return true;
}

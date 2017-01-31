#include "pch.h"

#include "Collider.h"
#include "Math\Aabb.h"
#include "Collision\Collision.h"
#include "Core\Entity.h"

void Hourglass::Collider::LoadFromXML(tinyxml2::XMLElement* data)
{
	tinyxml2::XMLElement* colShape = nullptr;

	if (nullptr != (colShape = data->FirstChildElement("AABB")))
	{
		// Get the AABB min values
		tinyxml2::XMLElement* minXML = colShape->FirstChildElement("Min");
		minXML->QueryFloatAttribute("x", &m_Aabb.pMin[0]);
		minXML->QueryFloatAttribute("y", &m_Aabb.pMin[1]);
		minXML->QueryFloatAttribute("z", &m_Aabb.pMin[2]);

		// Get the AABB max values
		tinyxml2::XMLElement* maxXML = colShape->FirstChildElement("Max");
		maxXML->QueryFloatAttribute("x", &m_Aabb.pMax[0]);
		maxXML->QueryFloatAttribute("y", &m_Aabb.pMax[1]);
		maxXML->QueryFloatAttribute("z", &m_Aabb.pMax[2]);

		m_CollisionShapeType = kAabb;
	}
	else if (nullptr != (colShape = data->FirstChildElement("Cylinder")))
	{
		colShape->QueryFloatAttribute("Height", &m_Cylinder.Height);
		colShape->QueryFloatAttribute("Radius", &m_Cylinder.Radius);

		tinyxml2::XMLElement* centerXML = colShape->FirstChildElement("Center");
		centerXML->QueryFloatAttribute("x", &m_Cylinder.Center[0]);
		centerXML->QueryFloatAttribute("y", &m_Cylinder.Center[1]);
		centerXML->QueryFloatAttribute("z", &m_Cylinder.Center[2]);

		m_CollisionShapeType = kCylinder;
	}

	if (IsStaticCollider())
		CalculateWorldStaticAabb();

	int groupId = COLLISION_GROUP_DEFAULT;
	data->QueryIntAttribute("coll_group", &groupId);
	m_CollisionGroupId = (uint8_t)groupId;

	m_IsRotated = false;
	data->QueryIntAttribute("IsRotated", (int*)&m_IsRotated);

	m_IsTrigger = false;
	data->QueryIntAttribute("IsTrigger", (int*)&m_IsTrigger);
}

void Hourglass::Collider::SetAabb(const Aabb& aabb)
{
	memcpy(&m_Aabb, &aabb, sizeof(float) * 6);
	m_CollisionShapeType = kAabb;

	if (IsStaticCollider())
		CalculateWorldStaticAabb();
}

const Hourglass::Aabb& Hourglass::Collider::GetColliderWorldAabb() const
{
	return m_WorldAabb;
}

void Hourglass::Collider::CalculateWorldStaticAabb()
{
	switch (m_CollisionShapeType)
	{
	case kAabb:
		m_WorldAabb = GetLocalAabbShape();
		break;

	case kCylinder:
		m_WorldAabb = GetWorldCylinderShape().GetAabb();
		break;

	default:
		break;
	}
}

Hourglass::Aabb Hourglass::Collider::GetLocalAabbShape() const
{
	assert(m_CollisionShapeType == kAabb);

	return Aabb((float*)&m_Aabb);
}

Hourglass::Cylinder Hourglass::Collider::GetLocalCylinderShape() const
{
	assert(m_CollisionShapeType == kCylinder);

	return Cylinder(*(Cylinder*)&m_Cylinder);
}

Hourglass::Cylinder Hourglass::Collider::GetWorldCylinderShape() const
{
	assert(m_CollisionShapeType == kCylinder);

	Cylinder cylinder(*(Cylinder*)&m_Cylinder);

	if (IsStaticCollider())
		return cylinder;
	else
	{
		cylinder.Center += GetEntity()->GetPosition();
		return cylinder;
	}
}

bool Hourglass::Collider::TestIntersectionWithRay(const Ray& ray, float* t /*= nullptr*/, Vector3* outNormal /*= nullptr*/) const
{
	if (m_IsTrigger)
		return false;

	const Aabb& worldAabb = GetColliderWorldAabb();

	if (m_CollisionShapeType == kAabb && m_IsRotated)
	{
		// Ray to transformed aabb checking is expensive, only check when necessary
		if (worldAabb.Contains(ray.Origin) || ray.Intersects(worldAabb))
		{
			Matrix m = GetEntity()->GetTransform()->GetMatrix();
			Matrix invMat = m.Invert();
			Ray invRay = ray.Transform(invMat);

			float inv_t;
			Aabb localAabb = GetLocalAabbShape();
			bool result = invRay.Intersects(localAabb, &inv_t);

			if (result)
			{
				Vector3 dirVec = invRay.Direction * inv_t;
				*t = Vector3::TransformNormal(dirVec, m).Length();

				if (outNormal)
				{
					Vector3 hitPoint = invRay.Origin + dirVec;
					Vector3 normal = localAabb.GetSurfaceNormalFromPoint(hitPoint);
					normal = Vector3::TransformNormal(normal, m);
					normal.Normalize();

					*outNormal = normal;
				}
			}

			return result;
		}

		return false;
	}

	bool result = ray.Intersects(worldAabb, t);

	if (result && outNormal)
	{
		Vector3 hitPoint = ray.Origin + ray.Direction * *t;
		*outNormal = worldAabb.GetSurfaceNormalFromPoint(hitPoint);
	}

	return result;
}

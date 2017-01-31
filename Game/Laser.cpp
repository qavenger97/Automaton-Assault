#include "GamePch.h"

#include "Laser.h"
#include "Common.h"

IMPLEMENT_GAME_COMPONENT_TYPEID(Laser)

void Laser::LoadFromXML( tinyxml2::XMLElement* data )
{
	m_Radius = 1.0f;
	data->QueryFloatAttribute( "radius", &m_Radius );
	data->QueryUnsignedAttribute( "coll_mask", &m_CollMask );
}

hg::IComponent* Laser::MakeCopyDerived() const
{
	Laser* copy = (Laser*)IComponent::Create(SID(Laser));
	copy->m_Radius = m_Radius;
	return copy;
}

void Laser::Update()
{
	hg::Transform* trans = GetEntity()->GetTransform();
	Vector3 start = trans->GetWorldPosition();

	Vector3 scale = trans->GetWorldScale();

	//char buf[1024];
	//sprintf_s(buf, "%f %f %f", scale.x, scale.y, scale.z);
	//hg::DevTextRenderer::DrawText_WorldSpace(buf, start);

	const float max_length = 100.0f;

	hg::Ray ray(start, trans->Forward(), max_length);
	Vector3 hitPos;

	if (hg::g_Physics.RayCast(ray, nullptr, &hitPos, nullptr, COLLISION_BULLET_HIT_MASK))
	{
		float dist = Vector3::Distance(hitPos, start);
		if (dist <= 0.01f)
			dist = 0.01f;
		trans->SetScale(m_Radius, m_Radius, dist);
	}
	else
	{
		trans->SetScale(m_Radius, m_Radius, max_length);
	}
}

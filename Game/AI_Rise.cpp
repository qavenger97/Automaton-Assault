#include "GamePch.h"

#include "AI_Rise.h"

void AI_Rise::LoadFromXML( tinyxml2::XMLElement* data )
{
	bool reverse = false;
	data->QueryBoolAttribute( "reverse", &reverse );
	if (reverse)
		m_Reverse = true;
	
	const char* surfaceEntityName = data->Attribute( "laser_entity_name" );
	if (surfaceEntityName)
		m_LaserEntityName = WSID( surfaceEntityName );

	m_Duration = 8.0f;
	data->QueryFloatAttribute( "duration", &m_Duration );
	m_StartSpeed = 9.25f;
	data->QueryFloatAttribute( "start_speed", &m_StartSpeed );
	m_EndSpeed = 1.0f;
	data->QueryFloatAttribute( "end_speed", &m_EndSpeed );
}

void AI_Rise::Init( Hourglass::Entity * entity )
{
	m_LaserEntity = hg::Entity::FindByName( m_LaserEntityName );
	m_TravelVec = Vector3(0.00126768160f,0.605186403f,-0.796082854f);
	m_Timer = 0.0f;
}

Hourglass::IBehavior::Result AI_Rise::Update( Hourglass::Entity* entity )
{
	float dt = hg::g_Time.Delta();
	m_Timer += dt;
	if (m_Timer >= m_Duration)
		return IBehavior::kSUCCESS;

	float speed = Math::Lerp( m_StartSpeed, m_EndSpeed, m_Timer / m_Duration );

	float r = m_Reverse ? (1.0f - m_Timer / m_Duration) : m_Timer / m_Duration;
	Vector3 displacement = m_TravelVec;
	displacement.y = sin( XM_2PI * 0.75f * r ) * 0.5f + 0.5f;
	displacement.y *= m_TravelVec.y * 2.0f;
	displacement.Normalize();
	displacement *= dt * speed;

	if (m_Reverse)
		displacement.y *= -1;
	
	hg::Transform* leTf = m_LaserEntity->GetTransform();
	Vector3 pos = leTf->GetWorldPosition();
	leTf->SetPosition( pos + displacement );
	return IBehavior::kRUNNING;
}

Hourglass::IBehavior* AI_Rise::MakeCopy() const
{
	AI_Rise* copy = (AI_Rise*)IBehavior::Create( SID(AI_Rise) );
	copy->m_LaserEntityName = m_LaserEntityName;
	copy->m_Duration = m_Duration;
	copy->m_StartSpeed = m_StartSpeed;
	copy->m_EndSpeed = m_EndSpeed;
	copy->m_TravelVec = m_TravelVec;
	copy->m_Reverse = m_Reverse;
	return copy;
}

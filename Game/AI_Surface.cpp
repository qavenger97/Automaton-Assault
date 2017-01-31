#include "GamePch.h"

#include "AI_Surface.h"

void AI_Surface::LoadFromXML( tinyxml2::XMLElement* data )
{
	const char* surfaceEntityName = data->Attribute( "surface_entity_name" );
	if (surfaceEntityName)
		m_SurfaceEntityName = WSID(surfaceEntityName);

	m_StartRotation = 0.0f;
	data->QueryFloatAttribute( "start_rotation", &m_StartRotation );

	m_EndRotation = 0.0f;
	data->QueryFloatAttribute( "end_rotation", &m_EndRotation );

	m_StartSpeed = 0.0f;
	data->QueryFloatAttribute( "start_speed", &m_StartSpeed );

	m_EndSpeed = 0.0f;
	data->QueryFloatAttribute( "end_speed", &m_EndSpeed );
}

void AI_Surface::Init( Hourglass::Entity* entity )
{
	m_SurfaceEntity = hg::Entity::FindByName( m_SurfaceEntityName );
	if (m_SurfaceEntity)
		m_SurfaceEntity->GetTransform()->SetRotation( 0.0f, 0.0f, m_StartRotation );

	m_Rotation = m_StartRotation;

	if (m_StartSpeed < m_EndSpeed)
		hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_BOSS_LEAVEGROUNDSHALLOW, entity->GetPosition());
	else
		hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_BOSS_ENTERGROUNDSHALLOW, entity->GetPosition());
}

Hourglass::IBehavior::Result AI_Surface::Update( Hourglass::Entity* entity )
{
	float r = (m_Rotation - m_StartRotation) / (m_EndRotation - m_StartRotation);
	float speed = Math::Lerp( m_StartSpeed, m_EndSpeed, r );
	m_Rotation += speed * hg::g_Time.Delta();

	if (m_Rotation <= m_EndRotation)
	{
		m_SurfaceEntity->GetTransform()->SetRotation( 0.0f, 0.0f, m_EndRotation );
		return IBehavior::kSUCCESS;
	}

	m_SurfaceEntity->GetTransform()->SetRotation( 0.0f, 0.0f, m_Rotation );

	return IBehavior::kRUNNING;
}

Hourglass::IBehavior* AI_Surface::MakeCopy() const
{
	AI_Surface* copy = (AI_Surface*)IBehavior::Create( SID(AI_Surface) );
	copy->m_StartSpeed = m_StartSpeed;
	copy->m_EndSpeed = m_EndSpeed;
	copy->m_SurfaceEntityName = m_SurfaceEntityName;
	copy->m_EndRotation = m_EndRotation;
	copy->m_StartRotation = m_StartRotation;
	return copy;
}

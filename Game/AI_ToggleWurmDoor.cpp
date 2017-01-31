#include "GamePch.h"

#include "AI_ToggleWurmDoor.h"

void AI_ToggleWurmDoor::LoadFromXML( tinyxml2::XMLElement* data )
{
	m_Open = false;

	const char* state = data->Attribute( "state" );
	if (strcmp( state, "open" ) == 0)
		m_Open = true;

	m_Duration = 1.0f;
	data->QueryFloatAttribute( "duration", &m_Duration );

	m_Angle = 75.0f;
	data->QueryFloatAttribute( "angle", &m_Angle );

	m_Angle = DirectX::XMConvertToRadians( m_Angle );
}

void AI_ToggleWurmDoor::Init( Hourglass::Entity * entity )
{
	m_Timer = 0.0f;
	m_Front = hg::Entity::FindByTag( "front_wurm_door" )->GetTransform();
	m_Back = hg::Entity::FindByTag( "back_wurm_door" )->GetTransform();
}

Hourglass::IBehavior::Result AI_ToggleWurmDoor::Update( Hourglass::Entity* entity )
{	
	m_Timer += hg::g_Time.Delta();

	float r = min( 1.0f, m_Timer / m_Duration );
	float rx = r * XM_PIDIV2;
	float ry = sinf( rx );

	if (!m_Open)
		ry = 1.0f - ry;

	UpdateDoorRotation( m_Front, m_Angle * ry );
	UpdateDoorRotation( m_Back, -m_Angle * ry );

	return (m_Timer >= m_Duration) ? IBehavior::kSUCCESS : IBehavior::kRUNNING;
}

void AI_ToggleWurmDoor::UpdateDoorRotation( hg::Transform* trans, float angle )
{
	Quaternion rot = Quaternion::CreateFromYawPitchRoll( 0.0f, 0.0f, angle );
	rot *= trans->GetParent()->GetWorldRotation();
	trans->SetRotation( rot );
}

Hourglass::IBehavior* AI_ToggleWurmDoor::MakeCopy() const
{
	AI_ToggleWurmDoor* copy = (AI_ToggleWurmDoor*)IBehavior::Create( SID(AI_ToggleWurmDoor) );
	copy->m_Open = m_Open;
	copy->m_Duration = m_Duration;
	copy->m_Angle = m_Angle;
	return copy;
}

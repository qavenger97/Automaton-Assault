#include "GamePch.h"

#include "AI_RadialPatrol.h"
#include "PlayerComp.h"
#include "GrapplerComp.h"

void AI_RadialPatrol::LoadFromXML( tinyxml2::XMLElement* data )
{	
	m_PatrolDuration = 5.0f;
	data->QueryFloatAttribute( "patrol_duration", &m_PatrolDuration );
	
	m_PatrolRadius = 16.0f;
	data->QueryFloatAttribute( "patrol_radius", &m_PatrolRadius );

	m_Center = Vector3( 36.0f, 0.0f, -9.0f );
	data->QueryFloatAttribute( "center_x", &m_Center.x );
	data->QueryFloatAttribute( "center_z", &m_Center.z );

	m_PatrolSpeed = 6.0f;
	data->QueryFloatAttribute( "patrol_speed", &m_PatrolSpeed );
	
	m_Motor = nullptr;
	m_Player = nullptr;
	m_PlayerComp = nullptr;
}

void AI_RadialPatrol::Start()
{
	
}

void AI_RadialPatrol::Init( Hourglass::Entity* entity )
{
	m_IntervalTimer = Math::RNG::Range( 1.0f, 4.0f );
	m_MovingRight = false;
	m_Timer = 0.0f;
	if (m_Motor == nullptr)
	{
		m_Motor = entity->GetComponent<hg::Motor>();
		m_Motor->SetEnabled( true );
		m_Motor->SetMoveEnabled( true );
	}

	if (m_PlayerComp == nullptr)
	{
		hg::Entity* player = Hourglass::Entity::FindByTag( SID( Player ) );
		m_PlayerComp = player->GetComponent<PlayerComp>();
		m_Player = player->GetTransform();
	}
}

Hourglass::IBehavior::Result AI_RadialPatrol::Update( Hourglass::Entity* entity )
{
	m_Timer += hg::g_Time.Delta();

	Vector3 toVector = m_Center;
	toVector -= entity->GetTransform()->GetWorldPosition();
	Vector3 perpVector = (m_MovingRight) ? toVector.Cross( Vector3::Up ) : Vector3::Up.Cross(toVector);
	perpVector.Normalize();
	m_Motor->SetDesiredMove( perpVector );
	m_Motor->SetDesiredForward( -toVector );

	if (m_Timer >= m_IntervalTimer)
	{
		m_IntervalTimer += Math::RNG::Range( 1.0f, 4.0f );
		m_MovingRight = !m_MovingRight;
	}

	if (m_Timer >= m_PatrolDuration)
	{
		m_Motor->SetMoveEnabled( false );
		return IBehavior::kSUCCESS;
	}

	return IBehavior::kRUNNING;
}

Hourglass::IBehavior* AI_RadialPatrol::MakeCopy() const
{
	AI_RadialPatrol* copy = (AI_RadialPatrol*)IBehavior::Create( SID(AI_RadialPatrol) );
	copy->m_PlayerComp = m_PlayerComp;
	copy->m_PatrolDuration = m_PatrolDuration;
	copy->m_PatrolRadius = m_PatrolRadius;
	copy->m_Center = m_Center;
	copy->m_Motor = nullptr;
	copy->m_Player = nullptr;
	copy->m_PlayerComp = nullptr;

	return copy;
}

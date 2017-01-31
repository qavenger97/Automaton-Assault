#include "GamePch.h"

#include "AI_TurnTo.h"
#include "Laser.h"
#include "Common.h"

void AI_TurnTo::LoadFromXML( tinyxml2::XMLElement* data )
{
	m_Acceleration = 0.0f;
	data->QueryFloatAttribute( "acceleration", &m_Acceleration );

	m_InitialVelocity = 3.0f;	
	data->QueryFloatAttribute( "velocity", &m_InitialVelocity );
}

void AI_TurnTo::Init( Hourglass::Entity* entity )
{
	m_Player = hg::Entity::FindByName( SID(Player) )->GetTransform();
	m_BossHead = hg::Entity::FindByName( SID(boss_head_inner) )->GetTransform();
	hg::Entity* laserEnt = hg::Entity::FindByName( SID( LaserEmissionPoint ) );
	m_Laser = laserEnt->GetTransform();
	m_Velocity = m_InitialVelocity;
}

Hourglass::IBehavior::Result AI_TurnTo::Update( Hourglass::Entity* entity )
{
	float dt = hg::g_Time.Delta();
	m_Velocity += m_Acceleration * dt;

	const int max_length = 100;
	Vector3 startPos = m_Laser->GetWorldPosition();
	hg::Ray ray( startPos, m_Laser->Forward(), (float)max_length );
	Vector3 hitPos;

	if (hg::g_Physics.RayCast( ray, nullptr, &hitPos, nullptr, COLLISION_BULLET_HIT_MASK ))
	{
		Vector3 targetPos = m_Player->GetWorldPosition();
		targetPos.y -= 0.5f;
		Vector3 dir = targetPos - hitPos;
		dir.Normalize();
		Vector3 lookAtPos = hitPos + dir * m_Velocity * dt;
		Quaternion rot = hg::Transform::BuildLookAtRotation( m_BossHead->GetWorldPosition(), lookAtPos);
		m_BossHead->SetRotation( rot );
	}
	else
	{
		m_BossHead->TurnTo( m_Player->GetWorldPosition(), dt * 5.0f);
	}
	return IBehavior::kRUNNING;
}

Hourglass::IBehavior* AI_TurnTo::MakeCopy() const
{
	AI_TurnTo* copy = (AI_TurnTo*)IBehavior::Create( SID(AI_TurnTo) );
	copy->m_Acceleration = m_Acceleration;
	copy->m_Velocity = m_Velocity;
	copy->m_InitialVelocity = m_InitialVelocity;
	return copy;
}
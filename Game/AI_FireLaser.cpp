#include "GamePch.h"

#include "AI_FireLaser.h"
#include "Common.h"
#include "Laser.h"

void AI_FireLaser::LoadFromXML( tinyxml2::XMLElement* data )
{
	m_Duration = 0.4f;
	data->QueryFloatAttribute( "duration", &m_Duration );

	bool reverse = false;
	data->QueryBoolAttribute( "reverse", &reverse );
	m_Reverse = reverse;
}

void AI_FireLaser::Init( Hourglass::Entity * entity )
{
	hg::Entity* laserEnt = hg::Entity::FindByName( SID(LaserEmissionPoint) );
	laserEnt->GetComponent<Laser>()->SetEnabled( true );
	laserEnt->GetComponent<hg::MeshRenderer>()->SetEnabled( true );

	m_Laser = laserEnt->GetTransform();
	m_Timer = 0.0f;

	const float max_length = 100.0f;

	m_StartScale = Vector3( 20.0f, 20.0f, 1.0f );

	Vector3 startPos = m_Laser->GetWorldPosition();
	hg::Ray ray( startPos, m_Laser->Forward(), max_length );
	Vector3 hitPos;

	if (hg::g_Physics.RayCast( ray, nullptr, &hitPos, nullptr, COLLISION_BULLET_HIT_MASK ))
	{
		float dist = Vector3::Distance( hitPos, startPos );
		if (dist <= 0.01f)
			dist = 0.01f;

		m_EndScale = m_StartScale;
		m_EndScale.z = dist;
	}
	else
	{
		m_EndScale = m_StartScale;
		m_EndScale.z = 100.0f;
	}

	if (m_Reverse)
	{
		Vector3 temp = m_StartScale;
		m_StartScale = m_EndScale;
		m_EndScale = temp;
	}

	m_bPlayingLaserBeamSound = false;
}

Hourglass::IBehavior::Result AI_FireLaser::Update(Hourglass::Entity* entity)
{
	if (!m_Reverse && !m_bPlayingLaserBeamSound)
	{
		hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_BOSS_LASERBEAM, entity->GetPosition());
		m_bPlayingLaserBeamSound = true;
	}

	m_Timer += hg::g_Time.Delta();
	m_Laser->SetScale( Vector3::Lerp( m_StartScale, m_EndScale, m_Timer / m_Duration ) );
	if (m_Timer >= m_Duration)
	{
		if (m_Reverse)
		{
			hg::Entity* laserEnt = hg::Entity::FindByName( SID( LaserEmissionPoint ) );
			laserEnt->GetComponent<Laser>()->SetEnabled( false );
			laserEnt->GetComponent<hg::MeshRenderer>()->SetEnabled( false );

			hg::g_AudioSystem.PostEvent(AK::EVENTS::STOP_BOSS_LASERBEAM);
		}
		return IBehavior::kSUCCESS;
	}
	else
	{
		return IBehavior::kRUNNING;
	}
}

Hourglass::IBehavior* AI_FireLaser::MakeCopy() const
{
	AI_FireLaser* copy = (AI_FireLaser*)IBehavior::Create( SID(AI_FireLaser) );
	copy->m_Duration = m_Duration;
	copy->m_StartScale = m_StartScale;
	copy->m_EndScale = m_EndScale;
	copy->m_Reverse = m_Reverse;
	return copy;
}	

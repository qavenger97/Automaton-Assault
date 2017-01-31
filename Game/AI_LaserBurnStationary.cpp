#include "GamePch.h"

#include "AI_LaserBurnStationary.h"
#include "Laser.h"
#include "Common.h"

void AI_LaserBurnStationary::LoadFromXML( tinyxml2::XMLElement* data )
{
	m_Duration = 20.0f;
	data->QueryFloatAttribute( "duration", &m_Duration );
}

void AI_LaserBurnStationary::Init( Hourglass::Entity * entity )
{
	hg::Entity* laserEnt = hg::Entity::FindByName( SID( LaserEmissionPoint ) );
	laserEnt->GetComponent<Laser>()->SetEnabled( true );
	laserEnt->GetComponent<hg::MeshRenderer>()->SetEnabled( true );

	m_Laser = laserEnt->GetTransform();
	m_Timer = 0.0f;

	const float max_length = 100.0f;

	Vector3 startPos = m_Laser->GetWorldPosition();
	hg::Ray ray( startPos, m_Laser->Forward(), max_length );
	Vector3 hitPos;

	if (hg::g_Physics.RayCast( ray, nullptr, &hitPos, nullptr, COLLISION_BULLET_HIT_MASK ))
	{
		m_FxLaserBurn = hg::Entity::Assemble( SID(FxLaserBurn) );
		m_FxLaserBurn->GetTransform()->SetPosition(hitPos);
	}
}

Hourglass::IBehavior::Result AI_LaserBurnStationary::Update( Hourglass::Entity* entity )
{
	m_Timer += hg::g_Time.Delta();

	if (m_Timer >= m_Duration)
	{
		m_FxLaserBurn->Destroy();
		return IBehavior::kSUCCESS;
	}
	else
		return IBehavior::kRUNNING;
}

Hourglass::IBehavior* AI_LaserBurnStationary::MakeCopy() const
{
	AI_LaserBurnStationary* copy = (AI_LaserBurnStationary*)IBehavior::Create( SID(AI_LaserBurnStationary) );
	copy->m_Duration = m_Duration;
	return copy;
}

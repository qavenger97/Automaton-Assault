#include "GamePch.h"

#include "AI_LaserChase.h"
#include "Common.h"
#include "HealthModule.h"
#include "GameMessage.h"


void AI_LaserChase::LoadFromXML( tinyxml2::XMLElement* data )
{
	m_Duration = 5.0f;
	data->QueryFloatAttribute( "duration", &m_Duration );

	m_DamageInterval = 0.4f;
	data->QueryFloatAttribute( "dmg_interval", &m_DamageInterval );
}

void AI_LaserChase::Init( Hourglass::Entity* entity )
{
	m_Laser = hg::Entity::FindByName( SID( LaserEmissionPoint ) )->GetTransform();
	m_FxLaserBurn = hg::Entity::Assemble( SID( FxLaserBurn ) )->GetComponent<hg::GPUParticleEmitter>();
	m_Timer = 0.0f;
	m_DamageTimer;
}

Hourglass::IBehavior::Result AI_LaserChase::Update( Hourglass::Entity* entity )
{
	m_Timer += hg::g_Time.Delta();
	m_DamageTimer += hg::g_Time.Delta();

	if (m_Timer >= m_Duration)
		return IBehavior::kSUCCESS;

	const float max_length = 100.0f;

	Vector3 startPos = m_Laser->GetWorldPosition();
	hg::Ray ray( startPos, m_Laser->Forward(), max_length );
	Vector3 hitPos;
	hg::Entity* hit;

	if (hg::g_Physics.RayCast( ray, &hit, &hitPos, nullptr, COLLISION_BULLET_HIT_MASK ))
	{
		m_FxLaserBurn->Emit( 50 );
		m_FxLaserBurn->GetEntity()->GetTransform()->SetPosition( hitPos );		

		if (hit->GetComponent<Health>() && m_DamageTimer >= m_DamageInterval)
		{
			m_DamageTimer = 0.0f;
			hg::Entity* particle = hg::Entity::Assemble( SID( FX_BulletImpact_Metal ) );
			particle->GetTransform()->SetPosition( hitPos );

			DamageMessage dmg( kDmgType_Bullet );
			hit->SendMsg( &dmg );

		}
	}

	return IBehavior::kRUNNING;
}

Hourglass::IBehavior* AI_LaserChase::MakeCopy() const
{
	AI_LaserChase* copy = (AI_LaserChase*)IBehavior::Create( SID(AI_LaserChase) );
	copy->m_Duration = m_Duration;
	copy->m_DamageInterval = m_DamageInterval;
	return copy;
}

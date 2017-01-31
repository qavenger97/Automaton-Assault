#include "GamePch.h"

#include "AI_ToggleShield.h"
#include "UnitShield.h"
#include "HealthModule.h"

void AI_ToggleShield::LoadFromXML( tinyxml2::XMLElement* data )
{
	bool shieldOn;
	data->QueryBoolAttribute( "shield_on", &shieldOn );
	m_ShieldOn = shieldOn;
	m_Health = nullptr;
	m_UnitShield = nullptr;
}

void AI_ToggleShield::Init( Hourglass::Entity* entity )
{
	if (m_Health == nullptr)
	{
		m_Health = entity->GetComponent<Health>();
	}
	if (m_UnitShield == nullptr)
	{
		m_UnitShield = entity->GetComponent<UnitShield>();
	}
}

Hourglass::IBehavior::Result AI_ToggleShield::Update( Hourglass::Entity* entity )
{
	m_Health->SetInvulnerable( m_ShieldOn != 0 );
	m_UnitShield->SetEnabled( m_ShieldOn != 0 );

	unsigned int seconds = unsigned int(hg::g_Time.Elapsed());

	return IBehavior::kSUCCESS;
}

Hourglass::IBehavior* AI_ToggleShield::MakeCopy() const
{
	AI_ToggleShield* copy = (AI_ToggleShield*)IBehavior::Create( SID(AI_ToggleShield) );
	copy->m_Health = nullptr;
	copy->m_UnitShield = nullptr;
	copy->m_ShieldOn = m_ShieldOn;
	return copy;
}

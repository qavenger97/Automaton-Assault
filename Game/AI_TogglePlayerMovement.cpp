#include "GamePch.h"

#include "AI_TogglePlayerMovement.h"
#include "PlayerComp.h"

void AI_TogglePlayerMovement::LoadFromXML( tinyxml2::XMLElement* data )
{
	bool movementOn;
	data->QueryBoolAttribute( "movement_on", &movementOn );
	m_PlayerMovementOn = movementOn;
	m_PlayerComp = nullptr;
}

void AI_TogglePlayerMovement::Init(hg::Entity* entity)
{
	if (m_PlayerComp == nullptr)
	{
		m_PlayerComp = Hourglass::Entity::FindByTag( SID( Player ) )->GetComponent<PlayerComp>();
	}
}

Hourglass::IBehavior::Result AI_TogglePlayerMovement::Update( Hourglass::Entity* entity )
{
	m_PlayerComp->SetMovementLock( m_PlayerMovementOn == 0 );
	if (m_PlayerMovementOn)
	{
		m_PlayerComp->GetEntity()->GetComponent<hg::Motor>()->SetEnabled(false);
	}
	return IBehavior::kSUCCESS;
}

Hourglass::IBehavior* AI_TogglePlayerMovement::MakeCopy() const
{
	AI_TogglePlayerMovement* copy = (AI_TogglePlayerMovement*)IBehavior::Create( SID(AI_TogglePlayerMovement) );
	
	copy->m_PlayerMovementOn = m_PlayerMovementOn;
	copy->m_PlayerComp = m_PlayerComp;

	return copy;
}

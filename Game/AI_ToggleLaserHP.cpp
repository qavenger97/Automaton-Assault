#include "GamePch.h"

#include "AI_ToggleLaserHP.h"

void AI_ToggleLaserHP::LoadFromXML( tinyxml2::XMLElement* data )
{
	bool on = false;
	data->QueryBoolAttribute( "on", &on );
	m_On = on;
}

Hourglass::IBehavior::Result AI_ToggleLaserHP::Update( Hourglass::Entity* entity )
{
	hg::Entity::FindByName( SID( LaserBossHitBox ) )->GetComponent<hg::DynamicCollider>()->SetEnabled( bool(m_On) );
	return IBehavior::kSUCCESS;
}

Hourglass::IBehavior* AI_ToggleLaserHP::MakeCopy() const
{
	AI_ToggleLaserHP* copy = (AI_ToggleLaserHP*)IBehavior::Create( SID(AI_ToggleLaserHP) );
	copy->m_On = m_On;
	return copy;
}

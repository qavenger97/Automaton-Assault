#include "GamePch.h"

#include "AI_Teleport.h"

void AI_Teleport::LoadFromXML( tinyxml2::XMLElement* data )
{
	data->QueryFloatAttribute( "x", &m_TeleportPos.x );
	data->QueryFloatAttribute( "y", &m_TeleportPos.y );
	data->QueryFloatAttribute( "z", &m_TeleportPos.z );
}

Hourglass::IBehavior::Result AI_Teleport::Update( Hourglass::Entity* entity )
{
	hg::Entity::FindByName( SID( LaserWurmBoss ) )->GetTransform()->SetPosition( m_TeleportPos );
	return IBehavior::kSUCCESS;
}

Hourglass::IBehavior* AI_Teleport::MakeCopy() const
{
	AI_Teleport* copy = (AI_Teleport*)IBehavior::Create( SID(AI_Teleport) );
	copy->m_TeleportPos = m_TeleportPos;
	return copy;
}

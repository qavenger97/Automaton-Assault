#include "GamePch.h"

#include "AI_IsPlayerAlone.h"
#include "EntityGroup.h"

void AI_IsPlayerAlone::LoadFromXML( tinyxml2::XMLElement* data )
{
	const char* targetTag = data->Attribute( "target_tag" );

	if (targetTag)
		m_TargetTag = WSID( targetTag );
}

bool AI_IsPlayerAlone::Check( Hourglass::Entity* entity )
{
	return hg::Entity::FindByName( m_TargetTag )->GetComponent<EntityGroup>()->IsOneOnOne();
}

hg::IBehavior* AI_IsPlayerAlone::MakeCopy() const
{
	AI_IsPlayerAlone* copy = (AI_IsPlayerAlone*)IBehavior::Create( SID(AI_IsPlayerAlone) );
	copy->m_TargetTag = m_TargetTag;

	return copy;
}

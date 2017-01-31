#include "GamePch.h"

#include "AI_EnemyGrouping.h"
#include "EntityGroup.h"

void AI_EnemyGrouping::LoadFromXML( tinyxml2::XMLElement* data )
{
	const char* targetTag = data->Attribute( "target_tag" );

	if (targetTag)
		m_TargetTag = WSID( targetTag );

	m_Policy = kRemove;
	const char* policy = data->Attribute( "policy" );
	if (strcmp( policy, "add" ) == 0)
	{
		m_Policy = kAdd;
	}
}

void AI_EnemyGrouping::Init( Hourglass::Entity* entity)
{
	EntityGroup* eg = hg::Entity::FindByName(m_TargetTag)->GetComponent<EntityGroup>(); 

	if (eg)
	{
		if (m_Policy == kAdd)
		{
			eg->AddEntity( entity );
		}
		else
		{
			eg->RemoveEntity( entity );
		}
	}
}

Hourglass::IBehavior::Result AI_EnemyGrouping::Update( Hourglass::Entity* entity )
{
	return kSUCCESS;
}

Hourglass::IBehavior* AI_EnemyGrouping::MakeCopy() const
{
	AI_EnemyGrouping* copy = (AI_EnemyGrouping*)IBehavior::Create( SID(AI_EnemyGrouping) );
	copy->m_TargetTag = m_TargetTag;
	copy->m_Policy = m_Policy;

	return copy;
}
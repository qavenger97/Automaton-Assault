#include "GamePch.h"

#include "AI_AlignBossHead.h"

void AI_AlignBossHead::LoadFromXML( tinyxml2::XMLElement* data )
{
	m_Duration = 1.0f;
	data->QueryFloatAttribute( "duration", &m_Duration );
}

void AI_AlignBossHead::Init( hg::Entity* entity )
{
	m_BossHead = hg::Entity::FindByName( SID( boss_head_inner ) )->GetTransform();
	hg::Transform* bossHeadOuter = hg::Entity::FindByName( SID( boss_head ) )->GetTransform();
	m_TurnToPoint = bossHeadOuter->GetWorldPosition() + bossHeadOuter->Forward();
	m_Timer = 0.0f;
}

Hourglass::IBehavior::Result AI_AlignBossHead::Update( Hourglass::Entity* entity )
{
	float dt = hg::g_Time.Delta();
	m_Timer += dt;
	m_BossHead->TurnTo( m_TurnToPoint, dt );
	return (m_Timer >= m_Duration) ? IBehavior::kSUCCESS : IBehavior::kRUNNING;
}

Hourglass::IBehavior* AI_AlignBossHead::MakeCopy() const
{
	AI_AlignBossHead* copy = (AI_AlignBossHead*)IBehavior::Create( SID(AI_AlignBossHead) );
	copy->m_Duration = m_Duration;
	return copy;
}

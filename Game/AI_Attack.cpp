#include "GamePch.h"

#include "AI_Attack.h"

void AI_Attack::LoadFromXML( tinyxml2::XMLElement* data )
{
	const char* targetTag = data->Attribute( "target_tag" );

	if (targetTag)
		m_TargetTag = WSID( targetTag );

	m_AttackInterval = 1.0f;
	data->QueryFloatAttribute( "attack_interval", &m_AttackInterval );
}

void AI_Attack::Start()
{
	m_Target = Hourglass::Entity::FindByTag( m_TargetTag )->GetTransform();
	m_NextAttackTime = hg::g_Time.GameElapsed();
}

Hourglass::IBehavior::Result AI_Attack::Update( Hourglass::Entity* entity )
{
	if (!m_Target || !entity)
		return IBehavior::kFAILURE;

	if (hg::g_Time.GameElapsed() >= m_NextAttackTime)
	{
		m_AttackStartTime = hg::g_Time.GameElapsed();
		m_NextAttackTime = hg::g_Time.GameElapsed() + m_AttackInterval;
	}

	// Test code to visualize attack behavior
	float t = 1.0f - (m_AttackStartTime + 0.5f - hg::g_Time.GameElapsed()) / 0.5f;
	if (t >= 0.0f && t <= 1.0f)
	{
		hg::DebugRenderer::DrawSphere( entity->GetTransform()->GetWorldPosition(), t * 2.0f, Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
	}

	return IBehavior::kRUNNING;
}

Hourglass::IBehavior* AI_Attack::MakeCopy() const
{
	AI_Attack* copy = (AI_Attack*)IBehavior::Create( SID( AI_Attack ) );

	copy->m_TargetTag = m_TargetTag;
	copy->m_Target = m_Target;
	copy->m_AttackInterval = m_AttackInterval;

	return copy;
}

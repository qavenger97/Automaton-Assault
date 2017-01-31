#include "GamePch.h"

#include "AI_IsInAtkRange.h"

void AI_IsInAtkRange::LoadFromXML( tinyxml2::XMLElement * data )
{
	const char* targetTag = data->Attribute( "target_tag" );

	if (targetTag)
		m_TargetTag = WSID( targetTag );

	m_Range = 1.0f;
	data->QueryFloatAttribute( "attack_interval", &m_Range );
}

void AI_IsInAtkRange::Start()
{
	m_Target = Hourglass::Entity::FindByTag( m_TargetTag )->GetTransform();
}

bool AI_IsInAtkRange::Check( Hourglass::Entity * entity )
{
	XMVECTOR vToTarget = m_Target->GetWorldPosition() - entity->GetTransform()->GetWorldPosition();
	vToTarget = XMVectorSetY( vToTarget, 0.0f );
	float lenSq;
	XMStoreFloat( &lenSq, XMVector3LengthSq( vToTarget ) );

	return (lenSq > m_Range * m_Range);
}

hg::IBehavior * AI_IsInAtkRange::MakeCopy() const
{
	AI_IsInAtkRange* copy = (AI_IsInAtkRange*)IBehavior::Create( SID( AI_IsInAtkRange ) );

	copy->m_TargetTag = m_TargetTag;
	copy->m_Target = m_Target;
	copy->m_Range = m_Range;

	return copy;
}

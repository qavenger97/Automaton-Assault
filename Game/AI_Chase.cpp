#include "GamePch.h"

#include "AI_Chase.h"

void AI_Chase::LoadFromXML( tinyxml2::XMLElement* data )
{
	const char* targetTag = data->Attribute( "target_tag" );

	if (targetTag)
		m_TargetTag = WSID( targetTag );

	m_Speed = 1.0f;
	data->QueryFloatAttribute( "speed", &m_Speed );
}

void AI_Chase::Start()
{
	m_Target = Hourglass::Entity::FindByTag( m_TargetTag )->GetTransform();
}

Hourglass::IBehavior::Result AI_Chase::Update( Hourglass::Entity* entity )
{
	if (!m_Target || !entity)
	{
		return IBehavior::kFAILURE;
	}

	const XMVECTOR& worldPos = m_Target->GetWorldPosition();
	XMVECTOR toTarget = worldPos - entity->GetTransform()->GetWorldPosition();	

	hg::Ray ray( worldPos, toTarget, 20.0f );
	Vector3 hit;
	hg::Entity* hitEntity;

	if (hg::g_Physics.RayCast( ray, &hitEntity, &hit ))
	{
		hg::DebugRenderer::DrawLine( entity->GetTransform()->GetWorldPosition(), hitEntity->GetTransform()->GetWorldPosition() );

		if (hitEntity->GetTag() != m_TargetTag)
		{
			return kSUCCESS;
		}
	}

	// Only move when dynamic collider is available
	hg::DynamicCollider* col = entity->GetComponent<hg::DynamicCollider>();
	if (!col)
	{
		return IBehavior::kFAILURE;
	}

	// Project movement onto XZ plane
	XMVECTOR toTargetXZ = XMVectorSetY( toTarget, 0.0f );

	Vector3 moveVec;
	XMStoreFloat3( &moveVec, XMVector3Normalize( toTargetXZ ) * m_Speed * hg::g_Time.Delta() );
	col->Move( moveVec );

	return IBehavior::kRUNNING;
}

Hourglass::IBehavior* AI_Chase::MakeCopy() const
{
	AI_Chase* copy = (AI_Chase*)IBehavior::Create( SID( AI_Chase ) );
	copy->m_Speed = m_Speed;
	copy->m_TargetTag = m_TargetTag;
	copy->m_Target = m_Target;

	return copy;
}

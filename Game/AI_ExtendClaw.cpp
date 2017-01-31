#include "GamePch.h"

#include "AI_ExtendClaw.h"
#include "IKComponent.h"

void AI_ExtendClaw::LoadFromXML( tinyxml2::XMLElement* data )
{
	const char* targetTag = data->Attribute( "target_tag" );

	if (targetTag)
		m_TargetTag = WSID( targetTag );

	m_ExtendDuration = 2.0f;
	data->QueryFloatAttribute( "extend_duration", &m_ExtendDuration );
}

void AI_ExtendClaw::Init( Hourglass::Entity* entity )
{
	if (m_Trans == nullptr)
	{
		m_Trans = entity->GetTransform()->GetChild(0);
		m_Trans->GetEntity()->GetComponent<hg::MeshRenderer>()->SetEnabled(false);
	}

	if (m_Target == nullptr)
	{
		m_Target = hg::Entity::FindByName(SID(Player))->GetTransform();
	}

	m_Trans->UnLink(true);


	if (m_IK == nullptr)
	{
		m_IK = entity->GetComponent<IKComponent>();
		m_IK->SetTarget( m_Trans->GetEntity() );
		m_IK->SetSource( entity );
		m_IK->SetFrozen( false );
	}

	m_Timer = 0.0f;

	XMStoreFloat3(&m_StartPos, m_Trans->GetWorldPosition());

	hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_GRAPPLER_EXTENDCLAW, entity->GetPosition());
}

Hourglass::IBehavior::Result AI_ExtendClaw::Update(Hourglass::Entity* entity)
{
	m_Timer += hg::g_Time.Delta();

	IBehavior::Result result;

	if (m_Timer < m_ExtendDuration)
	{
		Vector3 newPos = Vector3::Lerp( m_StartPos, m_Target->GetWorldPosition(), m_Timer / m_ExtendDuration );
		m_Trans->SetPosition( newPos );
		result = IBehavior::kRUNNING;
	}
	else
	{
		m_Timer = m_ExtendDuration;
		m_IK->SetReversed(true);
		m_Trans->LinkTo( entity->GetTransform() );
		m_Trans->SetPosition( XMLoadFloat3( &m_StartPos ) );
		m_Trans->GetEntity()->GetComponent<hg::MeshRenderer>()->SetEnabled( true );

		hg::g_AudioSystem.PostEvent(AK::EVENTS::STOP_GRAPPLER_EXTENDCLAW);
		hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_GRAPPLER_HOOKED);
		result = IBehavior::kSUCCESS;
	}
	

	if (m_IK->IsEmpty())
	{
		m_IK->AddFront( m_IK->GetLength() );
	}
	else
	{
		const XMVECTOR& frontPos = m_IK->GetFrontPosition();

		XMVECTOR V = XMVectorSubtract( frontPos, XMLoadFloat3(&m_StartPos) );
		XMVECTOR X = XMVector3Length( V );
		float dist = XMVectorGetX( X );
		float len = m_IK->GetLength();
		if (dist >= len)
		{
			dist -= len;
			m_IK->AddFront( len );
		}
	}

	return result;
}

Hourglass::IBehavior* AI_ExtendClaw::MakeCopy() const
{
	AI_ExtendClaw* copy = (AI_ExtendClaw*)IBehavior::Create( SID(AI_ExtendClaw) );
	copy->m_ExtendDuration = m_ExtendDuration;
	copy->m_TargetTag = m_TargetTag;
	copy->m_Trans = nullptr;
	copy->m_Target = nullptr;
	copy->m_IK = nullptr;
	
	return copy;
}

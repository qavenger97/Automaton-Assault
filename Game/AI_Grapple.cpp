#include "GamePch.h"

#include "AI_Grapple.h"
#include "PlayerComp.h"
#include "IKComponent.h"
#include "GrapplerComp.h"

const float AI_Grapple::kFallHeight = 0.97f;

void AI_Grapple::LoadFromXML( tinyxml2::XMLElement* data )
{
	const char* targetTag = data->Attribute( "target_tag" );

	if (targetTag)
		m_TargetTag = WSID( targetTag );

	m_RetractSpeed = 3.5f;
	data->QueryFloatAttribute( "retract_speed", &m_RetractSpeed );

	m_RetractDuration = 1.0f / m_RetractSpeed;

	StrID m_TargetTag;
	m_Target = nullptr;
}

void AI_Grapple::Init( Hourglass::Entity * entity )
{
	if(!m_Target)
		m_Target = Hourglass::Entity::FindByTag( m_TargetTag )->GetTransform();

	m_Target->GetEntity()->GetComponent<PlayerComp>()->SetMovementLock( true );

	if (m_IK == nullptr)
	{
		m_IK = entity->GetComponent<IKComponent>();
	}

	m_Target->UnLink(true);

	m_StartPos = entity->GetComponent<GrapplerComp>()->GetGrappleStartPos();
	m_RetractTimer = 0.0f;

	hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_GRAPPLER_REELIN, entity->GetPosition());
}

Hourglass::IBehavior::Result AI_Grapple::Update( Hourglass::Entity* entity )
{
	Vector3 playerPos = m_Target->GetWorldPosition();	

	if (playerPos.y >= kFallHeight)
	{
		m_Target->MoveTo( m_IK->GetBackPosition(), min( hg::g_Time.Delta() * m_RetractSpeed, 1.0f ) );
	}

	if (m_IK->IsEmpty())
	{
		m_Target->GetEntity()->GetComponent<PlayerComp>()->SetMovementLock( false );
		hg::g_AudioSystem.PostEvent(AK::EVENTS::STOP_GRAPPLER_REELIN);
		return IBehavior::kSUCCESS;
	}
	
	m_RetractTimer += hg::g_Time.Delta();

	if(m_RetractDuration <= m_RetractTimer)
	{ 
		m_RetractTimer = 0.0f;
		m_IK->RemoveFront();
	}

	return IBehavior::kRUNNING;
}

Hourglass::IBehavior* AI_Grapple::MakeCopy() const
{
	AI_Grapple* copy = (AI_Grapple*)IBehavior::Create( SID(AI_Grapple) );
	copy->m_TargetTag = m_TargetTag;
	copy->m_RetractSpeed = m_RetractSpeed;
	copy->m_RetractDuration = m_RetractDuration;

	return copy;
}

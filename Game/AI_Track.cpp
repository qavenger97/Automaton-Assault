#include "GamePch.h"

#include "AI_Track.h"

void AI_Track::LoadFromXML( tinyxml2::XMLElement* data )
{
	const char* targetTag = data->Attribute( "target_tag" );

	if (targetTag)
	{
		m_TargetTag = WSID( targetTag );
	}

	m_Speed = 1.0f;
	data->QueryFloatAttribute( "speed", &m_Speed );

	m_RepathFreq = 1.0f;
	data->QueryFloatAttribute( "repath_freq", &m_RepathFreq );

	m_WaypointAgent = nullptr;
}

void AI_Track::Start()
{
	m_Target = Hourglass::Entity::FindByTag( m_TargetTag );	
}

void AI_Track::Init( Hourglass::Entity* entity )
{	
	m_WaypointAgent = entity->GetComponent<hg::WaypointAgent>();
	XMVECTOR targetPos = m_Target->GetTransform()->GetWorldPosition();
	m_WaypointAgent->SetDestination( targetPos );
	m_Timer = m_RepathFreq;
}

hg::IBehavior::Result AI_Track::Update( Hourglass::Entity * entity )
{
	if (!m_Target || !entity)
	{
		return IBehavior::kFAILURE;
	}

	XMVECTOR targetPos = m_Target->GetTransform()->GetWorldPosition();
	XMVECTOR worldPos = entity->GetTransform()->GetWorldPosition();
	XMVECTOR targetDir = targetPos - worldPos;

	float dt = hg::g_Time.Delta();
	m_Timer += dt;
	if (m_Timer >= m_RepathFreq)
	{
		m_Timer = 0.0f;
		m_WaypointAgent->SetDestination( targetPos );
	}

	Vector3 moveVec; // we will move one way or another

	// Only move when dynamic collider is available
	hg::DynamicCollider* col = entity->GetComponent<hg::DynamicCollider>();
	if (!col)
	{
		return IBehavior::kFAILURE;
	}

	hg::Ray ray( worldPos, targetDir, 20.0f );
	Vector3 hit;
	hg::Entity* hitEntity;

	Vector3 desiredDirection = m_WaypointAgent->GetDesiredDirection();
	desiredDirection.Normalize();
	XMStoreFloat3( &moveVec, desiredDirection * m_Speed * dt );

	if (hg::g_Physics.RayCast( ray, &hitEntity, &hit ))
	{
		hg::DebugRenderer::DrawLine( entity->GetTransform()->GetWorldPosition(), hitEntity->GetTransform()->GetWorldPosition() );

		if (hitEntity == m_Target)
		{
			return kSUCCESS;
		}
	}

	col->Move( moveVec );

	return IBehavior::kRUNNING;
}

hg::IBehavior* AI_Track::MakeCopy() const
{
	AI_Track* copy = (AI_Track*)IBehavior::Create( SID( AI_Track ) );
	copy->m_Speed = m_Speed;
	copy->m_TargetTag = m_TargetTag;
	copy->m_Target = m_Target;
	copy->m_RepathFreq = m_RepathFreq;
	copy->m_Timer = m_Timer;

	return copy;
}

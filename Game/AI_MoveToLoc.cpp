#include "GamePch.h"

#include "AI_MoveToLoc.h"
#include "GrapplerComp.h"

void AI_MoveToLoc::LoadFromXML( tinyxml2::XMLElement* data )
{
	m_Trans = nullptr;

	m_CompletionToleranceSq = 0.3f;
	data->QueryFloatAttribute( "completion_tolerance", &m_CompletionToleranceSq );
	m_CompletionToleranceSq *= m_CompletionToleranceSq;
}

void AI_MoveToLoc::Init( Hourglass::Entity* entity )
{
	if (m_Trans == nullptr)
	{
		m_Trans = entity->GetTransform();
	}

	if (m_Motor == nullptr)
	{
		m_Motor = entity->GetComponent<hg::Motor>();
	}

	m_Motor->SetMoveEnabled( true );

	m_TargetLocation = entity->GetComponent<GrapplerComp>()->GetRadialPatrolStartPoint();
	m_TargetLocation.y = 0.0f;
}

Hourglass::IBehavior::Result AI_MoveToLoc::Update( Hourglass::Entity* entity )
{
	Vector3 pos = m_Trans->GetWorldPosition();
	pos.y = 0.0f;

	Vector3 toVector = m_TargetLocation - pos;

	if (toVector.LengthSquared() > m_CompletionToleranceSq)
	{
		toVector.Normalize();
		m_Motor->SetDesiredMove( toVector );
		m_Motor->SetDesiredForward( toVector );

		return IBehavior::kRUNNING;
	}
	else
	{
		m_Motor->SetMoveEnabled( false );

		return IBehavior::kSUCCESS;
	}
	
}

Hourglass::IBehavior* AI_MoveToLoc::MakeCopy() const
{
	AI_MoveToLoc* copy = (AI_MoveToLoc*)IBehavior::Create( SID(AI_MoveToLoc) );

	copy->m_Trans = nullptr;
	copy->m_Motor = nullptr;
	copy->m_CompletionToleranceSq = m_CompletionToleranceSq;

	return copy;
}

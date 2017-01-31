#include "GamePch.h"

#include "Chase.h"
#include "Common.h"
#include "Component\Separation.h"
#include "IKObject.h"
#include "IKComponent.h"
#include "GameUtils.h"

extern bool g_bDebugAIBehaviors;
static bool s_bDrawDebugAIChasingInfo = false;

void Chase::LoadFromXML(tinyxml2::XMLElement* data)
{
	const char* targetTag = data->Attribute("target_tag");

	if (targetTag)
		m_TargetTag = WSID(targetTag);

	m_StopDistance = 4.0f;
	data->QueryFloatAttribute("stop_dist", &m_StopDistance);

	m_Speed = 1.0f;
	data->QueryFloatAttribute("speed", &m_Speed);

	m_RepathFreq = 1.0f;
	data->QueryFloatAttribute( "repath_freq", &m_RepathFreq );

	const char* runAnim = data->Attribute( "run_anim" );
	m_Run = WSID(runAnim);

	m_AnimSpeed = 1.0f;
	data->QueryFloatAttribute( "anim_speed", &m_AnimSpeed );

	m_WaypointAgent = nullptr;
	m_Motor = nullptr;
	m_Separation = nullptr;
}

void Chase::Init(Hourglass::Entity* entity)
{
	m_Target = Hourglass::Entity::FindByTag( m_TargetTag );
	m_WaypointAgent = entity->GetComponent<hg::WaypointAgent>();

	XMVECTOR targetPos = m_Target->GetTransform()->GetWorldPosition();
	XMVECTOR worldPos = entity->GetTransform()->GetWorldPosition();
	XMVECTOR targetDir;
	XMVECTOR vToTarget = targetDir = targetPos - worldPos;
	vToTarget = XMVectorSetY( vToTarget, 0.0f );

	hg::Ray ray( Vector3( worldPos ) + Vector3::Up, targetDir, 20.0f );
	Vector3 hit;
	hg::Entity* hitEntity;
	if (hg::g_Physics.RayCast( ray, &hitEntity, &hit, nullptr, ~COLLISION_ENEMY_MASK ))
	{
		m_Pursuit = (hitEntity == m_Target);
	}

	if (!m_Pursuit)
	{
		m_WaypointAgent->Continue();
		m_WaypointAgent->SetDestination( targetPos );		
	}
	else
	{
		m_WaypointAgent->Cease();
	}	
	
	m_Timer = 4.0f;

	if (m_Motor == nullptr)
	{
		m_Motor = entity->GetComponent<hg::Motor>();
	}

	if (m_Motor)
	{
		m_Motor->SetMoveEnabled( true );
		m_Motor->SetEnabled( true );
	}

	if (m_Separation == nullptr)
	{
		m_Separation = entity->GetComponent<hg::Separation>();
	}

	hg::Animation* anim = entity->GetComponent<hg::Animation>();
	if (anim)
		anim->Play( m_Run, m_AnimSpeed );
}

Hourglass::IBehavior::Result Chase::Update(Hourglass::Entity* entity)
{
	if (g_bDebugAIBehaviors)
	{
		hg::DevTextRenderer::DrawText_WorldSpace("Chase", entity->GetPosition());

		Vector3 pos = entity->GetTransform()->GetWorldPosition();
		Vector3 v1 = entity->GetTransform()->Right();
		Vector3 v2 = entity->GetTransform()->Up();
		Vector3 v3 = entity->GetTransform()->Forward();
		hg::DebugRenderer::DrawLine(pos, pos + v1, Vector4(1, 0, 0, 1), Vector4(1, 0, 0, 1));
		hg::DebugRenderer::DrawLine(pos, pos + v2, Vector4(0, 1, 0, 1), Vector4(0, 1, 0, 1));
		hg::DebugRenderer::DrawLine(pos, pos + v3, Vector4(0, 0, 1, 1), Vector4(0, 0, 1, 1));
	}

	if (!m_Target || !entity)
		return IBehavior::kFAILURE;

	XMVECTOR targetPos = m_Target->GetTransform()->GetWorldPosition();
	XMVECTOR worldPos = entity->GetTransform()->GetWorldPosition();
	XMVECTOR targetDir;
	XMVECTOR vToTarget = targetDir = targetPos - worldPos;
	vToTarget = XMVectorSetY(vToTarget, 0.0f);
	float lenSq;
	XMStoreFloat(&lenSq, XMVector3LengthSq(vToTarget));

	if (lenSq <= m_StopDistance * m_StopDistance && GameUtils::AI_CanSeePlayer(entity, m_Target, Vector3(0, 1, 0)))
	{
		m_Motor->SetMoveEnabled( false );
		return IBehavior::kSUCCESS;
	}

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
		return IBehavior::kFAILURE;

	//if(m_Target->GetTag() == SID(;

	hg::Ray ray( Vector3(worldPos) + Vector3::Up, targetDir, 20.0f );
	Vector3 hit;
	hg::Entity* hitEntity;
	bool hitPlayer = false;

	if (s_bDrawDebugAIChasingInfo)
		hg::DebugRenderer::DrawLine(worldPos, worldPos + XMVector3Normalize(vToTarget) * 20.0f, Vector4(1.0f, 0.0f, 0.0f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f));

	if (hg::g_Physics.RayCast( ray, &hitEntity, &hit, nullptr, ~COLLISION_ENEMY_MASK ))
	{
		if (s_bDrawDebugAIChasingInfo)
			hg::DebugRenderer::DrawLine(entity->GetTransform()->GetWorldPosition(), hitEntity->GetTransform()->GetWorldPosition());

		if (hitEntity == m_Target)
		{
			Vector3 plannarDir = ray.Direction;
			plannarDir.y = 0.0f;
			//plannarDir.Normalize();
			XMStoreFloat3( &moveVec, plannarDir * m_Speed * dt );
			hitPlayer = true;

			if (!m_Pursuit)
			{
				m_WaypointAgent->Cease();
			}

			m_Pursuit = true;
		}
		else 
		{
			if (m_Pursuit)
			{
				m_WaypointAgent->Continue();
			}

			m_Pursuit = false;
		}
	}
	
	if (!hitPlayer)
	{
		Vector3 desiredDirection = m_WaypointAgent->GetDesiredDirection();
		desiredDirection.y = 0.0f;
		//desiredDirection.Normalize();
		XMStoreFloat3( &moveVec, desiredDirection * m_Speed * dt );

		if (m_Motor)
		{
			moveVec.Normalize();
			m_Motor->SetDesiredForward( desiredDirection );

			if (m_Separation)
				m_Separation->SetDesiredMove(moveVec);
			else
				m_Motor->SetDesiredMove(moveVec);
		}
	}
	else if( m_Motor )
	{
		moveVec.Normalize();
		vToTarget = XMVector3Normalize( vToTarget );
		m_Motor->SetDesiredForward( vToTarget );

		if (m_Separation)
			m_Separation->SetDesiredMove(moveVec);
		else
			m_Motor->SetDesiredMove(moveVec);
	}

	return IBehavior::kRUNNING;
}

Hourglass::IBehavior* Chase::MakeCopy() const
{
	Chase* copy = (Chase*)IBehavior::Create(SID(Chase));
	copy->m_Speed = m_Speed;
	copy->m_StopDistance = m_StopDistance;
	copy->m_TargetTag = m_TargetTag;
	copy->m_RepathFreq = m_RepathFreq;
	copy->m_Timer = m_Timer;
	copy->m_Run = m_Run;
	copy->m_Motor = nullptr;
	copy->m_Separation = nullptr;
	copy->m_WaypointAgent = nullptr;
	copy->m_AnimSpeed = m_AnimSpeed;

	return copy;
}

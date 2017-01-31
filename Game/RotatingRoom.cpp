#include "GamePch.h"

#include "RotatingRoom.h"
#include "SpinMotor.h"
#include "GrapplerComp.h"
#include "ShiftingPlatform.h"
#include "RadialGravity.h"

uint32_t RotatingRoom::s_TypeID = hg::ComponentFactory::GetGameComponentID();

void RotatingRoom::LoadFromXML( tinyxml2::XMLElement* data )
{	
	tinyxml2::XMLElement* grapplerSetXML = data->FirstChildElement( "Grapplers" );
	tinyxml2::XMLElement* grapplerXML = grapplerSetXML->FirstChildElement( "Grappler" );

	unsigned int grapplerId = 0;

	while (grapplerXML)
	{
		grapplerXML->QueryFloatAttribute( "x", &m_GrapplerSpawnPoints[grapplerId].x );
		grapplerXML->QueryFloatAttribute( "y", &m_GrapplerSpawnPoints[grapplerId].y );
		grapplerXML->QueryFloatAttribute( "z", &m_GrapplerSpawnPoints[grapplerId].z );

		grapplerXML->QueryFloatAttribute( "patrol_start_x", &m_GrapplerPatrolPos[grapplerId].x );
		grapplerXML->QueryFloatAttribute( "patrol_start_y", &m_GrapplerPatrolPos[grapplerId].y );
		grapplerXML->QueryFloatAttribute( "patrol_start_z", &m_GrapplerPatrolPos[grapplerId].z );	

		++grapplerId;

		grapplerXML = grapplerXML->NextSiblingElement();
	}
}

void RotatingRoom::Init()
{
	m_Timer = 0.0f;
	m_CurrentGrapplerId = 0;
	m_GrapplerStopAngle[0] = -36.0f;
	m_GrapplerStopAngle[1] = -126.0f;
	m_GrapplerStopAngle[2] = -216.0f;
	m_GrapplerStopAngle[3] = -270.0f;
}


void RotatingRoom::Start()
{
	IComponent::SetEnabled( false );
	if (m_SpinMotor == nullptr)
	{
		m_SpinMotor = GetEntity()->GetComponent<SpinMotor>();
	}
	m_CurrentGrapplerId = 0;
	m_Timer = 0.0f;
	m_Timer = 10.0f;
	m_Delay = 10.0f;
}

void RotatingRoom::Update()
{
	if (m_SpinMotor->GetCurrentDegrees() < m_GrapplerStopAngle[m_CurrentGrapplerId])
	{
		m_SpinMotor->StopSpin();
		if (m_CurrentGrapplerId == kNumGrapplers - 1)
		{
			hg::Entity::FindByName( SID( BridgeExit ) )->GetComponent<ShiftingPlatform>()->Extend();
			hg::Entity* ent =
				hg::Entity::FindByName( SID( RadialGravity ) );
			ent->GetComponent<RadialGravity>()->ActivateBridgeExit();
			IComponent::SetEnabled( false );
			
			// Stop battle music
			hg::g_AudioSystem.SetState(AK::STATES::ATMOSPHERE::GROUP, AK::STATES::ATMOSPHERE::STATE::SAFE);

			return;
		}
		SpawnNextGrappler();
		if (++m_CurrentGrapplerId == kNumGrapplers)
		{
			m_CurrentGrapplerId = 0;
		}
		m_Timer = 0.0f;
	}
	else
	{
		m_Timer += hg::g_Time.Delta();
		if (m_Timer >= m_Delay)
		{
			m_SpinMotor->StartSpin();

			hg::g_AudioSystem.SetState(AK::STATES::ATMOSPHERE::GROUP, AK::STATES::ATMOSPHERE::STATE::DANGER);
		}
	}
}

void RotatingRoom::SpawnNextGrappler()
{
	m_Grapplers[m_CurrentGrapplerId] = hg::Entity::Assemble( SID(Grappler) );

	m_Grapplers[m_CurrentGrapplerId]->GetTransform()->
		SetPosition( m_GrapplerSpawnPoints[m_CurrentGrapplerId] );

	GrapplerComp* gc = m_Grapplers[m_CurrentGrapplerId]->GetComponent<GrapplerComp>();
	
	gc->SetRadialPatrolStartPoint( m_GrapplerPatrolPos[m_CurrentGrapplerId] );

	hg::Transform* grapplerTrans = m_Grapplers[m_CurrentGrapplerId]->GetTransform();
	hg::Transform* clawTrans = hg::Entity::Assemble( SID( GrapplerClaw ) )->GetTransform();

	gc->SetGrapple(clawTrans->GetEntity());

	Vector3 clawPos = grapplerTrans->GetWorldPosition();
	clawPos += grapplerTrans->Forward();
	clawTrans->LinkTo(grapplerTrans);
	clawTrans->SetPosition( clawPos );
	
	hg::g_AudioSystem.SetState(AK::STATES::ENEMY::GROUP, AK::STATES::ENEMY::STATE::GRAPPLER);
}

void RotatingRoom::OnEvent(const StrID& eventName)
{
	if (eventName == SID(Shift))
	{
		IComponent::SetEnabled(true);
	}
}

hg::IComponent* RotatingRoom::MakeCopyDerived() const
{
	RotatingRoom* copy = (RotatingRoom*)IComponent::Create( SID(RotatingRoom) );

	// TODO: handle copying of data members (ex: copy->m_data = m_data)

	return copy;
}

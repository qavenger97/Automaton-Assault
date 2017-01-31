#include "GamePch.h"

#include "AI_SpinClaw.h"

void AI_SpinClaw::LoadFromXML( tinyxml2::XMLElement* data )
{
	m_Claw = nullptr;
	m_RotationSpeed = 0.0f;
	m_SpinTime = 3.0f;
	data->QueryFloatAttribute( "spin_time", &m_SpinTime);
	data->QueryFloatAttribute( "rotation_speed", &m_RotationSpeed);
}

void AI_SpinClaw::Init( hg::Entity * entity )
{
	if (m_Claw == nullptr)
	{
		m_Claw = entity->GetTransform()->GetChild( 0 );
	}
	m_Timer = 0.0f;
}

Hourglass::IBehavior::Result AI_SpinClaw::Update( Hourglass::Entity* entity )
{
	float dt = hg::g_Time.Delta();
	m_Timer += dt;
	m_Claw->Rotate( 0.0f, 0.0f, m_RotationSpeed*dt );
	if (m_Timer >= m_SpinTime)
	{
		return IBehavior::kSUCCESS;
	}
	return IBehavior::kRUNNING;
}

Hourglass::IBehavior* AI_SpinClaw::MakeCopy() const
{
	AI_SpinClaw* copy = (AI_SpinClaw*)IBehavior::Create( SID(AI_SpinClaw) );
	
	copy->m_Claw = nullptr;
	copy->m_RotationSpeed = m_RotationSpeed;
	copy->m_SpinTime = m_SpinTime;

	return copy;
}

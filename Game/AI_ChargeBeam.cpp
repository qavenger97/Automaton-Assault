#include "GamePch.h"

#include "AI_ChargeBeam.h"

void AI_ChargeBeam::LoadFromXML( tinyxml2::XMLElement* data )
{
	bool isPoweringDown = false;
	data->QueryBoolAttribute( "state", &isPoweringDown );
	m_PoweringDown = isPoweringDown;

	m_Duration = 5.0f;
	data->QueryFloatAttribute( "duration", &m_Duration );

	m_StartRadius = 0.0f;
	data->QueryFloatAttribute( "start_radius", &m_StartRadius );

	m_EndRadius = 1000.0f;
	data->QueryFloatAttribute( "end_radius", &m_EndRadius );

	m_StartIntensity = 0.0f;
	data->QueryFloatAttribute( "start_intensity", &m_StartIntensity );

	m_EndIntensity = 100.0f;
	data->QueryFloatAttribute( "end_intensity", &m_EndIntensity );
}

void AI_ChargeBeam::Init( Hourglass::Entity* entity )
{
	hg::Entity* lightEnt = hg::Entity::FindByName(SID(WurmChargingLight));
	if (lightEnt)
		m_ChargingLight = lightEnt->GetComponent<hg::Light>();

	m_Timer = 0.0f;
}

Hourglass::IBehavior::Result AI_ChargeBeam::Update( Hourglass::Entity* entity )
{
	m_Timer += hg::g_Time.Delta();
	if (m_Timer >= m_Duration)
		return IBehavior::kSUCCESS;

	float r = m_Timer / m_Duration;
	m_ChargingLight->SetRadius( Math::Lerp( m_StartRadius, m_EndRadius, r ) );
	m_ChargingLight->SetIntensity( Math::Lerp( m_StartIntensity, m_EndIntensity, r ) );

	return IBehavior::kRUNNING;
}

Hourglass::IBehavior* AI_ChargeBeam::MakeCopy() const
{
	AI_ChargeBeam* copy = (AI_ChargeBeam*)IBehavior::Create( SID(AI_ChargeBeam) );
	copy->m_PoweringDown = m_PoweringDown;
	copy->m_Duration = m_Duration;
	copy->m_StartRadius = m_StartRadius;
	copy->m_EndRadius = m_EndRadius;
	copy->m_StartIntensity = m_StartIntensity;
	copy->m_EndIntensity = m_EndIntensity;
	return copy;
}

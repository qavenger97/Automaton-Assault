#pragma once

class AI_ChargeBeam : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	void AI_ChargeBeam::Init( Hourglass::Entity* entity );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	uint32_t m_PoweringDown : 1;
	hg::Light* m_ChargingLight;
	float m_Duration;
	float m_StartIntensity;
	float m_EndIntensity;
	float m_StartRadius;
	float m_EndRadius;
	float m_Timer;
};
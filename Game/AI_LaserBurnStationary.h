#pragma once

class AI_LaserBurnStationary : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	void Init( Hourglass::Entity* entity );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	hg::Transform* m_Laser;
	hg::Entity* m_FxLaserBurn;
	float m_Timer;

	float m_Duration;
};
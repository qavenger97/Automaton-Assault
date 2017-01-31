#pragma once

class AI_SpinClaw : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	void Init( hg::Entity* entity);

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	hg::Transform* m_Claw;
	float m_Timer;
	float m_RotationSpeed;
	float m_SpinTime;
};
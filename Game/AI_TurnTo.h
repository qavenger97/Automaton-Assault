#pragma once

class AI_TurnTo : public Hourglass::IAction
{
public:

	void LoadFromXML( tinyxml2::XMLElement* data );
	void AI_TurnTo::Init( Hourglass::Entity* entity );
	IBehavior::Result Update( Hourglass::Entity* entity );
	IBehavior* MakeCopy() const;

private:

	hg::Transform* m_Player;
	hg::Transform* m_BossHead;
	hg::Transform* m_Laser;
	float m_Acceleration;
	float m_Velocity;
	float m_InitialVelocity;
};
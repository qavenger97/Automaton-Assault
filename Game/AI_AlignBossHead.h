#pragma once

class AI_AlignBossHead : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	void Init( hg::Entity* entity );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	float m_Timer;
	float m_Duration;
	Vector3 m_TurnToPoint;
	hg::Transform* m_BossHead;
};
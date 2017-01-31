#pragma once

class PlayerComp;
class GrapplerComp;

/*
Tri Segments:
1. 0 - 2.0944
2. 2.0945 - 4.1887
3. 4.1888 - 6.2832
*/

class AI_RadialPatrol : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	void Start();

	void Init( Hourglass::Entity* entity );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:
	
	hg::Motor* m_Motor;
	hg::Transform* m_Player;
	PlayerComp* m_PlayerComp;
	float m_Timer;
	float m_IntervalTimer;
	uint32_t m_MovingRight : 1;
	float m_PatrolDuration;
	float m_PatrolRadius;
	float m_PatrolSpeed;
	Vector3 m_Center;
};
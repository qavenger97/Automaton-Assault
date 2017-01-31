#pragma once

class AI_Rise : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	void Init( Hourglass::Entity* entity );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	StrID m_LaserEntityName;
	hg::Entity* m_LaserEntity;
	Vector3 m_TravelVec;
	float m_StartSpeed;
	float m_EndSpeed;
	float m_Duration;
	float m_Timer;
	uint32_t m_Reverse : 1;
};
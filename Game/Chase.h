#pragma once

namespace Hourglass
{
	class WaypointAgent;
	class Separation;
}

class Chase : public Hourglass::IAction
{
public:
	void LoadFromXML(tinyxml2::XMLElement* data);

	bool IsRunningChild() const { return false; }

	void Init(Hourglass::Entity* entity);

	IBehavior::Result Update(Hourglass::Entity* entity);

	IBehavior* MakeCopy() const;

private:

	StrID m_Run;
	float				m_AnimSpeed;

	uint32_t			m_Pursuit : 1;

	StrID				m_TargetTag;
	hg::Entity*			m_Target;
	hg::Motor*			m_Motor;
	hg::Separation*		m_Separation;
	float				m_Speed;
	float				m_StopDistance;
	hg::WaypointAgent*	m_WaypointAgent;
	float				m_Timer;
	float				m_RepathFreq;
};


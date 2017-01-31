#pragma once

namespace Hourglass
{
	class Entity;
	class WaypointAgent;
}

class AI_Track : public Hourglass::IAction
{
public:

	void LoadFromXML( tinyxml2::XMLElement* data );

	bool IsRunningChild() const { return false; }

	void Start();

	void Init( Hourglass::Entity* entity );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	StrID				m_TargetTag;
	hg::Entity*			m_Target;
	float				m_Speed;
	hg::WaypointAgent*	m_WaypointAgent;
	float				m_Timer;
	float				m_RepathFreq;
};
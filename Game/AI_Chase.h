#pragma once

class AI_Chase : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	bool IsRunningChild() const { return false; }

	void Start();

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:
	StrID			m_TargetTag;
	hg::Transform*	m_Target;
	float			m_Speed;
};


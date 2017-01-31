#pragma once

namespace Hourglass
{
	class Transform;
}

class AI_Attack : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	bool IsRunningChild() const { return false; }

	void Start();

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:
	StrID			m_TargetTag;
	StrID			m_AnimationName;
	hg::Transform*	m_Target;
	float			m_AttackInterval;

	float			m_AttackStartTime;
	float			m_NextAttackTime;
};


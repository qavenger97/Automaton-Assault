#pragma once

class IKComponent;

class AI_ExtendClaw : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	void Init( Hourglass::Entity* entity );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	hg::Transform* m_Trans;
	hg::Transform*	m_Target;
	IKComponent* m_IK;
	XMFLOAT3 m_StartPos;
	StrID m_TargetTag;
	float m_ExtendDuration;
	float m_Timer;
};
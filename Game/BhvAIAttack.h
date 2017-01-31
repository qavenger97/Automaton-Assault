#pragma once

class BhvAIAttack : public Hourglass::IAction
{
public:
	void LoadFromXML(tinyxml2::XMLElement* data);

	bool IsRunningChild() const { return false; }

	void Init(Hourglass::Entity* entity);

	IBehavior::Result Update(Hourglass::Entity* entity);

	IBehavior* MakeCopy() const;

private:
	StrID			m_TargetTag;
	hg::Entity*		m_Target;
	hg::Motor*		m_Motor;
	hg::Animation*  m_Animation;

	enum { kNumAttacks = 3 };
	StrID			m_AttackAnims[kNumAttacks];
	float			m_AttackAnimSpeeds[kNumAttacks];

	float			m_AttackInterval;
	float			m_AttackDistance;

	float			m_AttackStartTime;
	float			m_NextAttackTime;
	bool			m_DamageDealt;
};


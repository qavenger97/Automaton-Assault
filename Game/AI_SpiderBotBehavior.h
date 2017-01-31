#pragma once

class AI_SpiderBotBehavior : public Hourglass::IAction
{
public:
	void LoadFromXML(tinyxml2::XMLElement* data);

	void Init(hg::Entity* entity);

	IBehavior::Result Update(hg::Entity* entity);

	IBehavior* MakeCopy() const;

private:
	// Test if ready to jump
	bool SpiderJump_CheckCondition(hg::Entity* entity) const;

	void SpiderJump_Update(hg::Entity* entity);

	void SpiderChase_Update(hg::Entity* entity);

	bool SpiderAttack_CheckCondition(hg::Entity* entity) const;
	void SpiderAttack_Update(hg::Entity* entity);

	void Attack(hg::Entity* owner);

private:
	//struct Anims
	//{
	//	// full body
	//	StrID idle;
	//	StrID walk;
	//	StrID jump;
	//	StrID bigJump;
	//} m_Anims;

	//hg::Animation* animation = nullptr;
	hg::Entity*		m_Target;
	hg::Animation*	m_Animation;

	bool			m_Deployed;

	float			m_JumpStartTime;
	float			m_JumpTimer;
	Vector3			m_JumpStartPoint;
	Vector3			m_JumpEndPoint;

	float			m_AttackStartTime;
	float			m_AttackTimer;

	float			m_VelocityY;

	float			m_ClickSoundTimer;
	uint8_t			m_State;
};
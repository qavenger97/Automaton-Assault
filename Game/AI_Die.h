#pragma once

namespace Hourglass
{
	class PrimitiveRenderer;
}

class AI_Die : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	void Init( Hourglass::Entity* entity );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	hg::Animation* m_Anim;
	StrID m_DeathAnim;
	Vector3 m_StartScale;
	float m_DeathEndTime;
	float m_Timer;
	float m_AnimPauseTime;
	float m_YAxisStartPos;
	bool  m_ExplodeOnDeath;
	float  m_dropLoot;
};


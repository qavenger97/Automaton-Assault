#pragma once

namespace Hourglass
{
	class MeshRenderer;
}

class Temp_PlayerDie : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	void Init( Hourglass::Entity* entity );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	hg::MeshRenderer* m_Pr;
	float m_DeathEndTime;
	float m_Timer;
	float m_FlickerFreq;
	float m_NextColorSwap;
	float m_YAxisStartPos;
	uint32_t m_IsFlickering;
	Color m_MeshColor;
	Color m_FlickerColor;
};


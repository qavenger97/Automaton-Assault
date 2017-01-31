#pragma once

class AI_IsPlayerAlone : public hg::ICondition
{
public:

	void LoadFromXML( tinyxml2::XMLElement* data );

	bool Check( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	StrID m_TargetTag;
};
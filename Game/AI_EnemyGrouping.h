#pragma once

class AI_EnemyGrouping : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	void Init( Hourglass::Entity* entity );

	Hourglass::IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	enum Policy
	{
		kAdd = 0,
		kRemove = 1
	};

	int32_t m_Policy;
	StrID	m_TargetTag;
};
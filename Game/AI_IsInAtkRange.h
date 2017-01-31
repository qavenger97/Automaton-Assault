#pragma once

namespace Hourglass
{
	class Transform;
}

class AI_IsInAtkRange : public hg::ICondition
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	void Start();

	bool Check( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	StrID				m_TargetTag;
	hg::Transform*		m_Target;
	float				m_Range;
};
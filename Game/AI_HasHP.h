#pragma once

class Health;

class AI_HasHP : public hg::ICondition
{
public:

	bool Check( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	Health* m_Health;
};
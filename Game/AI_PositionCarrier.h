#pragma once

class AI_PositionCarrier : public Hourglass::IAction
{
public:

	void Init( hg::Entity* entity );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;
};
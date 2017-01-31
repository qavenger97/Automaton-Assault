#pragma once

class AI_CarrierSpawnEnemies : public hg::IAction
{
public:
	void Init( hg::Entity* entity ) override;

	hg::IBehavior::Result Update( hg::Entity* entity ) override;

	hg::IBehavior* MakeCopy() const;

};
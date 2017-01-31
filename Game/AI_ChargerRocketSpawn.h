#pragma once

class AI_ChargerRocketSpawn : public Hourglass::IAction
{
public:
	void Start();

	void Init( Hourglass::Entity* entity );

	IBehavior::Result Update(Hourglass::Entity* entity);

	void Shutdown( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;
private:
	bool	m_bHasSpawned;
};
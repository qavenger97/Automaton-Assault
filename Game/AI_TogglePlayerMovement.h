#pragma once

class PlayerComp;

class AI_TogglePlayerMovement : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	void Init( hg::Entity* entity );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	uint32_t m_PlayerMovementOn;
	PlayerComp* m_PlayerComp;
};
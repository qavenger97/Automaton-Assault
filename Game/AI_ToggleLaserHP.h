#pragma once

class AI_ToggleLaserHP : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	uint32_t m_On;
};
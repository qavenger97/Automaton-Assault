#pragma once

class Health;
class UnitShield;

class AI_ToggleShield : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	void Init( Hourglass::Entity* entity );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	uint32_t m_ShieldOn;
	UnitShield* m_UnitShield;
	Health* m_Health;
};
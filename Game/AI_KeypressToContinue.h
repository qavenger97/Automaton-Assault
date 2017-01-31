#pragma once

class AI_KeypressToContinue : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	char m_Key;

};
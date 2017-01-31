#pragma once

class AI_Teleport : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	Vector3 m_TeleportPos;
};
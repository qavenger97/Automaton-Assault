#pragma once

class AI_ToggleWurmDoor : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	void Init( Hourglass::Entity* entity );

	IBehavior::Result Update( Hourglass::Entity* entity );

	void UpdateDoorRotation( hg::Transform* trans, float angle );

	IBehavior* MakeCopy() const;

private:

	float m_Timer;
	float m_Duration;
	float m_Angle;
	uint32_t m_Open : 1;
	hg::Transform* m_Front;
	hg::Transform* m_Back;
};
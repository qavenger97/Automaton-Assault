#pragma once

class AI_FireLaser : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	void Init( Hourglass::Entity* entity );

	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	hg::Transform* m_Laser;
	float m_Timer;
	
	float m_Duration;
	Vector3 m_StartScale;
	Vector3 m_EndScale;
	uint32_t m_Reverse : 1;
	bool	m_bPlayingLaserBeamSound;
};
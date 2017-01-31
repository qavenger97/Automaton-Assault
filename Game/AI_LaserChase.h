#pragma once

class AI_LaserChase : public Hourglass::IAction
{
public:
	void LoadFromXML( tinyxml2::XMLElement* data );

	void Init( hg::Entity* entity );
	IBehavior::Result Update( Hourglass::Entity* entity );

	IBehavior* MakeCopy() const;

private:

	hg::Transform* m_Laser;
	hg::GPUParticleEmitter* m_FxLaserBurn;
	float m_Timer;
	float m_DamageTimer;
	float m_Duration;
	float m_DamageInterval;
};
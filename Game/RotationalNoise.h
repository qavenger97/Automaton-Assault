#pragma once

class RotationalNoise : public hg::IComponent
{
public:
	virtual int GetTypeID() const { return s_TypeID; }
	static uint32_t s_TypeID;

	void LoadFromXML( tinyxml2::XMLElement* data );

	void Init();

	void Update();

	hg::IComponent* MakeCopyDerived() const;

private:

	float m_NoiseLow;
	float m_NoiseHigh;
	float m_MaxAngle;
	float m_Timer;
	float m_IntervalInverse;
	
	float m_Yaw;
	float m_Pitch;

	hg::Transform* m_Trans;
};
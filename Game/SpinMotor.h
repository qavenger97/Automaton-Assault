/*!
 * \file SpinMotor.h
 *
 * \author Shiyang Ao
 * \date November 2016
 *
 * 
 */
#pragma once

class SpinMotor : public Hourglass::IComponent
{
	DECLARE_COMPONENT_TYPEID
public:
	void LoadFromXML( tinyxml2::XMLElement* data );
	void Init();

	void Update();

	void StopSpin();
	void StartSpin();

	float GetCurrentDegrees() { return XMConvertToDegrees( m_CurrentRadians ); }
	float GetCurrentRadians() { return m_CurrentRadians; }

	hg::IComponent* MakeCopyDerived() const;

private:
	bool m_Shifting;
	float m_SpinSpeed;
	float m_CurrentRadians;
	Vector3 m_FixedEuler;
	
	enum SpinType
	{
		kYaw,
		kPitch,
		kRoll
	};

	uint32_t m_SpinType;
};

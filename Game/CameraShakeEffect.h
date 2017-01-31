#pragma once

class CameraShakeEffect : public Hourglass::IComponent
{
	DECLARE_COMPONENT_TYPEID
public:

	void LoadFromXML(tinyxml2::XMLElement* data);

	void Start();

	void Update();

	hg::IComponent* MakeCopyDerived() const;

private:
	float	m_Duration;
	float	m_Fade;
	float	m_Amplitude;
	float	m_Roll;

	bool	m_Started;
};

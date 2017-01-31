#include "GamePch.h"

#include "CameraShakeEffect.h"

IMPLEMENT_GAME_COMPONENT_TYPEID(CameraShakeEffect)

void CameraShakeEffect::LoadFromXML(tinyxml2::XMLElement* data)
{
	m_Duration = 1.0f;
	data->QueryFloatAttribute("Duration", &m_Duration);

	m_Fade = 1.0f;
	data->QueryFloatAttribute("Fade", &m_Fade);

	m_Amplitude = 1.0f;
	data->QueryFloatAttribute("Amplitude", &m_Amplitude);

	m_Roll = 1.0f;
	data->QueryFloatAttribute("Roll", &m_Roll);
}

void CameraShakeEffect::Start()
{
	m_Started = false;
}

void CameraShakeEffect::Update()
{
	if (!m_Started)
	{
		hg::g_cameraSystem.GetMainCamera()->ApplyCameraShake(m_Duration, m_Fade, m_Amplitude, m_Roll);

		m_Started = true;
		GetEntity()->Destroy();
	}
}

hg::IComponent* CameraShakeEffect::MakeCopyDerived() const
{
	CameraShakeEffect* cpy = (CameraShakeEffect*)IComponent::Create(SID(CameraShakeEffect));

	cpy->m_Duration = m_Duration;
	cpy->m_Fade = m_Fade;
	cpy->m_Amplitude = m_Amplitude;
	cpy->m_Roll = m_Roll;

	return cpy;
}

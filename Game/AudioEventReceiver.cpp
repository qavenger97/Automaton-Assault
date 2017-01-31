#include "GamePch.h"

#include "AudioEventReceiver.h"

IMPLEMENT_GAME_COMPONENT_TYPEID(AudioEventReceiver)

void AudioEventReceiver::LoadFromXML(tinyxml2::XMLElement* data)
{
	m_EventName = "";
	const char* name = data->Attribute("event_name");
	if (name)
	{
		m_EventName = WSID(name);
	}
}

Hourglass::IComponent* AudioEventReceiver::MakeCopyDerived() const
{
	AudioEventReceiver* copy = (AudioEventReceiver*)IComponent::Create(SID(AudioEventReceiver));
	copy->m_EventName = m_EventName;
	return copy;
}

void AudioEventReceiver::OnEvent(const StrID& eventName)
{
	if (eventName == SID(PostAudioEvent))
	{
#if defined DEBUG_USE_STRING_AS_STRID
		hg::g_AudioSystem.PostEvent(m_EventName.c_str(), GetEntity()->GetPosition());
#else
		hg::g_AudioSystem.PostEvent(hg::StrIDUtil::GetStringFromStrID(m_EventName), entity->GetPosition());
#endif
	}
}

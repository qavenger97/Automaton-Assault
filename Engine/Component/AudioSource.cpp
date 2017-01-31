#include "pch.h"

#include "AudioSource.h"
#include "Core/ComponentFactory.h"
#include "System/AudioSystemWwise.h"

IMPLEMENT_COMPONENT_TYPEID(Hourglass::AudioSource)

void Hourglass::AudioSource::Start()
{
	hg::g_AudioSystem.RegisterEntity(GetEntity(), "AudioSourceEntity");
}

void Hourglass::AudioSource::Shutdown()
{
	hg::g_AudioSystem.UnRegisterEntity(GetEntity());
}

Hourglass::IComponent* Hourglass::AudioSource::MakeCopyDerived() const
{
	AudioSource* copy = (AudioSource*)IComponent::Create(SID(AudioSource));

	return copy;
}

void Hourglass::AudioSource::PostAudioEvent(uint64_t eventId) const
{
	hg::g_AudioSystem.PostEvent(eventId, GetEntity());
}

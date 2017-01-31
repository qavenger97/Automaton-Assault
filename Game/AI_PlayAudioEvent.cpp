#include "GamePch.h"

#include "AI_PlayAudioEvent.h"

void AI_PlayAudioEvent::LoadFromXML(tinyxml2::XMLElement* data)
{
	m_EventName = WSID("");
	const char* eventName = data->Attribute("event_name");
	if (eventName)
	{
		m_EventName = WSID(eventName);
	}

	m_Delay = 0.0f;
	data->QueryFloatAttribute("delay", &m_Delay);

	m_OnDoneReturnFailure = false;
	data->QueryBoolAttribute("return_failure", &m_OnDoneReturnFailure);
}

void AI_PlayAudioEvent::Init(Hourglass::Entity* entity)
{
	m_Timer = 0.0f;
}

Hourglass::IBehavior::Result AI_PlayAudioEvent::Update(Hourglass::Entity* entity)
{
	m_Timer += hg::g_Time.Delta();

	if (m_Timer >= m_Delay)
	{
#if defined DEBUG_USE_STRING_AS_STRID
		hg::g_AudioSystem.PostEvent(m_EventName.c_str(), entity->GetPosition());
#else
		hg::g_AudioSystem.PostEvent(hg::StrIDUtil::GetStringFromStrID(m_EventName), entity->GetPosition());
#endif
		return m_OnDoneReturnFailure ? kFAILURE : kSUCCESS;
	}

	return kRUNNING;
}

Hourglass::IBehavior* AI_PlayAudioEvent::MakeCopy() const
{
	AI_PlayAudioEvent* copy = (AI_PlayAudioEvent*)IBehavior::Create(SID(AI_PlayAudioEvent));
	copy->m_EventName = m_EventName;
	copy->m_Delay = m_Delay;
	copy->m_OnDoneReturnFailure = m_OnDoneReturnFailure;
	return copy;
}

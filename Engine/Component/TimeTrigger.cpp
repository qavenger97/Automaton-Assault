#include "pch.h"

#include "TimeTrigger.h"
#include "Core\TimeManager.h"
#include "Core\ComponentFactory.h"

IMPLEMENT_COMPONENT_TYPEID(Hourglass::TimeTrigger)

void Hourglass::TimeTrigger::LoadFromXML(tinyxml2::XMLElement* data)
{
	m_Delay = 0.0f;
	data->QueryFloatAttribute("delay", &m_Delay);

	m_Start = false;
	m_Done = false;

	BaseEventGroup::LoadFromXML(data);
}

void Hourglass::TimeTrigger::Update()
{
	if (m_Start && !m_Done)
	{
		if (m_StartTime + m_Delay <= hg::g_Time.GameElapsed())
		{
			FireEvents();
			m_Done = true;
		}
	}
}

void Hourglass::TimeTrigger::OnEvent(const StrID& eventName)
{
	if (eventName == SID(StartTimeTrigger))
	{
		m_Start = true;
		m_StartTime = hg::g_Time.GameElapsed();
	}
}

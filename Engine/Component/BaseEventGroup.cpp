#include "pch.h"

#include "BaseEventGroup.h"

#include "Core\Entity.h"

void Hourglass::BaseEventGroup::LoadFromXML(tinyxml2::XMLElement* data)
{
	m_EventTargetIds.clear();
	tinyxml2::XMLElement* eventGroupXML = data->FirstChildElement("EventTarget");

	while (eventGroupXML)
	{
		TriggerEvent e;

		if (eventGroupXML->QueryIntAttribute("instanceId", &e.targetId) == tinyxml2::XML_SUCCESS)
		{
			const char* eventName = eventGroupXML->Attribute("eventName");

			if (eventName)
			{
				e.eventName = WSID(eventName);
				m_EventTargetIds.push_back(e);
			}
		}

		eventGroupXML = eventGroupXML->NextSiblingElement("EventTarget");
	}

	m_IsTriggered = false;
}

Hourglass::IComponent* Hourglass::BaseEventGroup::MakeCopyDerived() const
{
	// Base class shouldn't be copied
	return nullptr;
}

void Hourglass::BaseEventGroup::FireEvents()
{
	if (m_IsTriggered)
		return;

	for (int i = 0; i < m_EventTargetIds.size(); i++)
	{
		int id = m_EventTargetIds[i].targetId;
		Entity* ent = Entity::FindByInstanceID(id);

		if (!ent)
			continue;

		EventMessage msg;
		msg.eventName = m_EventTargetIds[i].eventName;
		ent->SendMsg(&msg);
	}

	m_IsTriggered = true;
}

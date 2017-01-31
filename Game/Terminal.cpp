#include "GamePch.h"

#include "Terminal.h"

IMPLEMENT_GAME_COMPONENT_TYPEID(Terminal)

void Terminal::Init()
{
	m_bPlayerInRange = false;
}

void Terminal::LoadFromXML(tinyxml2::XMLElement* data)
{
	m_EventTargetIds.clear();
	tinyxml2::XMLElement* xmlEventTarget = data->FirstChildElement("EventTarget");

	while (xmlEventTarget)
	{
		hg::TriggerEvent e;

		if (xmlEventTarget->QueryIntAttribute("instanceId", &e.targetId) == tinyxml2::XML_SUCCESS)
		{
			const char* eventName = xmlEventTarget->Attribute("eventName");

			if (eventName)
			{
				e.eventName = WSID(eventName);
				m_EventTargetIds.push_back(e);
			}
		}

		xmlEventTarget = xmlEventTarget->NextSiblingElement("EventTarget");
	}

}

void Terminal::Update()
{
	if (m_bPlayerInRange)
	{
		hg::DevTextRenderer::DrawText_WorldSpace("Press E to interactive", GetEntity()->GetPosition());

		if (hg::g_Input.GetBufferedKeyState('E') == hg::kKeyStatePressed)
		{
			for (int i = 0; i < m_EventTargetIds.size(); i++)
			{
				int id = m_EventTargetIds[i].targetId;
				hg::Entity* ent = hg::Entity::FindByInstanceID(id);

				if (!ent)
					continue;

				hg::EventMessage msg;
				msg.eventName = m_EventTargetIds[i].eventName;
				ent->SendMsg(&msg);
			}
		}
	}
}

void Terminal::OnTriggerEnter(hg::Entity* other)
{
	if (other->GetTag() == SID(Player))
	{
		m_bPlayerInRange = true;
	}
}

void Terminal::OnTriggerExit(hg::Entity* other)
{
	if (other->GetTag() == SID(Player))
	{
		m_bPlayerInRange = false;
	}
}

hg::IComponent* Terminal::MakeCopyDerived() const
{
	Terminal* cpy = (Terminal*)IComponent::Create(SID(Terminal));

	cpy->m_bPlayerInRange = false;

	return cpy;
}

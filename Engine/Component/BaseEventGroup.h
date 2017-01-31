#pragma once

#include "IComponent.h"
#include "Core\Message.h"

namespace Hourglass
{
	class EventMessage : public Message
	{
	public:
		int GetType() const { return MessageType::kOnEvent; }

		StrID eventName;
	};

	struct TriggerEvent
	{
		int targetId;
		StrID eventName;
	};

	class BaseEventGroup : public IComponent
	{
	public:
		// Override IComponent functions

		void LoadFromXML(tinyxml2::XMLElement* data);

		IComponent* MakeCopyDerived() const;

		// Send all events to receivers
		void FireEvents();

	protected:
		std::vector<TriggerEvent>		m_EventTargetIds;

		bool	m_IsTriggered;
	};
}
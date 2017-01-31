#pragma once

#include "BaseEventGroup.h"

namespace Hourglass
{
	class TimeTrigger : public BaseEventGroup
	{
		DECLARE_COMPONENT_TYPEID
	public:
		void LoadFromXML(tinyxml2::XMLElement* data);

		void Update();

		void OnEvent(const StrID& eventName);

	private:
		float	m_Delay;
		bool	m_Start;
		float	m_StartTime;
		bool	m_Done;
	};
}
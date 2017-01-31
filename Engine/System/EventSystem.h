#pragma once
#include "ISystem.h"
#include "Component\TriggerEventGroup.h"
#include "Component\TimeTrigger.h"

namespace Hourglass
{


	class EventSystem : public ISystem
	{
	public:
		void Init();

		void Update();

	private:
		DECLARE_COMPONENT_POOL(TriggerEventGroup, 1024);
		DECLARE_COMPONENT_POOL(TimeTrigger, 1024);
	};

}
#include "pch.h"

#include "EventSystem.h"

void Hourglass::EventSystem::Init()
{
	INIT_COMPONENT_POOL(TriggerEventGroup);
	INIT_COMPONENT_POOL(TimeTrigger);
}

void Hourglass::EventSystem::Update()
{
	m_TimeTriggerPool.UpdatePooled();
}

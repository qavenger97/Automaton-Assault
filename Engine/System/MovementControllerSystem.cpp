#include "pch.h"
#include "MovementControllerSystem.h"

void Hourglass::MovementControllerSystem::Init()
{
	INIT_COMPONENT_POOL(MovementController);
}

void Hourglass::MovementControllerSystem::Update()
{
	for (MovementController& m : m_MovementControllers)
	{
		if(m.IsEnabled() && m.IsAlive())
			m.Update();
	}
}

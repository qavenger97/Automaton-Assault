#include "pch.h"

#include "GameLogicSystem.h"

namespace Hourglass
{
	void GameLogicSystem::Start()
	{
		RunStoredStartFunctions();
	}
	void GameLogicSystem::Update()
	{
		RunStoredUpdateFunctions();  // This is a ComponentFactory function (protected)
	}
}
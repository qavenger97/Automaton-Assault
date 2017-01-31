#pragma once
#include "Component/MovementController.h"
#include "../Core/ComponentPool.h"
#include "../System/ISystem.h"
namespace Hourglass
{

	class MovementControllerSystem : public ISystem
	{
	public:
		void Init();
		void Update();
	private:
		DECLARE_COMPONENT_POOL(MovementController, 8);
	};

}
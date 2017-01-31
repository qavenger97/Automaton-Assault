#pragma once

#include "..\Core\ComponentFactory.h"

namespace Hourglass
{
	class GameLogicSystem : public ComponentFactory
	{
	public:

		/* 
		* Run all start functions on all game logic components
		*/
		void Start();

		/*
		* Update all game logic components for the frame
		*/
		void Update();

	};
}
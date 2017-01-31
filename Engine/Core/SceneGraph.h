#pragma once
#include "Component\Transform.h"
#include <list>
namespace Hourglass
{
	class SceneGraph : public Transform
	{
	public:

		virtual void Init();
		void Update();
		void Reset();
	};
}
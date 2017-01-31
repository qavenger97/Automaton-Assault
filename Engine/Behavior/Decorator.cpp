#include "pch.h"

#include "Decorator.h"

namespace Hourglass
{
	void Decorator::Init( IBehavior* behavior )
	{
		child = behavior;
	}
}
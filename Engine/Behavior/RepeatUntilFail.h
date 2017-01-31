#pragma once

#include "Decorator.h"

namespace Hourglass
{
	class RepeatUntilFail : public Decorator
	{

		virtual Result Run( Entity* entity );

		/**
		* Make a copy of this component
		*/
		virtual IBehavior* MakeCopy() const;
	};
}
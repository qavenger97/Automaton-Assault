#pragma once

#include "BehaviorFactory.h"
#include "StrID.h"

namespace Hourglass
{
	class AssembledBehaviorReg : BehaviorFactory
	{
	public:
		AssembledBehaviorReg( StrID name, IBehavior* root )
		{
			Register( name, root );
		}
	};
}
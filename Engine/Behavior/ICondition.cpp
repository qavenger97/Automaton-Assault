#include "pch.h"

#include "ICondition.h"

namespace Hourglass
{
	IBehavior::Result ICondition::Run( Entity* entity )
	{
		return Check( entity ) ? IBehavior::kSUCCESS : IBehavior::kFAILURE;
	}
}
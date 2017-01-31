#include "pch.h"

#include "IBehavior.h"
#include "Core/BehaviorFactory.h"

namespace Hourglass
{
	IBehavior* IBehavior::Create( StrID name )
	{
		return BehaviorFactory::GetFreeBehavior( name );
	}

	IBehavior * IBehavior::AssembleRoot( StrID name )
	{
		return BehaviorFactory::GetAssembledRoot( name );
	}
}
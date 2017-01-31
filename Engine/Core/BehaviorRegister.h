#pragma once

#include "BehaviorFactory.h"
#include "StrID.h"

namespace Hourglass
{
	template<typename T>
	class BehaviorRegister : BehaviorFactory
	{
	public:
		BehaviorRegister( StrID name, BaseBehaviorPool* pool )
		{
			Register( name, pool, &CreateInstanceToAssemble<T> );
		}
	};
}
#pragma once

#include "ComponentFactory.h"

namespace Hourglass
{
	template<typename T>
	class ComponentRegister : ComponentFactory
	{
	public:

		ComponentRegister( StrID name, BaseComponentPool* pool, unsigned int compRegisterFlags )
		{
			Register( name, pool, &CreateInstanceToAssemble<T>, compRegisterFlags );
		}
	};
}
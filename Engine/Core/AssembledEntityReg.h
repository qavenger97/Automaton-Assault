#pragma once

#include "EntityAssembler.h"
#include "StrID.h"

namespace Hourglass
{
	class AssembledEntityReg : EntityAssembler
	{
	public:
		AssembledEntityReg( StrID name, Entity* entity)
		{
			Register( name, entity );
		}
	};
}
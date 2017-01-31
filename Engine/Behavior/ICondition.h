#pragma once

#include "Primitive.h"

namespace Hourglass
{
	class ICondition : public Primitive
	{
	public:		

		virtual IBehavior::Result Run( Entity* entity ) final;

		/**
		*  The condition is checked here, must return either true or false
		*/
		virtual bool Check( Entity* entity ) = 0;

		virtual void DestroyDerived() { }
	};
}
#pragma once

#include "IBehavior.h"

namespace Hourglass
{
	class Primitive : public IBehavior
	{
	public:

		virtual void AddBehavior( IBehavior* behavior ) { assert( 1 == 0 ); } // shouldn't happen, these are leafs
		virtual bool IsRunningChild() const { return false; } // primitives cannot have children
		virtual void Reset() { } // nothing to reset in primitives (not that I can think of [Brandon 10-3-2016])
		virtual void DestroyDerived(){ }
	};
}
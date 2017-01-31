#include "pch.h"

#include "Inverter.h"

namespace Hourglass
{
	IBehavior::Result Inverter::Run( Entity* entity )
	{
		Result result = GetChild()->Run( entity );

		if (result == kFAILURE)
		{
			return kSUCCESS;
		}
		else if (result == kSUCCESS)
		{
			return kFAILURE;
		}
		else
		{
			return kRUNNING;
		}
	}

	IBehavior* Inverter::MakeCopy() const
	{
		Inverter* copy = (Inverter*)IBehavior::Create( SID(Inverter) );
		copy->SetBehavior( GetChild()->MakeCopy() );
		return copy;
	}
}
#include "pch.h"

#include "RepeatUntilFail.h"

namespace Hourglass
{
	IBehavior::Result RepeatUntilFail::Run( Entity* entity )
	{
		Result result = GetChild()->Run( entity );
		if (result == kFAILURE)
		{
			return kSUCCESS;
		}
		else if (result == kSUCCESS)
		{
			GetChild()->Reset();
		}

		return kRUNNING;
	}

	IBehavior* RepeatUntilFail::MakeCopy() const
	{
		RepeatUntilFail* copy = (RepeatUntilFail *)IBehavior::Create( SID( RepeatUntilFail ) );
		copy->SetBehavior( GetChild()->MakeCopy() );
		return copy;
	}
}
#pragma once

#include "Sequence.h"

namespace Hourglass
{
	class Selector : public Sequence
	{
	public:

		// This is the only property that separates selector from sequence
		virtual Result GetResultToProceed() { return kFAILURE; }

		/**
		* Make a copy of this component
		*/
		virtual IBehavior* MakeCopy() const;
	};
}
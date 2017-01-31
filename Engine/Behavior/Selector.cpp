#include "pch.h"

#include "Selector.h"

namespace Hourglass
{
	IBehavior * Selector::MakeCopy() const
	{
		Selector* copy = (Selector*)IBehavior::Create( SID( Selector ) );

		copy->Clear();

		for (unsigned int i = 0; i < NumBehaviors(); ++i)
		{
			copy->AddBehavior( m_ChildBehaviors[i]->MakeCopy() );
		}

		return copy;
	}
}
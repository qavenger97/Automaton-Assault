#include "pch.h"

#include "IAction.h"

namespace Hourglass
{
	IBehavior::Result IAction::Run( Entity* entity )
	{
		if (!m_IsInitialized)
		{
			Init( entity );
			m_IsInitialized = true;
		}
			
		IBehavior::Result result = Update( entity );

		if (result != IBehavior::Result::kRUNNING)
		{
			m_IsInitialized = false;
			Shutdown( entity );
		}

		return result;
	}
}
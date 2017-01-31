#include "pch.h"

#include "Parallel.h"

namespace Hourglass
{
	void Parallel::LoadFromXML( tinyxml2::XMLElement * data )
	{
		const char* policy = data->Attribute( "policy" );
		if (strcmp(policy, "success") == 0)
		{
			m_Policy = kSUCCESS;
		}
		else if (strcmp(policy, "failure") == 0)
		{
			m_Policy = kFAILURE;
		}
		else
		{
			// should not get here
			assert( 0 );
		}
	}

	void Parallel::AddBehavior( IBehavior* behavior )
	{
		assert( m_NumChildren < s_kMaxInParallel );
		m_ChildBehaviors[m_NumChildren++] = behavior;
	}

	void Parallel::DestroyDerived()
	{
		for (unsigned int i = 0; i < m_NumChildren; ++i)
		{
			m_ChildBehaviors[i]->Destroy();
			m_ChildBehaviors[i] = nullptr;
		}
		m_NumChildren = 0;
	}

	bool Parallel::IsRunningChild() const
	{
		return m_FlagsRunningChildren != 0;
	}

	void Parallel::Start()
	{
		m_FlagsRunningChildren = 0;
	}

	IBehavior::Result Parallel::Run( Entity* entity )
	{
		if (m_FlagsRunningChildren == 0)
		{
			m_FlagsRunningChildren = 0xFFFFFFFF; // all flags on
		}

		for (unsigned int i = 0; i < m_NumChildren; ++i)
		{
			if(m_FlagsRunningChildren & (1 << i))
			{
				Result result = m_ChildBehaviors[i]->Run( entity );
				if (result == m_Policy)
				{
					Reset();
					return result;
				}
				else if (result != kRUNNING)
				{
					m_FlagsRunningChildren &= ~(1 << i); // flag not running
				}
			}
		}

		unsigned int runningMask = (1 << m_NumChildren) - 1; // first numChildren bits turned on for this mask
		if ((runningMask & m_FlagsRunningChildren) == 0)
		{
			m_FlagsRunningChildren = 0;
			return m_Policy == kSUCCESS ? kFAILURE : kSUCCESS;
		}

		return kRUNNING;
	}

	void Parallel::Reset()
	{
		for (unsigned int i = 0; i < m_NumChildren; ++i)
		{
			m_ChildBehaviors[i]->Reset();
		}

		m_FlagsRunningChildren = 0;
	}

	IBehavior * Parallel::MakeCopy() const
	{
		Parallel* copy = (Parallel*)IBehavior::Create( SID( Parallel ) );
		copy->SetPolicy( (Hourglass::IBehavior::Result)m_Policy );
		copy->Clear();
		for (unsigned int i = 0; i != m_NumChildren; ++i)
		{
			copy->AddBehavior( m_ChildBehaviors[i]->MakeCopy() );
		}

		return copy;
	}
}
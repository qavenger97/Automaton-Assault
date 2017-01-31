#include "pch.h"

#include "Sequence.h"

namespace Hourglass
{
	void Sequence::AddBehavior( IBehavior* behavior )
	{		
		m_ChildBehaviors[m_NumChildren++] = behavior;
	}

	bool Sequence::IsRunningChild() const
	{
		return m_RunningChildID != s_kNoRunningChild;
	}

	void Sequence::DestroyDerived()
	{
		for (unsigned int i = 0; i < m_NumChildren; ++i)
		{
			m_ChildBehaviors[i]->Destroy();
			m_ChildBehaviors[i] = nullptr;
		}
		m_NumChildren = 0;
	}

	void Sequence::Start()
	{
		m_RunningChildID = s_kNoRunningChild;
	}

	IBehavior::Result Sequence::Run( Entity* entity )
	{
		if (m_RunningChildID == s_kNoRunningChild)
		{
			m_RunningChildID = 0;
		}

		for (; m_RunningChildID < m_NumChildren; ++m_RunningChildID)
		{
			Result result = m_ChildBehaviors[m_RunningChildID]->Run( entity );
			if (result != GetResultToProceed())
			{
				if (result != Result::kRUNNING)
				{
					m_RunningChildID = s_kNoRunningChild;
				}
				return result;
			}
		}

		m_RunningChildID = s_kNoRunningChild;

		return GetResultToProceed();
	}

	void Sequence::Reset()
	{
		for (unsigned int i = 0; i < m_NumChildren; ++i)
		{
			m_ChildBehaviors[i]->Reset();
		}

		m_RunningChildID = s_kNoRunningChild;
	}

	IBehavior* Sequence::MakeCopy() const
	{
		Sequence* copy = (Sequence*)IBehavior::Create( SID( Sequence ) );

		copy->Clear();

		for (unsigned int i = 0; i < m_NumChildren; ++i)
		{
			copy->AddBehavior( m_ChildBehaviors[i]->MakeCopy() );
		}

		copy->m_RunningChildID = s_kNoRunningChild;

		return copy;
	}
}
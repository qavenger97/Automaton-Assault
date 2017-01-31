#pragma once

#include "IBehavior.h"
#include <vector>

namespace Hourglass
{
	class Sequence : public IBehavior
	{
	public:

		void AddBehavior( IBehavior* behavior );

		void Clear() { m_NumChildren = 0; }
		
		virtual bool IsRunningChild() const;

		virtual void DestroyDerived();

		virtual void Start();

		virtual Result Run( Entity* entity );

		virtual void Reset();

		virtual Result GetResultToProceed() { return kSUCCESS; }

		/**
		* Make a copy of this component
		*/
		virtual IBehavior* MakeCopy() const;

	private:

		static const uint32_t s_kNoRunningChild = -1;
		static const uint32_t s_kMaxChildren = 40;

	protected:

		unsigned int NumBehaviors() const { return m_NumChildren; }

		IBehavior* m_ChildBehaviors[s_kMaxChildren];

	private:
 
		uint32_t m_NumChildren = 0;		
		uint32_t m_RunningChildID;
	};
}
#pragma once

#include "IBehavior.h"
#include <vector>

namespace Hourglass
{
	class Parallel : public IBehavior
	{
	public:

		virtual void LoadFromXML( tinyxml2::XMLElement* data );

		void AddBehavior( IBehavior* behavior );

		void Clear() { m_NumChildren = 0; }

		virtual void DestroyDerived();

		virtual bool IsRunningChild() const;

		virtual void Start();

		virtual Result Run( Entity* entity );

		virtual void Reset();

		virtual void SetPolicy( IBehavior::Result policy ) { m_Policy = policy; };

		/**
		* Make a copy of this component
		*/
		virtual IBehavior* MakeCopy() const;

	private:

		static const int32_t s_kNoRunningChild = -1;
		static const int32_t s_kMaxInParallel = 32;

		uint32_t m_FlagsRunningChildren;

		uint32_t m_NumChildren = 0;

		int32_t m_Policy;

		IBehavior* m_ChildBehaviors[s_kMaxInParallel];
	};
}
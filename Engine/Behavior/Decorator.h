#pragma once

#include "IBehavior.h"

namespace Hourglass
{
	class Decorator : public IBehavior
	{
	public:

		virtual bool IsRunningChild() const { return m_Child->IsRunningChild(); }

		virtual void DestroyDerived() { m_Child->Destroy(); }

		void SetBehavior( IBehavior* behavior ) { m_Child = behavior; }

		virtual void AddBehavior( IBehavior* behavior ) { SetBehavior( behavior ); }

		virtual Result Run( Entity* entity ) = 0;

		virtual void Reset() { m_Child->Reset(); }

	protected:

		//IBehavior* GetChild() { return m_Child; }
		IBehavior* GetChild() const { return m_Child; }

	private:

		IBehavior* m_Child;
	};
}
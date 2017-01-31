#pragma once

#include "IComponent.h"
#include "Core\ComponentRegister.h"
#include "Behavior\IBehavior.h"

namespace Hourglass
{
	class BehaviorTree : public IComponent
	{
	public:

		virtual void LoadFromXML( tinyxml2::XMLElement* data );

		void SetRoot( IBehavior* root );

		void DestroyRoot();

		virtual void Update();

		virtual void Shutdown();

		virtual int GetTypeID() const { return s_TypeID; }

		/**
		* Make a copy of this component
		*/
		virtual IComponent* MakeCopyDerived() const;

		static uint32_t s_TypeID;

	private:

		IBehavior* m_Root = nullptr;
		uint32_t m_FirstTick : 1;
	};
}
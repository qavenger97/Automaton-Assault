/*!
 * \file TriggerEventGroup.h
 *
 * \author Shiyang Ao
 * \date November 2016
 *
 * 
 */
#pragma once

#include "IComponent.h"
#include "BaseEventGroup.h"

namespace Hourglass
{

	class TriggerEventGroup : public BaseEventGroup
	{
		DECLARE_COMPONENT_TYPEID
	public:
		// Override IComponent functions

		IComponent* MakeCopyDerived() const;

		void OnTriggerEnter(Entity* other);

		// TriggerEventGroup functions
	protected:
	};
}
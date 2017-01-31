#pragma once

#include "IComponent.h"

namespace Hourglass
{
	class UIComponent : public IComponent
	{
	public:

		virtual int GetTypeID() const { return s_TypeID; }

		/**
		* Make a copy of this component
		*/
		virtual IComponent* MakeCopyDerived() const;

		static uint32_t s_TypeID;
	};
}
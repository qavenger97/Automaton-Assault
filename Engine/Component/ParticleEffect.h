#pragma once

#include "IComponent.h"
#include "Core/ComponentRegister.h"

namespace Hourglass
{
	class ParticleEffect : public IComponent
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
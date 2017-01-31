#pragma once

#include "IComponent.h"

namespace Hourglass
{
	class AudioSource : public IComponent
	{
		DECLARE_COMPONENT_TYPEID
	public:
		void Start();

		void Shutdown();

		IComponent* MakeCopyDerived() const;

		void PostAudioEvent(uint64_t eventId) const;
	};
}
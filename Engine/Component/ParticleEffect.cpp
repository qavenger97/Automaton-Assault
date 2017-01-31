#include "pch.h"

#include "ParticleEffect.h"
#include "../System/ParticleSystem.h"
#include "Core/ComponentFactory.h"

namespace Hourglass
{
	uint32_t ParticleEffect::s_TypeID = ComponentFactory::GetSystemComponentID();

	IComponent* ParticleEffect::MakeCopyDerived() const
	{
		// TODO: Make a copy of this component
		// Do not bother initializing any values that the Init function will modify,
		// as Init will probably need to be called after multiple components are copied
		return IComponent::Create( SID(ParticleEffect) );
	}
}
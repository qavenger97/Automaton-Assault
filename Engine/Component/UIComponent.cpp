#include "pch.h"

#include "UIComponent.h"
#include "../System/UISystem.h"
#include "Core/ComponentFactory.h"

namespace Hourglass
{
	uint32_t UIComponent::s_TypeID = ComponentFactory::GetSystemComponentID();

	IComponent* UIComponent::MakeCopyDerived() const
	{
		// TODO: Make a copy of this component
		// Do not bother initializing any values that the Init function will modify,
		// as Init will probably need to be called after multiple components are copied
		return IComponent::Create( SID(UIComponent) );
	}
}
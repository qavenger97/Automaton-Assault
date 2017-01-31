#include "pch.h"

#include "IComponent.h"
#include "Core\ComponentFactory.h"

namespace Hourglass
{
	IComponent::IComponent()
	{
		m_IsAlive = false;
		m_IsEnabled = false;
		m_Entity = nullptr;
	}

	IComponent* IComponent::Create( StrID name )
	{
		return ComponentFactory::GetFreeComponent( name );
	}

	IComponent* IComponent::MakeCopy()
	{
		IComponent* copy = MakeCopyDerived();
		copy->m_IsEnabled = m_IsEnabled;
		copy->m_IsAlive = m_IsAlive;
		return copy;
	}

	/**
	* Attaches this component to the passed in entity
	*/
	void IComponent::Attach( Entity* entity )
	{
		m_Entity = entity;
	}

	/**
	* Detaches this component from it's owning owning entity
	*/
	void IComponent::Detach()
	{
		m_Entity = nullptr;
		SetEnabled( false );
	}

	void IComponent::Shutdown()
	{
		m_IsAlive = false;
		m_IsEnabled = false;
	}
}
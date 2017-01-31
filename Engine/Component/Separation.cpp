#include "pch.h"

#include "Component/IComponent.h"
#include "Core/ComponentFactory.h"
#include "Separation.h"

namespace Hourglass
{
	uint32_t Separation::s_TypeID = ComponentFactory::GetSystemComponentID();

	void Separation::LoadFromXML( tinyxml2::XMLElement* data )
	{
		const char* mode = data->Attribute( "mode" );
		if (strcmp( mode, "agent" ) == 0)
		{
			m_Mode = kAgent;
		}
		else if (strcmp( mode, "obstacle" ) == 0)
		{
			m_Mode = kObstacle;
		}

		m_SeparationStrength = 0.5f;
		data->QueryFloatAttribute( "separation_strength", &m_SeparationStrength );

		m_SafeRadius = 2.0f;
		data->QueryFloatAttribute( "safe_radius", &m_SafeRadius );
	}

	IComponent* Separation::MakeCopyDerived() const
	{
		Separation* copy = (Separation*)IComponent::Create( SID( Separation ) );

		copy->m_Mode = m_Mode;
		copy->m_SeparationStrength = m_SeparationStrength;
		copy->m_SafeRadius = m_SafeRadius;

		return copy;
	}
}

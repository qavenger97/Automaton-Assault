#include "GamePch.h"

#include "ShiftingPlatform.h"

uint32_t ShiftingPlatform::s_TypeID = hg::ComponentFactory::GetGameComponentID();

void ShiftingPlatform::LoadFromXML( tinyxml2::XMLElement* data )
{
	bool isExtended = true;
	data->QueryBoolAttribute( "extended", &isExtended );
	m_Extended = isExtended;

	tinyxml2::XMLElement* extendedVectorXML = data->FirstChildElement( "ExtendVector" );

	extendedVectorXML->QueryFloatAttribute( "x", &m_ExtendVector.x );
	extendedVectorXML->QueryFloatAttribute( "y", &m_ExtendVector.y );
	extendedVectorXML->QueryFloatAttribute( "z", &m_ExtendVector.z );	

	m_ShiftLength = 2.0f;
	data->QueryFloatAttribute( "shift_length", &m_ShiftLength );

	m_ShiftLengthInv = 1.0f / m_ShiftLength;
}

void ShiftingPlatform::Start()
{
	if (m_Extended)
	{
		GetEntity()->GetTransform()->Translate( m_ExtendVector );
	}

	m_ShiftActivated = false;
}

void ShiftingPlatform::Update()
{
	//hg::DevTextRenderer::DrawText_WorldSpace( "Bridge", GetEntity()->GetPosition() );

	if (IsExtending())
	{
		m_Timer += hg::g_Time.Delta();

		if (m_Timer >= m_ShiftLength)
		{
			m_ShiftActivated = false;
			m_Extended = true;

			m_Timer = m_ShiftLength;
		}

		Vector3 newPos = Vector3::Lerp( m_ShiftStart, m_ShiftEnd, min(1.0f, m_Timer * m_ShiftLengthInv) );
		GetEntity()->GetTransform()->SetPosition( newPos );
	}
	else if (IsRetracting())
	{
		m_Timer += hg::g_Time.Delta();

		if (m_Timer > m_ShiftLength)
		{
			m_ShiftActivated = false;
			m_Extended = false;
		}

		Vector3 newPos = Vector3::Lerp( m_ShiftStart, m_ShiftEnd, m_Timer * m_ShiftLengthInv );
		GetEntity()->GetTransform()->SetPosition( newPos );
	}
}

void ShiftingPlatform::Extend()
{
	if (m_Extended || m_ShiftActivated)
	{
		return;
	}

	m_ShiftActivated = true;
	m_Timer = 0.0f;

	hg::Transform& trans = *GetEntity()->GetTransform();
	m_ShiftStart = trans.GetWorldPosition();
	m_ShiftEnd = m_ShiftStart + m_ExtendVector;
}

void ShiftingPlatform::Retract()
{
	if (!m_Extended || m_ShiftActivated)
	{
		return;
	}

	m_ShiftActivated = true;
	m_Timer = 0.0f;

	hg::Transform& trans = *GetEntity()->GetTransform();
	m_ShiftStart = trans.GetWorldPosition();
	m_ShiftEnd = m_ShiftStart - m_ExtendVector;
}

void ShiftingPlatform::OnEvent(const StrID& eventName)
{
	if (eventName == SID(Extend))
	{
		Extend();
	}
	else if (eventName == SID(Retract))
	{
		Retract();
	}
}

hg::IComponent* ShiftingPlatform::MakeCopyDerived() const
{
	ShiftingPlatform* copy = (ShiftingPlatform*)IComponent::Create( SID(ShiftingPlatform) );

	// TODO: handle copying of data members (ex: copy->m_data = m_data)

	return copy;
}

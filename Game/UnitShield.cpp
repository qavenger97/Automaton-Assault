#include "GamePch.h"

#include "UnitShield.h"

uint32_t UnitShield::s_TypeID = hg::ComponentFactory::GetGameComponentID();

void UnitShield::LoadFromXML( tinyxml2::XMLElement* data )
{
	m_Color = Color( 0.29f, 0.0f, 0.59f, 1.0f );
	data->QueryFloatAttribute( "r", &m_Color.x );
	data->QueryFloatAttribute( "g", &m_Color.y );
	data->QueryFloatAttribute( "b", &m_Color.z );
	data->QueryFloatAttribute( "a", &m_Color.w );

	m_Radius = 1.0f;
	data->QueryFloatAttribute( "radius", &m_Radius );

	m_MaxSegments = 10;
	data->QueryIntAttribute( "max_segments", &m_MaxSegments );

	data->QueryFloatAttribute( "offset_x", &m_Offset.x );
	data->QueryFloatAttribute( "offset_y", &m_Offset.y );
	data->QueryFloatAttribute( "offset_z", &m_Offset.z );
}

void UnitShield::Start()
{
	// Turned off by default
	IComponent::SetEnabled( false );
}

void UnitShield::Update()
{
	int seconds = int( hg::g_Time.Elapsed() * 20 );
	int segmentCount = 10 + seconds % (m_MaxSegments + 1);
	const XMVECTOR& worldPos = GetEntity()->GetTransform()->GetWorldPosition();
	hg::DebugRenderer::DrawSphere( m_Offset + Vector3(worldPos), m_Radius, m_Color, segmentCount );
}

hg::IComponent* UnitShield::MakeCopyDerived() const
{
	UnitShield* copy = (UnitShield*)IComponent::Create( SID(UnitShield) );

	copy->m_Color = m_Color;
	copy->m_Radius = m_Radius;
	copy->m_MaxSegments = m_MaxSegments;
	copy->m_Offset = m_Offset;

	return copy;
}

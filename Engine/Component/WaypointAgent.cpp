#include "pch.h"

#include "WaypointAgent.h"
#include "Core\Entity.h"
#include "Core\ComponentFactory.h"
#include "Renderer\DebugRenderer.h"

namespace Hourglass
{
	uint32_t WaypointAgent::s_TypeID = ComponentFactory::GetSystemComponentID();

	void WaypointAgent::Init()
	{
		m_NeedsPath = true;
		m_NeedsCurrentWaypoint = true;
		m_NeedsNextWaypoint = false;
		m_Dormant = false;
	}

	void WaypointAgent::LoadFromXML( tinyxml2::XMLElement* data )
	{
		m_CompletionRadius = 1.0f;
		data->QueryFloatAttribute( "radius", &m_CompletionRadius );
		m_NeedsPath = true;
		m_NeedsCurrentWaypoint = true;
		m_NeedsNextWaypoint = false;
		m_Dormant = false;
	}

	const Vector3 WaypointAgent::GetDesiredDirection() const
	{
		Vector3 direction = (m_WaypointPosition - (Vector3)GetEntity()->GetTransform()->GetWorldPosition());
		direction.Normalize();
		return direction;
	}

	void WaypointAgent::SetDestination( Vector3 pos )
	{
		m_Destination = pos;
		m_NeedsPath = true;
		m_NeedsNextWaypoint = true;		
	}

	void WaypointAgent::SetNextWaypoint( unsigned int toVertex )
	{
		m_NextWaypoint = toVertex; 
		m_NeedsNextWaypoint = false; 
		m_NeedsPath = false;
	}

	void WaypointAgent::SetCurrentWaypoint( unsigned int toVertex )
	{
		m_CurrentWaypoint = toVertex;
		m_NeedsCurrentWaypoint = false;
	}

	void WaypointAgent::Cease()
	{
		m_NeedsCurrentWaypoint = false;
		m_NeedsNextWaypoint = false;
		m_NeedsPath = false;
	}

	void WaypointAgent::Continue()
	{
		m_NeedsCurrentWaypoint = true;
		m_NeedsNextWaypoint = true;
		m_NeedsPath = true;
	}

	void WaypointAgent::Update()
	{
		if (!GetEntity())
			return;

		Vector3 currPos = Vector3(GetEntity()->GetTransform()->GetWorldPosition());
		currPos.y = 0.0f;
		Vector3 waypoint = m_WaypointPosition;
		waypoint.y = 0.0f;
		float distSqr = Vector3::DistanceSquared( waypoint, currPos );
		if (distSqr <= m_CompletionRadius * m_CompletionRadius)
		{
			m_NeedsNextWaypoint = true;
			m_NeedsPath = true;
			m_CurrentWaypoint = m_NextWaypoint;
		}
	}

	void WaypointAgent::Shutdown()
	{

	}

	IComponent* WaypointAgent::MakeCopyDerived() const
	{
		WaypointAgent* cpy = (WaypointAgent*)IComponent::Create( SID( WaypointAgent ) );
		cpy->m_NextWaypoint = m_NextWaypoint;
		cpy->m_CurrentWaypoint = m_CurrentWaypoint;
		cpy->m_Destination = m_Destination;
		cpy->m_WaypointPosition = m_WaypointPosition;
		cpy->m_CompletionRadius = m_CompletionRadius;
		cpy->m_NeedsPath = m_NeedsPath;
		cpy->m_NeedsCurrentWaypoint = m_NeedsCurrentWaypoint;
		cpy->m_NeedsNextWaypoint = m_NeedsNextWaypoint;
		cpy->m_Dormant = m_Dormant;

		return cpy;
	}
}

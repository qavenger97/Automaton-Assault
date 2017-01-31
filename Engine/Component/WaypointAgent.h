#pragma once

#include "IComponent.h"

namespace Hourglass 
{
	class WaypointAgent : public IComponent
	{
	public:		

		virtual void Init();

		virtual void LoadFromXML( tinyxml2::XMLElement* data );

		void Invalidate() { m_NeedsCurrentWaypoint = 1; }

		bool DoesNeedPath() const { return m_NeedsPath != 0; }

		bool DoesNeedNextWaypoint() const { return m_NeedsNextWaypoint != 0; }

		bool DoesNeedCurrentWaypoint() const { return m_NeedsCurrentWaypoint != 0; }

		bool IsDormant() const { return m_Dormant != 0; }

		float GetCompletionRadius() const { return m_CompletionRadius; }

		const Vector3 GetDesiredDirection() const;

		unsigned int GetNextWaypoint() { return m_NextWaypoint; }

		unsigned int GetCurrentWaypoint() { return m_CurrentWaypoint; }

		const Vector3 GetDestination() const { return m_Destination; }

		void SetDestination( Vector3 pos );

		void SetCompletionRadius( float radius ) { m_CompletionRadius = radius; }

		void SetWaypointPosition( Vector3 pos ) { m_WaypointPosition = pos; }

		void SetNextWaypoint( unsigned int toVertex );

		void SetCurrentWaypoint( unsigned int toVertex );

		/**
		* Causes the waypoint agent to become dormant until continue is called
		*/
		void Cease();

		/**
		* If the waypoint agent is dormant, it will begin updating again
		*/
		void Continue();

		virtual void Update();

		virtual void Shutdown();

		virtual int GetTypeID() const { return s_TypeID; }

		/**
		* Make a copy of this component
		*/
		virtual IComponent* MakeCopyDerived() const;

		static uint32_t s_TypeID;
		
	private:

		static const unsigned int s_kWaypointNotSet = UINT_MAX;
		unsigned int m_NextWaypoint;
		unsigned int m_CurrentWaypoint;

		Vector3 m_Destination;

		Vector3 m_WaypointPosition;

		float m_CompletionRadius;

		uint32_t m_NeedsPath : 1;
		uint32_t m_NeedsCurrentWaypoint : 1;
		uint32_t m_NeedsNextWaypoint : 1;
		uint32_t m_Dormant : 1;
	};
}
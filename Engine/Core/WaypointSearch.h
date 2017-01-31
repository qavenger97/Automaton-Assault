#pragma once

#include "WaypointGraph.h"
#include <unordered_map>
#include <deque>
#include "Math/Aabb.h"

namespace Hourglass
{
	class WaypointSearch
	{
	public:

		void DrawDebug();

		void Init( WaypointGraph* waypointGraph );

		void Configure( unsigned int start, const unsigned int end );

		void FindPath();

		void TimeslicedFindPath( float timeslice );		

		bool IsComplete() { return m_Solution != s_kNoSolution; }

		unsigned int GetSolutionWaypoint();

	private:

		struct Planner
		{
			WaypointGraph::WVertex* m_Vertex;
			Planner* m_Parent;
			float m_GivenCost;
			float m_FinalCost;
		};

		/**
		* Pop the front of the queue and search
		*/
		void SearchFront();

		/**
		* Priority Comparison Function
		*/
		static bool CostCmp( Planner* const& lhs, Planner* const& rhs );		

		void Queue( Planner* planner );
		void DeQueue( Planner* planner );

	private:

		static const unsigned int s_kNoSolution = UINT_MAX;

		Planner* m_Current;

		WaypointGraph* m_WaypointGraph;

		std::unordered_map<unsigned int, Planner> m_Visited;

		std::deque<Planner*> m_Open;

		const WaypointGraph::WVertex* m_Goal;

		std::vector<Aabb> m_DebugDrawVertices;

		unsigned int m_Solution;

		unsigned int m_Start = 0;
		unsigned int m_End = 0;
	};
}
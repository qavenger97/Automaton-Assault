#include "pch.h"
#include "WaypointSearch.h"
#include "Math\SimpleMath.h"
#include "TimeManager.h"
#include "Renderer\DebugRenderer.h"

namespace Hourglass
{
	void WaypointSearch::DrawDebug()
	{
		Vector4 debugColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		Aabb debugAabb;
		debugAabb.pMin = m_WaypointGraph->operator[]( m_Start ).m_Waypoint - Vector3( 0.5f, 0.5f, 0.5f );
		debugAabb.pMax = m_WaypointGraph->operator[]( m_Start ).m_Waypoint + Vector3( 0.5f, 0.5f, 0.5f );
		DebugRenderer::DrawAabb( debugAabb, debugColor );

		debugColor = Vector4( 0.0f, 0.0f, 0.0f, 1.0f );
		debugAabb.pMin = m_WaypointGraph->operator[]( m_End ).m_Waypoint - Vector3( 0.5f, 0.5f, 0.5f );
		debugAabb.pMax = m_WaypointGraph->operator[]( m_End ).m_Waypoint + Vector3( 0.5f, 0.5f, 0.5f );
		DebugRenderer::DrawAabb( debugAabb, debugColor );
		for (auto& vertex : m_DebugDrawVertices)
		{
			DebugRenderer::DrawAabb( vertex, debugColor );
		}
	}

	void WaypointSearch::Init( WaypointGraph* waypointGraph )
	{
		m_WaypointGraph = waypointGraph;
	}

	void WaypointSearch::Configure( unsigned int start, const unsigned int end )
	{
		if (start > 1000000)
			return;
		m_Start = start;
		m_End = end;
		m_DebugDrawVertices.clear();
		m_Visited.clear();
		m_Solution = s_kNoSolution;

		m_Goal = &(*m_WaypointGraph)[end];

		Planner& startPlanner = m_Visited[start];
		startPlanner.m_Parent = nullptr;
		startPlanner.m_Vertex = &(*m_WaypointGraph)[start];
		startPlanner.m_GivenCost = 0.0f;
		
		startPlanner.m_FinalCost = DirectX::SimpleMath::Vector3::
			Distance( startPlanner.m_Vertex->m_Waypoint, m_Goal->m_Waypoint );
		Queue( &startPlanner );
	}

	void WaypointSearch::FindPath()
	{
		int i = 3;
		while (!m_Open.empty())
		{
			SearchFront();
		}
	}

	void WaypointSearch::TimeslicedFindPath( float timeslice )
	{	
		// Handle time internally to the function for now, allows immedate multithreading
		double startTime, freqInv;

		{
			LARGE_INTEGER startTimeLI, freqLI;
			QueryPerformanceCounter( &startTimeLI );
			QueryPerformanceFrequency( &freqLI );

			freqInv = 1.0 / double( freqLI.QuadPart );
			startTime = double( startTimeLI.QuadPart ) * freqInv;
		}

		while (!m_Open.empty() && m_Solution == s_kNoSolution)
		{
			SearchFront();

			double timeTaken;

			{
				LARGE_INTEGER timeTakenLI;
				QueryPerformanceCounter( &timeTakenLI );
				timeTaken = double( timeTakenLI.QuadPart ) * freqInv - startTime;
			}

			if (timeTaken >= timeslice)
			{
				break;
			}
		}		
	}


	void WaypointSearch::SearchFront()
	{
		m_Current = m_Open.back();
		m_Open.pop_back();
		WaypointGraph::WVertex* vertex = m_Current->m_Vertex;

		// Solution found / Reached Goal?
		if (vertex == m_Goal)
		{
			if (!m_Current->m_Parent)
			{
				for (unsigned int i = 0; i < m_WaypointGraph->Size(); ++i)
				{
					if (&(*m_WaypointGraph)[i] == m_Current->m_Vertex)
					{
						m_Solution = i;
						return;
					}
				}
			}

			while (m_Current->m_Parent->m_Parent)
			{
				m_Current = m_Current->m_Parent;
			}

			for (unsigned int i = 0; i < m_Current->m_Parent->m_Vertex->m_Edges.size(); ++i)
			{
				if (m_Current->m_Vertex == &(*m_WaypointGraph)[m_Current->m_Parent->m_Vertex->m_Edges[i].m_ToVertex])
				{
					m_Solution = m_Current->m_Parent->m_Vertex->m_Edges[i].m_ToVertex;
				}
			}

			return;
		}

		std::vector<WaypointGraph::Edge>& currEdgeList = vertex->m_Edges;
		unsigned int currNumEdges = unsigned int( currEdgeList.size() );

		for (unsigned int i = 0; i < currNumEdges; ++i)
		{
			unsigned short toEndpointVertex = currEdgeList[i].m_ToVertex;

			float newGivenCost = m_Current->m_GivenCost + currEdgeList[i].m_Distance;

			Aabb debugAabb;
			debugAabb.pMin = m_WaypointGraph->operator[](toEndpointVertex).m_Waypoint - Vector3( 0.5f, 0.5f, 0.5f );
			debugAabb.pMax = m_WaypointGraph->operator[]( toEndpointVertex ).m_Waypoint + Vector3( 0.5f, 0.5f, 0.5f );
			m_DebugDrawVertices.push_back( debugAabb );

			if (m_Visited.find( toEndpointVertex ) == m_Visited.end()) // no duplicate
			{
				Planner& successor = m_Visited[toEndpointVertex];
				successor.m_Vertex = &(*m_WaypointGraph)[toEndpointVertex];

				// Calculate successor costs
				float heuristicCost = DirectX::SimpleMath::Vector3::
					Distance( successor.m_Vertex->m_Waypoint, m_Goal->m_Waypoint );
				successor.m_FinalCost = successor.m_GivenCost = newGivenCost;
				successor.m_FinalCost += heuristicCost;

				successor.m_Parent = m_Current;
				Queue( &successor );
			}
			else // duplicate
			{
				Planner& successor = m_Visited[toEndpointVertex];
				if (newGivenCost < successor.m_GivenCost)
				{
					DeQueue( &successor );

					// calcuate successor costs
					float heuristicCost = successor.m_FinalCost - successor.m_GivenCost;
					successor.m_FinalCost = successor.m_GivenCost = newGivenCost;
					successor.m_FinalCost += heuristicCost;

					successor.m_Parent = m_Current;

					Queue( &successor );
				}
			}
		}
	}

	bool WaypointSearch::CostCmp( Planner * const & lhs, Planner * const & rhs )
	{
		return lhs->m_FinalCost > rhs->m_FinalCost;
	}

	void WaypointSearch::Queue( Planner * planner )
	{
		m_Open.insert( std::upper_bound( m_Open.begin(), m_Open.end(), planner, CostCmp ), planner );
	}

	void WaypointSearch::DeQueue( Planner * planner )
	{
		m_Open.erase( std::remove( m_Open.begin(), m_Open.end(), planner ), m_Open.end() );
	}

	unsigned int WaypointSearch::GetSolutionWaypoint()
	{
		return m_Solution;
	}
}

#pragma once

namespace Hourglass
{
	class WaypointGraph
	{
	public:

		struct Edge
		{
			unsigned int m_ToVertex;
			float m_Distance;
		};

		struct WVertex
		{
			Vector3 m_Waypoint;
			std::vector<Edge> m_Edges;

			explicit WVertex( const Vector3& waypoint ) { m_Waypoint = waypoint; }

			void AddEdge( const Edge& edge )
			{
				m_Edges.push_back( edge );
			}
		};

		WVertex& WaypointGraph::operator[]( unsigned int _index )
		{
			return m_Vertices[_index];
		}

		const WVertex& WaypointGraph::operator[]( unsigned int _index ) const
		{
			return m_Vertices[_index];
		}

		unsigned int AddVertex( const Vector3& w );
		unsigned int FindNearestVertex( const Vector3 & v );
		void Clear();
		unsigned int Size() { return unsigned int(m_Vertices.size()); }

		void DrawDebugEdges();

	private:

		std::vector<WVertex> m_Vertices;
	};
}
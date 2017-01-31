#include "pch.h"

#include "WaypointGraph.h"
#include "Renderer\DebugRenderer.h"
#include "TimeManager.h"

namespace Hourglass
{
	unsigned int WaypointGraph::AddVertex( const Vector3& w )
	{
		m_Vertices.push_back( WVertex( w ) );
		return unsigned int(m_Vertices.size() - 1);
	}

	void WaypointGraph::Clear()
	{
		m_Vertices.clear();
	}

	void WaypointGraph::DrawDebugEdges()
	{
#if _DEBUG
		Vector4 debugColor = { 1.0f, 0.1f, 0.1f, 1.0f };
		static unsigned int currDebug = unsigned int(m_Vertices.size())-1;
		static float time = 0.0f;
		time += g_Time.Delta();

		if (time >= 2.0f)
		{
			--currDebug;
			if (currDebug == m_Vertices.size())
			{
				currDebug = 0;
			}

			time = 0.0f;
		}
		for (unsigned int i = 0; i <m_Vertices.size(); ++i)
		{
			if (i == currDebug)
			{
				Aabb debugAabb;
				debugAabb.pMin = m_Vertices[i].m_Waypoint - Vector3( 0.5f, 0.5f, 0.5f );
				debugAabb.pMax = m_Vertices[i].m_Waypoint + Vector3( 0.5f, 0.5f, 0.5f );
				DebugRenderer::DrawAabb( debugAabb, debugColor );

				for (Edge& edge : m_Vertices[i].m_Edges)
				{
					Aabb debugAabbEdge;
					debugAabbEdge.pMin = m_Vertices[edge.m_ToVertex].m_Waypoint - Vector3( 0.5f, 0.5f, 0.5f );
					debugAabbEdge.pMax = m_Vertices[edge.m_ToVertex].m_Waypoint + Vector3( 0.5f, 0.5f, 0.5f );
					DebugRenderer::DrawAabb( debugAabbEdge, debugColor );
				}
			}
			/*
			Aabb debugAabb;
			debugAabb.pMin = vertex.m_Waypoint - Vector3( 0.5f, 0.5f, 0.5f );
			debugAabb.pMax = vertex.m_Waypoint + Vector3( 0.5f, 0.5f, 0.5f );
			DebugRenderer::DrawAabb( debugAabb, debugColor );
			*/
			for (Edge& edge : m_Vertices[i].m_Edges)
			{
				DebugRenderer::DrawLine(m_Vertices[i].m_Waypoint, m_Vertices[edge.m_ToVertex].m_Waypoint, debugColor, debugColor);
			}
		}
#endif
	}

	unsigned int WaypointGraph::FindNearestVertex( const Vector3 & v )
	{
		XMVECTOR a = XMLoadFloat3( &v );
		XMVECTOR shortDistSqr = XMVectorSplatInfinity();
		unsigned int nearestVertex = 0;

		for (unsigned int i = 0; i < m_Vertices.size(); ++i)
		{
			XMVECTOR b = XMLoadFloat3( &(m_Vertices[i].m_Waypoint) ) - a;
			XMVECTOR distSqr = XMVector3Dot( b, b );
			
			XMVECTOR distCmpVec = XMVectorLess( distSqr, shortDistSqr );
			float distCmpFlt;
			XMStoreFloat( &distCmpFlt, distCmpVec );
			if (distCmpFlt)
			{
				shortDistSqr = distSqr;
				nearestVertex = i;
			}
		}

		return nearestVertex;
	}

}

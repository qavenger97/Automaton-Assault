#include "pch.h"
#include "AISystem.h"
#include "Core\EntityAssembler.h"
#include "Core\AssembledBehaviorReg.h"
#include "Core\Entity.h"
#include "Component\Transform.h"
#include <io.h>
#include "Core\InputManager.h"
#include "Renderer\DebugRenderer.h"
#include "Component\Motor.h"
#include "Core\TimeManager.h"

namespace Hourglass
{
	static bool s_bDrawDebugAIDestination = false;

	void AISystem::Init()
	{
		ComponentPoolInit( SID(Separation), m_Separations, &m_SeparationPool, s_kMaxSeparations );
		ComponentPoolInit( SID(BehaviorTree), m_BehaviorTrees, &m_BehaviorTreePool, s_kMaxBehaviorTrees );
		ComponentPoolInit( SID(WaypointAgent), m_WaypointAgents, &m_WaypointAgentPool, s_kMaxWaypointAgents );
		
		BehaviorPoolInit( SID(Sequence), m_Sequences, &m_SequencePool, s_kMaxBehaviorTrees );
		BehaviorPoolInit( SID(Selector), m_Selectors, &m_SelectorPool, s_kMaxSelectors );
		BehaviorPoolInit( SID(RepeatUntilFail), m_Rufs, &m_RufPool, s_kMaxRufs );
		BehaviorPoolInit( SID(Inverter), m_Inverters, &m_InverterPool, s_kMaxInverters );
		BehaviorPoolInit( SID(Parallel), m_Parallels, &m_ParallelPool, s_kMaxParallels );	
	}

	void AISystem::Start()
	{
		m_WaypointSearch.Init( &m_WaypointGraph );
		m_InverterPool.Start();
		m_ParallelPool.Start();
		m_RufPool.Start();
		m_SelectorPool.Start();
		m_SequencePool.Start();
	}

	void AISystem::LoadBehaviors()
	{
		LoadBehaviorsFromXML();
	}

	void AISystem::Update()
	{
		m_WaypointGraph.DrawDebugEdges();

		float dt = hg::g_Time.Delta();		

		for (unsigned int i = 0; i < s_kMaxWaypointAgents; ++i)
		{
			if (m_WaypointAgents[i].IsAlive() && m_WaypointAgents[i].IsEnabled() && !m_WaypointAgents[i].IsDormant())
			{			
				m_WaypointAgents[i].Update();

				if (m_WaypointAgents[i].DoesNeedPath())
				{
					if (m_WaypointAgents[i].DoesNeedCurrentWaypoint())
					{
						const Vector3& currentPos = m_WaypointAgents[i].GetEntity()->GetTransform()->GetWorldPosition();
						unsigned int toCurrentWaypoint = m_WaypointGraph.FindNearestVertex( currentPos );
						m_WaypointAgents[i].SetCurrentWaypoint( toCurrentWaypoint );
					}
					else if (m_WaypointAgents[i].DoesNeedNextWaypoint())
					{
						Vector3 destination = m_WaypointAgents[i].GetDestination();
						unsigned int toDestination = m_WaypointGraph.FindNearestVertex( destination );
						m_WaypointSearch.Configure( m_WaypointAgents[i].GetCurrentWaypoint(), toDestination );

						m_WaypointSearch.FindPath();						
						
						//m_WaypointSearch.FindPath();
						
						unsigned int solutionWaypoint = m_WaypointSearch.GetSolutionWaypoint();
						m_WaypointAgents[i].SetNextWaypoint( solutionWaypoint );
						m_WaypointAgents[i].SetWaypointPosition( m_WaypointGraph[solutionWaypoint].m_Waypoint );
					}							
				}				
				//m_WaypointSearch.DrawDebug();

				if (s_bDrawDebugAIDestination && DEBUG_RENDER)
				{
					if (m_WaypointAgents[i].GetCurrentWaypoint() < 3000)
					{
						Vector4 debugColor = { 0.0f, 0.0f, 1.0f, 1.0f };
						Aabb debugAabb;
						debugAabb.pMin = m_WaypointGraph[m_WaypointAgents[i].GetCurrentWaypoint()].m_Waypoint - Vector3(0.5f, 0.5f, 0.5f);
						debugAabb.pMax = m_WaypointGraph[m_WaypointAgents[i].GetCurrentWaypoint()].m_Waypoint + Vector3(0.5f, 0.5f, 0.5f);
						DebugRenderer::DrawAabb(debugAabb, debugColor);
					}

					if (m_WaypointAgents[i].GetNextWaypoint() < 3000)
					{
						Vector4 debugColor = { 0.0f, 0.5f, 1.0f, 1.0f };
						Aabb debugAabb;
						debugAabb.pMin = m_WaypointGraph[m_WaypointAgents[i].GetNextWaypoint()].m_Waypoint - Vector3(0.5f, 0.5f, 0.5f);
						debugAabb.pMax = m_WaypointGraph[m_WaypointAgents[i].GetNextWaypoint()].m_Waypoint + Vector3(0.5f, 0.5f, 0.5f);
						DebugRenderer::DrawAabb(debugAabb, debugColor);
					}
				}
			}
		}

		for (unsigned int i = 0; i < s_kMaxBehaviorTrees; ++i)
		{
			if (m_BehaviorTrees[i].IsAlive() && m_BehaviorTrees[i].IsEnabled())
			{
				m_BehaviorTrees[i].Update();
			}
		}

		for (unsigned int i = 0; i < s_kMaxSeparations; ++i)
		{
			Separation& separation = m_Separations[i];

			if (separation.IsAlive() && separation.IsEnabled())
			{
				Motor& motor = *separation.GetEntity()->GetComponent<Motor>();
				float speed = motor.GetMaxSpeed();

				Vector3 acc = CalcSeparationAcc( &separation );

				Vector3 pos = separation.GetEntity()->GetTransform()->GetWorldPosition();
				//DebugRenderer::DrawLine( pos, pos + acc, Vector4( 0.0f, 0.0f, 1.0f, 1.0f ) );
				acc += separation.GetDesiredMove();
				acc *= speed * dt;

				Vector3 velocity = motor.GetDesiredMove() * motor.GetSpeed() + acc * 10;
				
				

				float magnitude = velocity.Length();
				motor.SetSpeed( min( speed, magnitude ) );

				velocity.Normalize();

				//DebugRenderer::DrawLine( pos, pos + velocity, Vector4( 1.0f, 0.0f, 0.0f, 1.0f ) );
				motor.SetDesiredMove( velocity );
			}
		}
	}

	void AISystem::LoadBehaviorsFromXML()
	{
		char folder[FILENAME_MAX] = "Assets\\XML\\Behavior\\";
		char filter[FILENAME_MAX];
		strcpy_s( filter, folder );
		strcat_s( filter, "*.xml" );

		WIN32_FIND_DATAA fileData;
		HANDLE searchHandle = FindFirstFileA( filter, &fileData );
		if (searchHandle != INVALID_HANDLE_VALUE) 
		{
			do 
			{
				// Piece together the full path from folder + file name
				char fullpath[FILENAME_MAX];
				strcpy_s( fullpath, folder );
				strcat_s( fullpath, fileData.cFileName );

				// Load a single document from the behaviors folder
				LoadBehaviorTreeXML( fullpath );

			} while (FindNextFileA( searchHandle, &fileData ));

			FindClose( searchHandle );
		}		
	}

	void AISystem::LoadBehaviorTreeXML( const char* path )
	{
		tinyxml2::XMLDocument doc;

		tinyxml2::XMLError errorResult = doc.LoadFile( path );
		assert( errorResult == tinyxml2::XML_SUCCESS );

		// Get the behavior tree
		tinyxml2::XMLElement * xmlTree = doc.FirstChildElement();
		assert( strcmp(xmlTree->Value(), "BehaviorTree") == 0 );

		const char* behaviorTreeName = xmlTree->Attribute( "name" );

		// Get the behavior tree root
		tinyxml2::XMLElement* xmlRoot = xmlTree->FirstChildElement();

		// There can only be one root, no siblings allowed for this one
		assert( xmlRoot->NextSibling() == NULL );		
		
		IBehavior* root = RecursiveLoadBehaviorXML( xmlRoot );
		hg::AssembledBehaviorReg reg( WSID(behaviorTreeName), root );
	}

	IBehavior* AISystem::RecursiveLoadBehaviorXML( tinyxml2::XMLElement* behaviorXML )
	{
		IBehavior* behavior = BehaviorFactory::GetBehaviorToAssemble( WSID( behaviorXML->Value() ) );
		
		tinyxml2::XMLElement* behaviorChildXML = behaviorXML->FirstChildElement();
		for (; behaviorChildXML; behaviorChildXML = behaviorChildXML->NextSiblingElement())
		{
			behavior->AddBehavior( RecursiveLoadBehaviorXML( behaviorChildXML ) );
		}

		behavior->LoadFromXML( behaviorXML );

		return behavior;
	}

	Vector3 AISystem::CalcSeparationAcc( Separation* curr )
	{
		Vector3 sum = Vector3::Zero;

		Entity* currEntity = curr->GetEntity();
		const Vector3& currPos = currEntity->GetTransform()->GetWorldPosition();

		for (unsigned int i = 0; i < s_kMaxSeparations; ++i)
		{
			Separation* other = &m_Separations[i];
			if (other == curr || !other->IsAlive() || !other->IsEnabled())
				continue;
			
			Entity* otherEntity = other->GetEntity();
			const Vector3& otherPos = otherEntity->GetTransform()->GetWorldPosition();

			Vector3 toVector = currPos - otherPos;
			toVector.y = 0.0f;
			float dist = toVector.Length();
			float otherSafeRadius = other->GetSafeRadius();
			float safeDist = curr->GetSafeRadius() + other->GetSafeRadius();

			if (dist < safeDist)
			{
				toVector.Normalize();
				toVector *= (safeDist - dist) / safeDist;
				sum += toVector;
			}
		}

		if (sum.Length() > 1.0f)
			sum.Normalize();

		sum *= curr->GetSeparationStrength();
		return sum;
	}

	void AISystem::LoadWaypoints( tinyxml2::XMLElement* waypointsXML )
	{
		// Process all waypoints
		tinyxml2::XMLElement* xmlWaypoint = waypointsXML->FirstChildElement();

		while (xmlWaypoint != nullptr)
		{
			tinyxml2::XMLElement* xmlPosition = xmlWaypoint->FirstChildElement( "Position" );
			Vector3 position;
			xmlPosition->QueryFloatAttribute( "x", &position.x );
			xmlPosition->QueryFloatAttribute( "y", &position.y );
			xmlPosition->QueryFloatAttribute( "z", &position.z );

			unsigned int vertIndex = m_WaypointGraph.AddVertex( position );
			WaypointGraph::WVertex& newVertex = m_WaypointGraph[vertIndex];
			
			tinyxml2::XMLElement* xmlEdges = xmlWaypoint->FirstChildElement( "Edges" );
			tinyxml2::XMLElement* xmlEdge = xmlEdges->FirstChildElement( "Edge" );
			while (xmlEdge != nullptr)
			{
				WaypointGraph::Edge edge;
				xmlEdge->QueryUnsignedAttribute( "vertex", &edge.m_ToVertex );
				xmlEdge->QueryFloatAttribute( "distance", &edge.m_Distance );
				newVertex.AddEdge( edge );
				xmlEdge = xmlEdge->NextSiblingElement();
			}
			
			xmlWaypoint = xmlWaypoint->NextSiblingElement();
		}
	}
}
#pragma once

#include "Core\BehaviorRegister.h"
#include "Component\BehaviorTree.h"
#include "Core\ComponentPool.h"
#include "Core\BehaviorPool.h"
#include "Behavior\Selector.h"
#include "Behavior\Inverter.h"
#include "Behavior\RepeatUntilFail.h"
#include "Behavior\Parallel.h"
#include "Component\WaypointAgent.h"
#include "ISystem.h"
#include "Core\WaypointGraph.h"
#include "Core\WaypointSearch.h"
#include "Component\Separation.h"

namespace Hourglass
{
	class AISystem : public ISystem
	{
	public:
		void Init();

		void Start();

		void LoadBehaviors();

		void LoadWaypoints( tinyxml2::XMLElement* waypointsXML );

		void Update();

	private:

		void LoadBehaviorsFromXML();
		void LoadBehaviorTreeXML( const char* path );
		IBehavior* RecursiveLoadBehaviorXML( tinyxml2::XMLElement* behaviorXML );

		Vector3 CalcSeparationAcc( Separation* acc );

		template <typename T>
		void BehaviorPoolInit(StrID name, T* data, BehaviorPool<T>* pool, unsigned int size );

		static const unsigned int s_kMaxSeparations = 256;
		Separation m_Separations[s_kMaxSeparations];
		ComponentPool<Separation> m_SeparationPool;

		static const unsigned int s_kMaxWaypointAgents = 256;
		WaypointAgent m_WaypointAgents[s_kMaxWaypointAgents];
		ComponentPool<WaypointAgent> m_WaypointAgentPool;

		static const unsigned int s_kMaxBehaviorTrees = 256;
		BehaviorTree m_BehaviorTrees[s_kMaxBehaviorTrees];
		ComponentPool<BehaviorTree> m_BehaviorTreePool;

		// Behavior Tree Data & Pools

		static const unsigned int s_kMaxSequences = 256;
		Sequence m_Sequences[s_kMaxSequences];
		BehaviorPool<Sequence> m_SequencePool;

		static const unsigned int s_kMaxSelectors = 256;
		Selector m_Selectors[s_kMaxSelectors];
		BehaviorPool<Selector> m_SelectorPool;

		static const unsigned int s_kMaxRufs = 256;
		RepeatUntilFail m_Rufs[s_kMaxRufs];
		BehaviorPool<RepeatUntilFail> m_RufPool;

		static const unsigned int s_kMaxInverters = 256;
		Inverter m_Inverters[s_kMaxInverters];
		BehaviorPool<Inverter> m_InverterPool;

		static const unsigned int s_kMaxParallels = 256;
		Parallel m_Parallels[s_kMaxParallels];
		BehaviorPool<Parallel> m_ParallelPool;

		WaypointGraph m_WaypointGraph;
		WaypointSearch m_WaypointSearch;
	};

	template<typename T>
	inline void AISystem::BehaviorPoolInit( StrID name, T * data, BehaviorPool<T>* pool, unsigned int size )
	{
		BehaviorRegister<T> reg( name, pool );
		pool->Init( data, size );
	}
}
#include "pch.h"

#include "BehaviorFactory.h"
#include "BehaviorPool.h"

namespace Hourglass
{
	std::unordered_map<StrID, BaseBehaviorPool*>* BehaviorFactory::m_Pools = nullptr;
	std::unordered_map< StrID, IBehavior*>* BehaviorFactory::m_AssembledRoots = nullptr;
	std::unordered_map< StrID, IBehavior*(*)()>* BehaviorFactory::m_BehaviorToAssembleFuncs = nullptr;
	std::vector<IBehavior*>* BehaviorFactory::m_AssembledBehaviors = nullptr;

	IBehavior* BehaviorFactory::GetAssembledRoot( StrID name )
	{
		std::unordered_map<StrID, IBehavior*>::iterator it =
			m_AssembledRoots->find( name );

		assert( it != m_AssembledRoots->end() );

		if (it == m_AssembledRoots->end())
		{
			return nullptr;
		}

		return it->second->MakeCopy();
	}

	IBehavior * BehaviorFactory::GetBehaviorToAssemble( StrID name )
	{
		std::unordered_map<StrID, IBehavior*(*)()>::iterator it =
			m_BehaviorToAssembleFuncs->find( name );

		assert( it != m_BehaviorToAssembleFuncs->end() );

		if (it == m_BehaviorToAssembleFuncs->end())
		{
			return nullptr;
		}

		return it->second();
	}

	void BehaviorFactory::Shutdown()
	{
		if (m_Pools)
		{
			delete m_Pools;
		}
		if (m_AssembledRoots)
		{
			delete m_AssembledRoots;
		}
		if (m_BehaviorToAssembleFuncs)
		{
			delete m_BehaviorToAssembleFuncs;
		}
		if (m_AssembledBehaviors)
		{
			for (std::vector<IBehavior*>::iterator it = m_AssembledBehaviors->begin();
			it != m_AssembledBehaviors->end(); ++it)
			{
				delete(*it);
			}

			delete m_AssembledBehaviors;
		}
	}

	IBehavior* BehaviorFactory::GetFreeBehavior( StrID name )
	{
		std::unordered_map<StrID, BaseBehaviorPool*>::iterator it =
			m_Pools->find( name );

		assert( it != m_Pools->end() );

		if (it == m_Pools->end())
		{
			return nullptr;
		}

		IBehavior* ret = it->second->GetFree();
		return ret;
	}

	void BehaviorFactory::Register( StrID name, BaseBehaviorPool* pool, IBehavior*(*behaviorToAssembleFunc)() )
	{
		if (m_Pools == nullptr)
		{
			m_Pools = new std::unordered_map<StrID, BaseBehaviorPool*>();
		}

		m_Pools->insert( std::make_pair( name, pool ) );

		if (m_BehaviorToAssembleFuncs == nullptr)
		{
			m_BehaviorToAssembleFuncs = new std::unordered_map<StrID, IBehavior*(*)()>();
		}

		m_BehaviorToAssembleFuncs->insert( std::make_pair( name, behaviorToAssembleFunc ) );
	}

	void BehaviorFactory::Register( StrID name, IBehavior* root )
	{
		if (m_AssembledRoots == nullptr)
		{
			m_AssembledRoots = new std::unordered_map<StrID, IBehavior*>();
		}

		m_AssembledRoots->insert( std::make_pair( name, root ) );		
	}
}
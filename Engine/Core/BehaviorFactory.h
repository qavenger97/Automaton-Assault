#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include "StrID.h"

namespace Hourglass
{
	class IBehavior;
	class BaseBehaviorPool;
	class BehaviorTree;

	class BehaviorFactory
	{
	public:

		~BehaviorFactory() {}

		static IBehavior* GetFreeBehavior( StrID name );

		/**
		* Get a fully assembled tree's root, given the
		* name of the pre-assembled root
		*/
		static IBehavior* GetAssembledRoot( StrID name );

		/**
		* Assembled behavior trees are stored separately
		* because they do not get updated.
		*	@param name - the name of the behavior to return
		*	@return - behavior to be used to create an assembled behavior tree
		*/
		static IBehavior* GetBehaviorToAssemble( StrID name );

		static int GetBehaviorID() { return m_BehaviorIdCounter++; }

		static void Shutdown();

	protected:

		void Register( StrID name, BaseBehaviorPool* pool, IBehavior*(*behaviorToAssembleFunc)() );
		void Register( StrID name, IBehavior* root );

		template<typename T>
		static IBehavior* CreateInstanceToAssemble();

	private:

		static int32_t m_BehaviorIdCounter;
		static std::unordered_map< StrID, BaseBehaviorPool*>* m_Pools;
		static std::unordered_map< StrID, IBehavior*>* m_AssembledRoots;
		static std::unordered_map< StrID, IBehavior*(*)()>* m_BehaviorToAssembleFuncs;
		static std::vector<IBehavior*>* m_AssembledBehaviors;
	};

	template<typename T>
	inline IBehavior* BehaviorFactory::CreateInstanceToAssemble()
	{
		T* behaviorToAssemble = new T;

		if (m_AssembledBehaviors == nullptr)
		{
			m_AssembledBehaviors = new std::vector<IBehavior*>();
		}

		m_AssembledBehaviors->push_back( behaviorToAssemble );

		return behaviorToAssemble;
	}

}
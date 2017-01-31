#pragma once

#include "Core/BehaviorRegister.h"

namespace Hourglass
{

	class Entity;

	class IBehavior
	{
	public:

		enum Result
		{
			kSUCCESS,
			kFAILURE,
			kRUNNING
		};

		virtual ~IBehavior() { }

		virtual void LoadFromXML( tinyxml2::XMLElement* data ) { };

		virtual void AddBehavior( IBehavior* behavior ) = 0;

		virtual void Start() { }

		virtual void DestroyDerived() = 0;		

		void Destroy() { m_IsAlive = false; DestroyDerived(); }

		bool IsAlive() { return m_IsAlive != 0; }

		void SetAlive( bool alive ) { m_IsAlive = alive; }

		static IBehavior* Create( StrID name );

		/**
		* Get the root of an entire assembled behavior tree
		*	@param name - name of the behavior tree to assemble
		*/
		static IBehavior* AssembleRoot( StrID name );

		virtual bool IsRunningChild() const = 0;

		virtual Result Run( Entity* entity ) = 0;

		virtual void Reset() = 0;

		/**
		* Make a copy of this component
		*/
		virtual IBehavior* MakeCopy() const = 0;

	private:

		int32_t m_IsAlive : 1;
	};
}
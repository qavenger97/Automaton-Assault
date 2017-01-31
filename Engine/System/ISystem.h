#pragma once

#include "Core/ComponentPool.h"
#include "Core/ComponentRegister.h"

namespace Hourglass
{
	class ISystem
	{
	protected:
		
		template <typename T>
		void ComponentPoolInit( StrID name, T* data, ComponentPool<T>* pool, 
			unsigned int size, unsigned int flags = CompRegFlags::kNone );
	};

	template<typename T>
	inline void ISystem::ComponentPoolInit( StrID name, T * data, ComponentPool<T>* pool, 
		unsigned int size, unsigned int flags )
	{
		ComponentRegister<T> reg( name, pool, flags );
		pool->Init( data, size );
	}
}
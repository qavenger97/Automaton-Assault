#include "pch.h"

#include "ComponentFactory.h"
#include "ComponentPool.h"

namespace Hourglass
{
	int32_t ComponentFactory::m_GameCIdCounter = 1000;
	int32_t ComponentFactory::m_SystemCIdCounter = 0;
	std::unordered_map<StrID, BaseComponentPool*>* ComponentFactory::m_Pools = nullptr;
	std::vector<BaseComponentPool*>* ComponentFactory::m_PoolsWithAutoUpdate = nullptr;
	std::unordered_map< StrID, IComponent*(*)()>* ComponentFactory::m_ComponentToAssembleFuncs = nullptr;
	std::vector<IComponent*>* ComponentFactory::m_AssembledComponents = nullptr;

	void ComponentFactory::Shutdown()
	{
		if (m_Pools)
		{
			delete m_Pools;
		}
		if (m_PoolsWithAutoUpdate)
		{
			delete m_PoolsWithAutoUpdate;
		}
		if (m_ComponentToAssembleFuncs)
		{
			delete m_ComponentToAssembleFuncs;
		}
		if (m_AssembledComponents)
		{
			for (std::vector<IComponent*>::iterator it = m_AssembledComponents->begin();
			it != m_AssembledComponents->end(); ++it)
			{
				delete(*it);
			}

			delete m_AssembledComponents;
		}		
	}

	bool ComponentFactory::IsComponentTypeRegistered(StrID name)
	{
		return (m_Pools->find(name) != m_Pools->end());
	}

	IComponent* ComponentFactory::GetFreeComponent(StrID name)
	{
		std::unordered_map<StrID, BaseComponentPool*>::iterator it =
			m_Pools->find( name );

		//assert( it != m_Pools->end() );

		if (it == m_Pools->end())
		{
			return nullptr;
		}

		return it->second->GetFree();
	}

	void ComponentFactory::Register( StrID name, BaseComponentPool* pool, IComponent*(*componentToAssembleFunc)(), unsigned int compRegFlags )
	{
		if (m_Pools == nullptr)
		{
			m_Pools = new std::unordered_map<StrID, BaseComponentPool*>();
		}

		m_Pools->insert( std::make_pair( name, pool ) );

		if (compRegFlags & CompRegFlags::kAutoUpdate)
		{
			if (m_PoolsWithAutoUpdate == nullptr)
			{
				m_PoolsWithAutoUpdate = new std::vector<BaseComponentPool*>();
			}
			m_PoolsWithAutoUpdate->push_back( pool );
		}

		if (m_ComponentToAssembleFuncs == nullptr)
		{
			m_ComponentToAssembleFuncs = new std::unordered_map<StrID, IComponent*(*)()>();
		}

		m_ComponentToAssembleFuncs->insert( std::make_pair( name, componentToAssembleFunc ) );
	}

	void ComponentFactory::RunStoredUpdateFunctions()
	{
		if (m_PoolsWithAutoUpdate == nullptr)
		{
			return;
		}

		std::vector<BaseComponentPool*>::iterator it = m_PoolsWithAutoUpdate->begin();
		for (; it != m_PoolsWithAutoUpdate->end(); ++it)
		{
			(*it)->UpdatePooled();
		}
	}

	void ComponentFactory::RunStoredStartFunctions()
	{
		if (m_PoolsWithAutoUpdate == nullptr)
		{
			return;
		}

		std::vector<BaseComponentPool*>::iterator it = m_PoolsWithAutoUpdate->begin();
		for (; it != m_PoolsWithAutoUpdate->end(); ++it)
		{
			(*it)->StartPooled();
		}
	}

	IComponent * ComponentFactory::GetComponentToAssemble( StrID name )
	{
		std::unordered_map<StrID, IComponent*(*)()>::iterator it =
			m_ComponentToAssembleFuncs->find( name );

		assert( it != m_ComponentToAssembleFuncs->end() );

		if (it == m_ComponentToAssembleFuncs->end())
		{
			return nullptr;
		}

		return it->second();
	}
	
}
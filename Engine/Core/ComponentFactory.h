#pragma once

#include <unordered_map>
#include <string>
#include <vector>


namespace Hourglass
{
	class IComponent;
	class BaseComponentPool;

	namespace CompRegFlags
	{
		enum
		{
			kNone = (1 << 0),
			kAutoUpdate = (1 << 1)
			//kAutoInit = (1 << 2)			
		};		
	}

	class ComponentFactory
	{
	public:

		static bool IsComponentTypeRegistered( StrID name );

		static IComponent* GetFreeComponent( StrID name );

		static int GetGameComponentID() { return m_GameCIdCounter++; }

		static int GetSystemComponentID() { return m_SystemCIdCounter++; }

		static void Shutdown();

		/**
		* Assembled Components are stored separately
		* because they do not get updated.
		*	@param name - the name of the component to return
		*	@return - component to be used to create a component
		*/
		static IComponent* GetComponentToAssemble( StrID name );

	protected:

		void RunStoredUpdateFunctions();
		void RunStoredStartFunctions();

		void Register( StrID name, BaseComponentPool* pool, IComponent*(*componentToAssembleFunc)(), unsigned int compRegFlags );

		template<typename T>
		static IComponent* CreateInstanceToAssemble();

	private:

		static int32_t m_GameCIdCounter;
		static int32_t m_SystemCIdCounter;
		static std::unordered_map<StrID, BaseComponentPool*>* m_Pools;
		static std::vector<BaseComponentPool*>* m_PoolsWithAutoUpdate;
		static std::unordered_map< StrID, IComponent*(*)()>* m_ComponentToAssembleFuncs;
		static std::vector<IComponent*>* m_AssembledComponents;
	};

	template<typename T>
	inline IComponent* ComponentFactory::CreateInstanceToAssemble()
	{
		T* ComponentToAssemble = new T;

		if (m_AssembledComponents == nullptr)
		{
			m_AssembledComponents = new std::vector<IComponent*>();
		}

		m_AssembledComponents->push_back( ComponentToAssemble );

		return ComponentToAssemble;
	}
}

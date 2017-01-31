#pragma once

#define DECLARE_COMPONENT_POOL(type,size)			 \
		static const uint32_t s_kMax##type##s = size;\
		type m_##type##s[size];						 \
		ComponentPool<type> m_##type##Pool

#define INIT_COMPONENT_POOL(type) ComponentPoolInit( SID(type), m_##type##s, &m_##type##Pool, s_kMax##type##s )
	

#include "Component\IComponent.h"
namespace Hourglass
{
	class BaseComponentPool
	{
	public:

		virtual IComponent* GetFree() = 0;
		virtual void UpdatePooled() = 0;
		virtual void StartPooled() = 0;
	};

	template<typename T>
	class ComponentPool : public BaseComponentPool
	{
	public:

		void Init( T* data, unsigned int size );

		virtual void StartPooled();

		virtual IComponent* GetFree();

		virtual void UpdatePooled();

	private:

		T* m_data;
		uint32_t m_Size;
	};

	template<typename T>
	void ComponentPool<T>::Init( T* data, unsigned int size )
	{
		m_data = data;
		m_Size = size;

		for (unsigned int i = 0; i < m_Size; ++i)
		{
			data[i].Init();
		}
	}

	template<typename T>
	inline void ComponentPool<T>::StartPooled()
	{
		for (unsigned int i = 0; i < m_Size; ++i)
		{
			if(m_data[i].IsAlive())
				m_data[i].Start();
		}
	}

	template<typename T>
	IComponent* ComponentPool<T>::GetFree()
	{
		for (unsigned int i = 0; i < m_Size; ++i)
		{
			if (!m_data[i].IsAlive())
			{
				m_data[i].SetAlive( true );
				return (IComponent*)&m_data[i];
			}
		}

		return nullptr;
	}


	template<typename T>
	void ComponentPool<T>::UpdatePooled()
	{
		for (unsigned int i = 0; i < m_Size; ++i)
		{
			if (m_data[i].IsAlive() && m_data[i].IsEnabled())
			{
				m_data[i].Update();
			}
		}
	}
}



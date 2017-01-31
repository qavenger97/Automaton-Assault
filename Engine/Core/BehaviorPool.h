#pragma once

#include "Behavior\IBehavior.h"
namespace Hourglass
{
	class BaseBehaviorPool
	{
	public:

		virtual IBehavior* GetFree() = 0;
	};

	template<typename T>
	class BehaviorPool : public BaseBehaviorPool
	{
	public:

		void Init( T* data, unsigned int size );

		void Start();

		virtual IBehavior* GetFree();

	private:

		T* m_data;
		uint32_t m_Size;
	};

	template<typename T>
	void BehaviorPool<T>::Init( T* data, unsigned int size )
	{
		m_data = data;
		m_Size = size;
	}

	template<typename T>
	void BehaviorPool<T>::Start()
	{
		for (unsigned int i = 0; i < m_Size; ++i)
		{
			m_data[i].Start();
		}
	}

	template<typename T>
	IBehavior* BehaviorPool<T>::GetFree()
	{
		for (unsigned int i = 0; i < m_Size; ++i)
		{
			if (!m_data[i].IsAlive())
			{
				m_data[i].SetAlive( true );
				return &m_data[i];
			}
		}

		assert( 0 ); // We don't want to be here! Should mean out of memory
		return nullptr;
	}
}
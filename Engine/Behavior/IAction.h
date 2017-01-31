#pragma once

#include "Primitive.h"

namespace Hourglass
{
	class IAction : public Primitive
	{
	public:

		virtual void Start() { }

		virtual IBehavior::Result Run( Entity* entity ) final;

		virtual void DestroyDerived() { m_IsInitialized = false; }

		virtual void Init( Entity* entity ) { };
		
		virtual IBehavior::Result Update( Entity* entity ) = 0;

		virtual void Shutdown( Entity* entity ) { };

	private:

		uint32_t m_IsInitialized : 1;
	};
}
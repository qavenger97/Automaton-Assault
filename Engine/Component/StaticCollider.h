#pragma once

#include "Collider.h"
#include "Math/Aabb.h"

namespace Hourglass
{
	class StaticCollider : public Collider
	{
	public:
		void Init();
		void Update();

		bool IsStaticCollider() const { return true; }

		virtual int GetTypeID() const { return s_TypeID; }

		static uint32_t s_TypeID;

		/**
		* Make a copy of this component
		*/
		virtual IComponent* MakeCopyDerived() const;

	private:

	};
}
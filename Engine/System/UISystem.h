#pragma once

#pragma once

#include "Component\UIComponent.h"
#include "Core\ComponentPool.h"
#include "ISystem.h"

namespace Hourglass
{
	class UISystem : public ISystem
	{
	public:

		void Init();

		void Update();

	private:

		static const unsigned int s_kMaxUIComponents = 64;
		UIComponent m_UIComponents[s_kMaxUIComponents];
		ComponentPool<UIComponent> m_UIComponentPool;
	};
}
#include "pch.h"

#include "UISystem.h"
#include "Component/UIComponent.h"
#include "Core/ComponentRegister.h"

namespace Hourglass
{
	void UISystem::Init()
	{
		ComponentPoolInit( SID(UIComponent), m_UIComponents, &m_UIComponentPool, 
			s_kMaxUIComponents, 0 );
	}

	void UISystem::Update()
	{
		for (unsigned int i = 0; i < s_kMaxUIComponents; ++i)
		{
			if (m_UIComponents[i].IsAlive())
			{
				if (m_UIComponents[i].IsEnabled())
				{
					m_UIComponents[i].Update();
				}
			}
		}
	}
}
#pragma once

#include "Component\Transform.h"

namespace Hourglass
{
	class KeyFrame
	{
	public:
				
		float GetKeyTime() { return m_KeyTime; }
		Transform* GetTransform() { return &m_Transform; }
		const Transform* GetTransform() const { return &m_Transform; }

		void SetKeyTime( float keyTime ) { m_KeyTime = keyTime; }

	private:

		float m_KeyTime;
		Transform m_Transform;
	};
}
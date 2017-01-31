#include "GamePch.h"

#include "AI_HasHP.h"
#include "HealthModule.h"

extern bool g_bDebugAIBehaviors;

bool AI_HasHP::Check( Hourglass::Entity * entity )
{
	if (!m_Health)
	{
		m_Health = entity->GetComponent<Health>();
	}

	if (m_Health)
	{
		if (g_bDebugAIBehaviors)
		{
			char buf[1024];
			sprintf_s(buf, "\n\nHP: %f", m_Health->GetValue());
			hg::DevTextRenderer::DrawText_WorldSpace(buf, entity->GetPosition());
		}

		return m_Health->GetValue() > 0;
	}

	if (g_bDebugAIBehaviors)
	{
		hg::DevTextRenderer::DrawText_WorldSpace("\n\nAI has no HP", entity->GetPosition());
	}

	return false;
}

hg::IBehavior* AI_HasHP::MakeCopy() const
{
	AI_HasHP* copy = (AI_HasHP*)IBehavior::Create( SID( AI_HasHP ) );
	copy->m_Health = nullptr;
	return copy;
}

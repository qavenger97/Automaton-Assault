#include "GamePch.h"
#include "BossAgent.h"
#include "GameMessage.h"
IMPLEMENT_GAME_COMPONENT_TYPEID(BossAgent)

void BossAgent::Start()
{
	m_health = GetEntity()->GetComponent<Health>();
	m_Bar = hg::Entity::FindByName(SID(Boss_Indicator_Bar));
}

void BossAgent::Update()
{
	if (hg::g_Time.GetTimeScale() != 0)
	{
		float hpRatio = m_health->GetHealthRatio();

		float percent = ceilf(hpRatio * 10) * 0.1f;
		m_Bar->GetTransform()->SetScale(percent, 1, 1);
		if (hpRatio <= FLT_EPSILON)
		{
			LevelCompleteMessage win;
			hg::g_EntityManager.BroadcastMessage(&win);
		}
	}
}

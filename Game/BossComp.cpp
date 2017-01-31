#include "GamePch.h"
#include "BossComp.h"
#include "GameMessage.h"
IMPLEMENT_GAME_COMPONENT_TYPEID(BossComp)

hg::Entity* BossComp::s_agent = nullptr;

void BossComp::Start()
{
	if (!s_agent)
	{
		s_agent = hg::Entity::FindByName(SID(Boss_Agent));
	}
}

void BossComp::OnMessage(hg::Message * msg)
{
	if (msg->GetType() == GameMessageType::kDamage)
	{
		if (((DamageMessage*)msg)->GetDamageType() == DamageType::kDmgType_Bullet)
		{
			if (s_agent)
				s_agent->SendMsg(msg);
		}
	}
}

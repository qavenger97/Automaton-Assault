#include "GamePch.h"
#include "PowerUpShield.h"
#include "GameMessage.h"

uint32_t PowerUpShield::s_TypeID = hg::ComponentFactory::GetGameComponentID();

void PowerUpShield::Start()
{
	m_timer = 0;
}

void PowerUpShield::Update()
{
	if (m_timer >= m_duration)
	{
		PowerUpMessage msg(PowerUpType::kPowerUp_End | PowerUpType::kPowerUp_Shield, 5);
		GetEntity()->GetTransform()->GetParent()->GetEntity()->SendMsg(&msg);
		GetEntity()->Destroy();
		return;
	}
	m_timer += hg::g_Time.Delta();
}

void PowerUpShield::LoadFromXML(tinyxml2::XMLElement * data)
{
	data->QueryFloatAttribute("duration", &m_duration);
}

hg::IComponent * PowerUpShield::MakeCopyDerived() const
{
	PowerUpShield* rs = (PowerUpShield*)hg::IComponent::Create(SID(PowerUpShield));
	*rs = *this;
	return rs;
}

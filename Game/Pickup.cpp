#include "GamePch.h"
#include "Pickup.h"
#include "../Engine/Core/Entity.h"
#include "PlayerComp.h"
#include "Gate.h"
#include "GameMessage.h"
uint32_t Pickup::s_TypeID = hg::ComponentFactory::GetGameComponentID();

void Pickup::Start()
{
	player = hg::Entity::FindByName(SID(Player))->GetComponent<PlayerComp>();
}
void Pickup::Update()
{
	GetEntity()->GetTransform()->Rotate(0,hg::g_Time.Delta() * 2,0);
	if (player)
	{
		if (XMVectorGetX(XMVector3LengthSq(player->GetEntity()->GetTransform()->GetWorldPosition() - GetEntity()->GetTransform()->GetWorldPosition())) < ((m_type == Type::BOSSTRIGGER)? 200.0f : 1.5f))
		{
			bool picked = false;
			switch (m_type)
			{
			case Pickup::AMMO:
				picked = player->RestoreAmmo(m_value);
				break;
			case Pickup::HEALTH:
				picked = player->RestoreHP(m_value);
				break;
			case Pickup::RICOCHET:
			{
				PowerUpMessage msg(PowerUpType::kPowerUp_Ricochet, m_value);
				player->OnMessage(&msg);
				picked = true;
			}
				break;
			case Pickup::SHIELD:
			{
				PowerUpMessage msg(PowerUpType::kPowerUp_Shield, m_value);
				player->OnMessage(&msg);
				picked = true;
			}
				break;
			case Pickup::BOSSTRIGGER:
			{
				PowerUpMessage msg(PowerUpType::kPowerUp_Boss, m_value);
				hg::g_cameraSystem.SendMsg(&msg);
				picked = true;
			}
				break;
			default:
				break;
			}
			if (picked)
			{
				GetEntity()->Destroy();
			}
		}
	}
}

void Pickup::LoadFromXML(tinyxml2::XMLElement * data)
{
	std::string type;
	type = data->Attribute("type");
	if (type == "Ammo")
	{
		m_type = Type::AMMO;
	}
	else if (type == "Health")
	{
		m_type = Type::HEALTH;
	}
	else if (type == "Ricochet")
	{
		m_type = Type::RICOCHET;
	}
	else if (type == "Shield")
	{
		m_type = Type::SHIELD;
	}
	else if (type == "BossTrigger")
	{
		m_type = Type::BOSSTRIGGER;
	}
	else
	{
		m_type = Type::NUM_TYPE;
	}
	m_value = 0;
	data->QueryFloatAttribute("value", &m_value);
}

hg::IComponent * Pickup::MakeCopyDerived() const
{
	Pickup* rs = (Pickup*)hg::IComponent::Create(SID(Pickup));
	*rs = *this;
	return rs;
}

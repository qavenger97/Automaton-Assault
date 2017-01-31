#include "GamePch.h"

#include "HealthModule.h"
#include "Projectile.h"

uint32_t Health::s_TypeID = hg::ComponentFactory::GetGameComponentID();

void Health::LoadFromXML(tinyxml2::XMLElement* data)
{
	m_Value = 1.0f;
	data->QueryFloatAttribute("Value", &m_Value);
	m_maxValue = m_Value;
	bool indirect = false;
	data->QueryBoolAttribute("Indirect", &indirect);
	m_InDirect = indirect;
}

void Health::Init()
{
	m_FlashEffectEndTime = 0.0f;
	m_PlayingFlashEffect = false;
	m_Renderer = nullptr;
}

void Health::Update()
{
	if (!m_Renderer)
	{
		m_Renderer = GetEntity()->GetComponent<hg::PrimitiveRenderer>();

		if (!m_Renderer)
			m_Renderer = GetEntity()->GetComponent<hg::MeshRenderer>();
	}

	if (m_Renderer)
	{
		if (m_PlayingFlashEffect)
		{
			float t = (hg::g_Time.GameElapsed() - (m_FlashEffectEndTime - 0.2f)) / 0.2f;
			if (t < 1.0f)
			{
				// sinf(t * 10.0f) * 0.5f + 0.5f
				Color c = Color::Lerp(Color(1.0f, 0.0f, 0.0f), m_MeshColor, t * XM_PIDIV2);
				Color d = Color::Lerp(Color(1.0f, 0.0f, 0.0f), Color(0, 0.7f, 1), GetHealthRatio());
				c += d;
				m_Renderer->SetColor(c);
			}
			else
			{
				m_Renderer->SetColor(m_MeshColor);
				m_PlayingFlashEffect = false;
			}
		}
	}
}

bool Health::RestoreHealth(float value)
{
	if (m_Value >= m_maxValue)return false;
	m_Value = min(m_Value + value, m_maxValue);
	return true;
}

void Health::OnMessage(hg::Message* msg)
{
	switch (msg->GetType())
	{
	case GameMessageType::kDamage:
		if (((DamageMessage*)msg)->GetDamageType() == kDmgType_BossHeadButt)
			DealDamage( 8.5f );
		else
			DealDamage( 2.5f );
		break;
	default:
		break;
	}
}

void Health::DealDamage( float dmgVal )
{
	if (m_Invulnerable)
	{
		return;
	}

	m_Value -= dmgVal;

	if (m_Value > 0.0f || m_InDirect)
	{
		if (!m_Renderer)
		{
			m_Renderer = GetEntity()->GetComponent<hg::PrimitiveRenderer>();
		}

		if (m_Renderer)
		{
			if (!m_PlayingFlashEffect)
				m_MeshColor = m_Renderer->GetColor();

			m_FlashEffectEndTime = hg::g_Time.GameElapsed() + 0.2f;
			m_PlayingFlashEffect = true;
		}
	}
	else
	{
		DeathMessage msg;
		GetEntity()->SendMsg( &msg );
	}	
}

hg::IComponent* Health::MakeCopyDerived() const
{
	Health* copy = (Health*)IComponent::Create(SID(Health));
	copy->m_maxValue = m_maxValue;
	copy->m_Value = m_maxValue;
	copy->m_Renderer = nullptr;
	return copy;
}

float Health::GetHealthRatio() const
{
	return G_CLAMP01( m_Value / m_maxValue );
}

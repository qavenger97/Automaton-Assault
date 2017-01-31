#include "GamePch.h"
#include "ThirdPersonCameraComponent.h"
#include "GameMessage.h"
uint32_t ThirdPersonCameraComponent::s_TypeID = hg::ComponentFactory::GetGameComponentID();

ThirdPersonCameraComponent::ThirdPersonCameraComponent()
	:
	m_attached(nullptr)
{
	memset(&m_curOffsetDistance, 0, sizeof XMFLOAT3 * 2);
}

void ThirdPersonCameraComponent::LoadFromXML(tinyxml2::XMLElement* data)
{
	float x;
	float y;
	float z;
	float speed;
	data->QueryFloatAttribute("x", &x);
	data->QueryFloatAttribute("y", &y);
	data->QueryFloatAttribute("z", &z);
	data->QueryFloatAttribute("speed", &speed);
	SetOffset(x, y, z);
	SetSpeed(speed);
}

void ThirdPersonCameraComponent::Init()
{
	m_attached = hg::Entity::FindByName( SID( Player ) );
	static bool bCamDistMenu = false;
	if (!bCamDistMenu)
	{
		hg::g_DevMenu.AddMenuVar( "Camera Distance: ", &m_distance, 0.0f, 100.0f, 1.0f );
		bCamDistMenu = true;
	}
}

void ThirdPersonCameraComponent::Update()
{
	if (!IsEnabled())return;
	if (m_attached)
	{
		XMVECTOR dir = XMLoadFloat3(&m_desireOffsetDistance);
		XMVECTOR offsetDir = XMLoadFloat3(&m_curOffsetDistance);
		offsetDir = XMVectorLerp(offsetDir, dir, hg::g_Time.Delta() * m_speed);
		XMStoreFloat3(&m_curOffsetDistance, offsetDir);

		m_distance = G_CLAMP(0, 100, Math::Lerp(m_distance, m_maxDistance, hg::g_Time.FixedDelta() * 0.1f));

		XMVECTOR offset = XMLoadFloat3(&m_direction) * m_distance + offsetDir;
		XMVECTOR finalPos = offset + m_attached->GetTransform()->GetWorldPosition();
		GetEntity()->GetTransform()->SetPosition(finalPos);

		if (hg::g_Input.GetBufferedKeyState('P') == hg::BufferedKeyState::kKeyStateDown)
		{
			SetDistance(m_distance + 0.3f);
		}
		if (hg::g_Input.GetBufferedKeyState('O') == hg::BufferedKeyState::kKeyStateDown)
		{
			SetDistance(m_distance - 0.3f);
		}
	}
	else
	{
		m_attached = hg::Entity::FindByName( SID( MainMenu ) )->GetTransform()->FindChild(SID(PlayerPos))->GetEntity();
	}
}

void ThirdPersonCameraComponent::AttachTo(hg::Entity * target)
{
	m_attached = target;
}

hg::IComponent * ThirdPersonCameraComponent::MakeCopyDerived() const
{
	ThirdPersonCameraComponent* cpy = (ThirdPersonCameraComponent*)IComponent::Create(SID(ThirdPersonCameraComponent));
	*cpy = *this;
	return cpy;
	//return hg::IComponent::Create("ThirdPersonCameraComponent");
}

void ThirdPersonCameraComponent::SetOffset(const float x, const float y, const float z)
{
	XMVECTOR offset = XMVectorSet(x, y, z, 0);
	XMVECTOR distance = XMVector3Length(offset);
	offset /= distance;
	XMStoreFloat3(&m_direction, offset);
	XMStoreFloat(&m_maxDistance, distance);
	m_distance = m_maxDistance;
}

void ThirdPersonCameraComponent::SetDistance(const float distance)
{
	m_maxDistance = distance;
}

void ThirdPersonCameraComponent::SetSpeed(const float speed)
{
	m_speed = speed;
}

void ThirdPersonCameraComponent::SetDesireOffsetDistance(FXMVECTOR& offset)
{
	XMStoreFloat3(&m_desireOffsetDistance, offset);
}

void ThirdPersonCameraComponent::OnMessage(hg::Message * msg)
{
	PowerUpMessage* pMsg = (PowerUpMessage*)msg;
	if (pMsg->GetPowerUpType() == PowerUpType::kPowerUp_Boss)
	{
		m_maxDistance += pMsg->GetAmount();
	}
}

void ThirdPersonCameraComponent::RecenterCamera()
{
	memset(&m_curOffsetDistance, 0, sizeof XMFLOAT3);
}

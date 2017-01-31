#include "GamePch.h"

#include "RadialGravity.h"
#include "PlayerComp.h"
#include "Projectile.h"

uint32_t RadialGravity::s_TypeID = hg::ComponentFactory::GetGameComponentID();

void RadialGravity::LoadFromXML( tinyxml2::XMLElement* data )
{
	m_DropRadiusSq = 10.2f;
	m_OuterRingRadiusSq = 13.0f;

	data->QueryFloatAttribute( "drop_radius", &m_DropRadiusSq );
	data->QueryFloatAttribute( "outer_ring_radius", &m_OuterRingRadiusSq );

	// Square each radius
	m_DropRadiusSq *= m_DropRadiusSq;
	m_OuterRingRadiusSq *= m_OuterRingRadiusSq;

	m_BridgeLeftBounds = 34.0f;
	m_BridgeRightBounds = 38.0f;

	m_BridgeBottomBounds = -11.0f;
	m_BridgeTopBounds = -7.0f;

	m_GroundY = 0.5f;

	m_BridgeExitMode = false;
}

void RadialGravity::Start()
{
	m_Player = Hourglass::Entity::FindByTag(SID(Player))->GetTransform();
	m_PlayerDC = m_Player->GetEntity()->GetComponent<hg::DynamicCollider>();
	IComponent::SetEnabled( false );
	m_DoomTime = 3.0f;
	m_HitBottomTime = 2.0f;
	m_FakePlayerVelocity = XMVectorSet( 0.0f, 0.0f, 0.0f, 1.0f );
	m_IsDoomFalling = false;
	m_BridgeIsRetracted = false;
}

void RadialGravity::DoomFall()
{
	float dt = hg::g_Time.Delta();
	m_DoomTimer += dt;

	if (m_DoomTimer < m_HitBottomTime)
	{
		XMVECTOR playerPos = m_Player->GetWorldPosition();
		Math::Gravity( playerPos, m_FakePlayerVelocity, dt );

		XMFLOAT3 fPlayerPos;
		XMStoreFloat3( &fPlayerPos, playerPos );

		m_Player->SetPosition( playerPos );
	}
	else if (m_DoomTimer >= m_DoomTime)
	{
		DeathMessage dieDieDie;
		m_Player->GetEntity()->SendMsg( &dieDieDie );
		m_IsDoomFalling = false;
		IComponent::SetEnabled( false );
	}
	
}

void RadialGravity::Update()
{
	if (m_IsDoomFalling)
	{
		DoomFall();
		return;
	}

	Vector3 playerPos2D;
	XMVECTOR playerPos3D;
	playerPos3D = playerPos2D = m_Player->GetWorldPosition();

	playerPos2D.y = 0.0f;

	Vector3 center = GetEntity()->GetTransform()->GetWorldPosition();
	center.y = 0.0f;

	float dt = hg::g_Time.Delta();
	
	if (m_BridgeExitMode)
	{
		float distSq = Vector3::DistanceSquared( playerPos2D, center );
		if (distSq < m_OuterRingRadiusSq && distSq >= m_DropRadiusSq)
		{
			if (playerPos2D.x > center.x && playerPos2D.z < m_BridgeTopBounds && playerPos2D.z > m_BridgeBottomBounds)
			{
				return;
			}

			m_IsDoomFalling = true;
			m_DoomTimer = 0.0f;

			// Prevent the player from doing anything other than fall to his doom
			m_Player->GetEntity()->GetComponent<PlayerComp>()->SetEnabled( false );

			DoomFall();
		}
		return;
	}

	// not in exit mode
	float distSq = Vector3::DistanceSquared( playerPos2D, center );
	if (distSq < m_OuterRingRadiusSq && distSq >= m_DropRadiusSq)
	{
		if (!m_BridgeIsRetracted && playerPos2D.z > center.z && playerPos2D.x > m_BridgeLeftBounds && playerPos2D.x < m_BridgeRightBounds)
		{
			return;
		}

		m_IsDoomFalling = true;
		m_DoomTimer = 0.0f;

		// Prevent the player from doing anything other than fall to his doom
		m_Player->GetEntity()->GetComponent<PlayerComp>()->SetEnabled( false );

		DoomFall();
	}
}

void RadialGravity::OnEvent(const StrID& eventName)
{
	if (eventName == SID(ActivateRadialGravity))
	{
		IComponent::SetEnabled(true);
	}
	else if (eventName == SID(DeactivateRadialGravity))
	{
		IComponent::SetEnabled(false);
	}
	else if (eventName == SID(BridgeRetracted))
	{
		m_BridgeIsRetracted = true;
	}
	else if (eventName == SID(BridgeExtended))
	{
		m_BridgeIsRetracted = false;
	}
}

hg::IComponent* RadialGravity::MakeCopyDerived() const
{
	RadialGravity* copy = (RadialGravity*)IComponent::Create( SID(RadialGravity) );

	copy->m_BridgeExitMode = false;
	// TODO: handle copying of data members (ex: copy->m_data = m_data)

	return copy;
}

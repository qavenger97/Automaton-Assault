
#include "GamePch.h"

#include "GunLogic.h"
#include "Projectile.h"
#include "Component/Animation.h"
#include "Renderer/DebugRenderer.h"

static const float kMuzzleLightFadeOutTime = 0.1f;

uint32_t GunLogic::s_TypeID = hg::ComponentFactory::GetGameComponentID();
void GunLogic::ConfigureMachineGun(GunLogic * gun)
{
	if (gun)
	{
		gun->SetType(GunType::AUTO);
		gun->SetMaxAmmo(560);
		gun->SetDamage(5);
		gun->SetMaxAmmoPerMag(30);
		gun->SetReloadTime(2);
		gun->SetRpm(560);
		gun->SetSpeardDegree(3, 0.15f);
		gun->Init();
		gun->SetAssembledBulletName(SID(Projectile));
		gun->m_spawnPos = gun->GetEntity()->GetTransform()->FindChild(SID(SpawnPos));
		gun->SetGunFireAudioEventId(AK::EVENTS::PLAY_MACHINEGUN_FIRE);
		gun->m_MuzzleLightRadius = 3.0f;
		gun->m_holder = hg::Entity::FindByName(SID(Player));
	}
}
void GunLogic::ConfigureShotGun(GunLogic * gun)
{
	if (gun)
	{
		gun->SetType(GunType::SINGLE);
		gun->SetMaxAmmo(110);
		gun->SetDamage(5);
		gun->SetMaxAmmoPerMag(15);
		gun->SetReloadTime(2);
		gun->SetRpm(60);
		gun->SetSpeardDegree(16, 0.15f);
		gun->SetBulletPerShot(7);
		gun->Init();
		gun->SetAssembledBulletName(SID(Projectile));
		gun->m_spawnPos = gun->GetEntity()->GetTransform()->FindChild(SID(SpawnPos));
		gun->SetGunFireAudioEventId(AK::EVENTS::PLAY_SHOTGUN_FIRE);
		gun->m_MuzzleLightRadius = 3.0f;
		gun->m_holder = hg::Entity::FindByName(SID(Player));
	}
}

void GunLogic::ConfigureTurretGun(GunLogic* gun)
{
	if (gun)
	{
		gun->SetType(GunType::AUTO);
		gun->SetMaxAmmo(560);
		gun->SetDamage(5);
		gun->SetMaxAmmoPerMag(30);
		gun->SetReloadTime(2);
		gun->SetRpm(560);
		gun->SetSpeardDegree(2, 0.15f);
		gun->Init();
		gun->SetAssembledBulletName(SID(TurretBullet));
		gun->m_spawnPos = gun->GetEntity()->GetTransform()->FindChild(SID(SpawnPos));
		gun->SetGunFireAudioEventId(AK::EVENTS::PLAY_TURRET_GUNFIRE);
		gun->m_MuzzleLightRadius = 5.0f;
		gun->m_holder = nullptr;
	}
}

GunLogic::GunLogic()
{
	m_type = GunType::NUM_OF_TYPE;
	m_state = State::IDLE;
}

void GunLogic::Init()
{
	m_playerAnim = nullptr;
	m_curAmmoInMag = m_maxAmmoPerMag;
	m_curAmmoTotal = m_maxAmmo;

	m_MuzzleLightTimer = 0.0f;
}

void GunLogic::SetMaxAmmo(int maxAmmo)
{
	m_maxAmmo = maxAmmo;
}
void GunLogic::SetMaxAmmoPerMag(int maxAmmoPerMag)
{
	m_maxAmmoPerMag = maxAmmoPerMag;
	m_inv_MaxAmmoPerMag = 1.0f / (maxAmmoPerMag + 1);
}
void GunLogic::SetReloadTime(float reloadTime)
{
	m_reloadTime = reloadTime;
	m_inv_reloadTime = 1.0f / reloadTime;
}
void GunLogic::SetRpm(float rpm)
{
	m_RPM = rpm;
	m_fireRate = 60.0f / rpm;
}
void GunLogic::SetSpeard(float maxSpeardRad, float fullySpeardRatio)
{
	m_Speard = maxSpeardRad;
	m_fullySpeardRatio = fullySpeardRatio;
	m_speardStep = 1 / (m_maxAmmoPerMag * fullySpeardRatio);
}

void GunLogic::SetSpeardDegree(float maxSpeardAngle, float fullySpeardRatio)
{
	m_Speard = maxSpeardAngle * G_DEG2RAD;
	m_fullySpeardRatio = fullySpeardRatio;
	m_speardStep = 1 / (m_maxAmmoPerMag * fullySpeardRatio);
}

void GunLogic::SetAssembledBulletName(StrID bullet)
{
	m_BulletName = bullet;
}

void GunLogic::SetDamage(float damage)
{
	m_damage = damage;
}

void GunLogic::SetType(GunType type)
{
	m_type = type;
}

void GunLogic::SetSpawnPoint(hg::Transform * position)
{
	m_spawnPos = position;

	m_MuzzleLight = m_spawnPos->GetEntity()->GetComponent<hg::Light>();
	if (m_MuzzleLight)
	{
		//Color color = m_MuzzleLight->GetColor();
		//color.w = 0.0f;
		//m_MuzzleLight->SetColor(color);

		//m_MuzzleLightRadius = m_MuzzleLight->GetRadius();
		m_MuzzleLight->SetRadius(0.0f);
	}
}

void GunLogic::SetGunFireAudioEventId(AudioEvent eventId)
{
	m_GunFireAudioEventId = eventId;
}

void GunLogic::Update()
{
	if (IsEnabled())
	{
		//hg::DebugRenderer::DrawAxis(*m_spawnPos);
		//hg::DebugRenderer::DrawAxis(*GetEntity()->GetTransform());

		float dt = hg::g_Time.Delta();
		m_fireTimer -= dt;

		switch (m_state)
		{
		case GunLogic::FIREING:
			Firing();
			break;
		case GunLogic::CHARGING:
			break;
		case GunLogic::RELOADING:
			Reloading();
			break;
		case GunLogic::RELOADED:
			SwitchState(State::IDLE);
			break;
		default:
			m_speardRange = max(m_speardRange - m_speardStep*dt*3, 0);
			break;
		}

		if (m_MuzzleLight)
		{
			if (m_MuzzleLightTimer > 0.0f)
			{
				m_MuzzleLightTimer -= hg::g_Time.Delta();
				
				//Color color = m_MuzzleLight->GetColor();
				//color.w = m_MuzzleLightTimer > 0 ? m_MuzzleLightTimer / kMuzzleLightFadeOutTime : 0.0f;
				//m_MuzzleLight->SetColor(color);

				float r = m_MuzzleLightTimer > 0 ? m_MuzzleLightTimer / kMuzzleLightFadeOutTime : 0.0f;
				if (r > 0.6f)
					r = 1.0f - (r - 0.6f) / 0.4f;
				else
					r = r / 0.6f;
				m_MuzzleLight->SetRadius(m_MuzzleLightRadius * r);
			}
		}
	}
}

void GunLogic::FireBullet()
{
	//XMVECTOR dir = GetEntity()->GetTransform()->GetWorldRotation();
	XMVECTOR dir = m_spawnPos->GetWorldRotation();

	float ver = m_Speard;
	float speardX = Math::RNG::Range(-m_Speard, m_Speard);
	float speardY = Math::RNG::Range(-m_Speard, m_Speard);
	dir = XMQuaternionMultiply(dir, XMQuaternionRotationRollPitchYaw(speardX * m_speardRange, speardY * m_speardRange, 0));
	hg::Entity* entity = hg::Entity::Assemble(m_BulletName, m_spawnPos->GetWorldPosition(), dir);

	Projectile* bullet = entity->GetComponent<Projectile>();
	bullet->AddDamage(m_damage);
	bullet->SetBounce(m_bounce);
	m_speardRange = G_CLAMP01(m_speardRange + m_speardStep);

	m_MuzzleLightTimer = kMuzzleLightFadeOutTime;
}

void GunLogic::FireBullets()
{
	for (int i = 0; i < m_bulletsPerShot; i++)
	{
		FireBullet();
	}
}



void GunLogic::Reloading()
{
	if (m_curAmmoInMag > m_maxAmmoPerMag || m_curAmmoTotal <= 0)
	{
		SwitchState(State::IDLE);
		return;
	}
	m_timer += hg::g_Time.Delta();
	if (m_timer >= m_reloadTime)
	{
		if (m_curAmmoInMag == 0)
		{
			int reloadAmt = min(m_curAmmoTotal, m_maxAmmoPerMag);
			m_curAmmoInMag += reloadAmt;
			m_curAmmoTotal -= reloadAmt;
		}
		else
		{
			int reloadAmt = min(m_maxAmmoPerMag - m_curAmmoInMag + 1, m_curAmmoTotal);
			m_curAmmoInMag += reloadAmt;
			m_curAmmoTotal -= reloadAmt;
		}
		SwitchState(State::RELOADED);
	}
}

void GunLogic::Fire()
{
	if (m_state != State::RELOADING)
		SwitchState(State::FIREING);
}
void GunLogic::Firing()
{
	if (m_curAmmoInMag > 0)
	{
		if (m_fireTimer <= 0)
		{
			EmitGunFireSound();

			switch (m_type)
			{
			case GunLogic::AUTO:
				FireBullet();
				break;
			case GunLogic::SEMI:
				break;
			case GunLogic::SINGLE:
				FireBullets();
				break;
			default:
				break;
			}

			if (m_bounce)
			{	
				if (m_holder)
				{
					FireMessage msg;
					m_holder->SendMsg(&msg);
				}
			}
			

			m_curAmmoInMag--;
			m_fireTimer = m_fireRate;
		}
	}
	else
	{
		if (m_state != State::RELOADING)
		{
			SwitchState(State::RELOADING);
			Reload();
		}
	}
}

void GunLogic::SetBounce(bool enable)
{
	m_bounce = enable;
}

void GunLogic::Cease()
{
	if (m_state != State::RELOADING)
		SwitchState(State::IDLE);
}
void GunLogic::Reload()
{
	SwitchState(State::RELOADING);
}
bool GunLogic::IsReloading()
{
	return m_state == State::RELOADING;
}
void GunLogic::OnGunSwitching()
{
	SwitchState(State::IDLE);
}
void GunLogic::SwitchState(State nextState)
{
	// Play weapon reloading sound clip
	if (nextState == State::RELOADING && m_state != State::RELOADING &&
		m_curAmmoTotal != 0 &&
		m_curAmmoInMag != m_maxAmmoPerMag)
	{
		EmitReloadSound();
	}

	m_state = nextState;
	m_timer = 0;

}

void GunLogic::SetBulletPerShot(int amount)
{
	m_bulletsPerShot = amount;
}
bool GunLogic::AddAmmo(int amount)
{
	if (m_curAmmoTotal >= m_maxAmmo) return false;
	m_curAmmoTotal = min(m_curAmmoTotal + amount, m_maxAmmo);
	return true;
}
float GunLogic::GetAmmoRatio() const
{
	return G_CLAMP01((float)m_curAmmoInMag / m_maxAmmoPerMag);
}
float GunLogic::GetTotalAmmoRatio() const
{
	return G_CLAMP01((float)m_curAmmoTotal / m_maxAmmo);
}
hg::IComponent * GunLogic::MakeCopyDerived() const
{
	GunLogic* cpy = (GunLogic*)hg::IComponent::Create(SID(GunLogic));
	*cpy = *this;
	return cpy;
}

void GunLogic::EmitGunFireSound() const
{
	if (GetEntity()->GetParent())
	{
		hg::Entity* player = GetEntity()->GetParent()->GetParent();
		hg::g_AudioSystem.PostEvent(m_GunFireAudioEventId, player);
	}
	else
	{
		hg::g_AudioSystem.PostEvent(m_GunFireAudioEventId);
	}
}

void GunLogic::EmitReloadSound() const
{
	if (GetEntity()->GetParent())
	{
		hg::Entity* player = GetEntity()->GetParent()->GetParent();
		hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_MACHINEGUN_RELOAD, player);
	}
	else
	{
		hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_MACHINEGUN_RELOAD);
	}
}

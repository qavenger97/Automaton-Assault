#include "GamePch.h"

#include "Turret.h"
#include "GunLogic.h"
#include "Projectile.h"

IMPLEMENT_GAME_COMPONENT_TYPEID(Turret)

hg::IComponent* Turret::MakeCopyDerived() const
{
	Turret* cpy = (Turret*)IComponent::Create(SID(Turret));

	return cpy;
}

void Turret::Start()
{
	// Assemble gun for turret
	m_Gun = GetGunTransform();
	GunLogic* gunLogic = m_Gun->GetEntity()->GetComponent<GunLogic>();
	gunLogic->Cease();
	GunLogic::ConfigureTurretGun(gunLogic);
	gunLogic->SetSpawnPoint(m_Gun->FindChild(SID(GunPos)));

	m_Player = hg::Entity::FindByTag(SID(Player));
	m_LightMaterials[kTurretLight_Red]		= hg::g_MaterialManager.GetMaterial("Assets/Material/Light_Red.xml");
	m_LightMaterials[kTurretLight_Yellow]	= hg::g_MaterialManager.GetMaterial("Assets/Material/Light_Yellow.xml");
	m_LightMaterials[kTurretLight_Off]		= hg::g_MaterialManager.GetMaterial("Assets/Material/Light_Off.xml");

	hg::Transform* lightTransform = m_Gun->FindChild(SID(Turret_Light));
	if (lightTransform)
	{
		hg::Entity* lightEnt = lightTransform->GetEntity();
		m_LightMesh = lightEnt->GetComponent<hg::MeshRenderer>();
	}
}

void Turret::Update()
{
	if (!m_Player)
		return;

	GunLogic* gunLogic = m_Gun->GetEntity()->GetComponent<GunLogic>();

	if (CanSeePlayer(m_Player))
	{
		// Rotating gun on a turret
		float dt = hg::g_Time.Delta();
		float y = XMVectorGetY(m_Gun->GetWorldPosition());

		// Make planar vector for gun direction
		XMVECTOR targetPos = m_Player->GetPosition();
		XMVectorSetY(targetPos, XMVectorGetY(m_Gun->GetWorldPosition()));

		XMVECTOR gunDir = XMVector3Normalize(targetPos - m_Gun->GetWorldPosition());
		XMVECTOR angle = XMVector3AngleBetweenNormals(gunDir, m_Gun->Forward());

		XMVECTOR dot = XMVector3Dot(gunDir, m_Gun->Right());

		if (XMVectorGetX(dot) < 0)
		{
			dt *= -1;
		}

		if (XMVectorGetX(angle) > dt)
		{
			XMVECTOR quat = XMQuaternionRotationAxis(XMVectorSet(0, 1, 0, 0), dt);
			m_Gun->Rotate(quat);
		}
		else if (XMVectorGetX(angle) != 0.0f)
		{
			m_Gun->LookAt(targetPos);
		}

		// Start firing
		if (gunLogic->GetGunState() != GunLogic::FIREING)
		{
			gunLogic->Fire();
		}

		if (m_LightMesh)
		{
			m_LightMesh->SetMaterial(m_LightMaterials[kTurretLight_Red]);
		}
	}
	else
	{
		// Lost player, fire until reload
		if (gunLogic->GetGunState() != GunLogic::FIREING)
		{
			gunLogic->Cease();

			// Flash turret light
			if (m_LightMesh)
			{
				if ((int)(hg::g_Time.GameElapsed() * 2) % 2 == 0)
				{
					m_LightMesh->SetMaterial(m_LightMaterials[kTurretLight_Yellow]);
				}
				else
				{
					m_LightMesh->SetMaterial(m_LightMaterials[kTurretLight_Off]);
				}
			}
		}
	}
}

void Turret::OnMessage(hg::Message* msg)
{
	if (msg->GetType() == GameMessageType::kDeath)
	{
		// Play explosion effect
		Vector3 pos = GetEntity()->GetPosition() + Vector3(0.0f, 1.0f, 0.0f);
		hg::Entity::Assemble(SID(FX_Explosion), pos, Quaternion::Identity);

		hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_TURRET_EXPLODE, GetEntity()->GetPosition());

		GetEntity()->Destroy();
	}
}

bool Turret::CanSeePlayer(hg::Entity* player) const
{
	Vector3 start = GetEntity()->GetPosition() + Vector3(0.0f, 1.0f, 0.0f);
	hg::Ray ray(start, player->GetPosition());
	hg::Entity* hitEnt;
	if (hg::g_Physics.RayCast(ray, &hitEnt, nullptr, nullptr, kTurretVisionMask))
	{
		if (hitEnt->GetTag() == SID(Player))
		{
			return true;
		}
	}

	return false;
}

hg::Transform* Turret::GetGunTransform() const
{
	std::vector<GunLogic*> gunScripts = GetEntity()->GetComponentsInChildren<GunLogic>();

	if (!gunScripts.empty())
		return gunScripts[0]->GetEntity()->GetTransform();

	return nullptr;
}

#include "GamePch.h"

#include "TurretSkinnedMesh.h"
#include "GunLogic.h"
#include "Projectile.h"

IMPLEMENT_GAME_COMPONENT_TYPEID(TurretSkinnedMesh)

enum TurretJoint
{
	kTurretJoint_Root,
	kTurretJoint_Yaw,
	kTurretJoint_Pitch,
	kTurretJoint_Muzzle,
	kTurretJoint_Laser
};

TurretSkinnedMesh::TurretSkinnedMesh()
	: m_bInitTransform(false)
{

}

hg::IComponent* TurretSkinnedMesh::MakeCopyDerived() const
{
	TurretSkinnedMesh* cpy = (TurretSkinnedMesh*)IComponent::Create(SID(TurretSkinnedMesh));

	cpy->m_bInitTransform = false;
	cpy->m_Yaw = 0.0f;

	return cpy;
}

void TurretSkinnedMesh::Start()
{
	if (!m_bInitTransform)
	{
		GetEntity()->GetTransform()->Rotate(XM_PIDIV2, 0, 0);
		m_bInitTransform = true;
	}

	// Assemble gun for turret
	m_Animation = GetEntity()->GetComponent<hg::Animation>();
	assert(m_Animation);

	m_Gun = GetEntity()->GetTransform();
	GunLogic* gunLogic = m_Gun->GetEntity()->GetComponent<GunLogic>();
	assert(gunLogic);

	GunLogic::ConfigureTurretGun(gunLogic);
	gunLogic->SetSpawnPoint(&m_Muzzle);

	m_Player = hg::Entity::FindByTag(SID(Player));
	m_Muzzle = *GetEntity()->GetTransform();
}

void TurretSkinnedMesh::Update()
{
	if (!m_Player)
		return;

	//static float t = 0.0f;
	//m_Animation->GetBoneTransform(kTurretJoint_Yaw).SetRotation(0, 0, t);
	//t += hg::g_Time.Delta();

	GunLogic* gunLogic = m_Gun->GetEntity()->GetComponent<GunLogic>();

	if (CanSeePlayer(m_Player))
	{
		// Rotating gun on a turret
		float dt = hg::g_Time.Delta();
		XMVECTOR gunPos = m_Gun->GetWorldPosition();
		float y = XMVectorGetY(m_Gun->GetWorldPosition());

		// Make planar vector for gun direction
		XMVECTOR targetPos = m_Player->GetPosition();
		XMVectorSetY(targetPos, XMVectorGetY(m_Gun->GetWorldPosition()));

		if (Vector3(m_GunForward).LengthSquared() <= FLT_EPSILON)
		{
			m_Yaw = 0.0f;
			m_GunForward = Matrix::CreateFromYawPitchRoll(m_Yaw, 0, 0).Forward();
			m_GunRight = Matrix::CreateFromYawPitchRoll(m_Yaw, 0, 0).Right();
		}

		//XMVECTOR forward = m_Animation->GetBoneTransform(kTurretJoint_Yaw).Forward();
		XMVECTOR right = m_Animation->GetBoneTransform(kTurretJoint_Yaw).Right();

		hg::DebugRenderer::DrawLine(gunPos, gunPos + m_GunForward * 1.0f, Color(1, 0, 0), Color(1, 0, 0));
		hg::DebugRenderer::DrawLine(gunPos, gunPos + m_GunRight * 1.0f, Color(0, 1, 0), Color(0, 1, 0));

		XMVECTOR gunDir = XMVector3Normalize(targetPos - m_Gun->GetWorldPosition());
		XMVECTOR angle = XMVector3AngleBetweenNormals(gunDir, m_GunForward);

		XMVECTOR dot = XMVector3Dot(gunDir, m_GunRight);

		if (XMVectorGetX(dot) < 0)
		{
			dt *= -1;
		}

		//if (XMVectorGetX(angle) > dt)
		{
			m_Yaw += dt;
			m_Animation->GetBoneTransform(kTurretJoint_Yaw).SetRotation(0, 0, -m_Yaw);
			m_GunForward = Matrix::CreateFromYawPitchRoll(m_Yaw, 0, 0).Forward();
			m_GunRight = Matrix::CreateFromYawPitchRoll(m_Yaw, 0, 0).Right();

			if (m_Yaw >= XM_2PI) m_Yaw -= XM_2PI;
			if (m_Yaw < 0) m_Yaw += XM_2PI;
		}
		//else if (XMVectorGetX(angle) != 0.0f)
		//{
		//	m_Animation->GetBoneTransform(kTurretJoint_Yaw).LookAt(targetPos);
		//}

		char buf[1024];
		sprintf_s(buf, "Angle: %f Yaw: %f\n", XMVectorGetX(angle), m_Yaw);
		OutputDebugStringA(buf);

		// Start firing
		if (gunLogic->GetGunState() != GunLogic::FIREING)
		{
			gunLogic->Fire();
		}
	}
	else
	{
		// Lost player, fire until reload
		if (gunLogic->GetGunState() != GunLogic::FIREING)
		{
			gunLogic->Cease();
		}
	}
}

void TurretSkinnedMesh::OnMessage(hg::Message* msg)
{
	if (msg->GetType() == GameMessageType::kDeath)
	{
		// Play explosion effect
		Vector3 pos = GetEntity()->GetPosition() + Vector3(0.0f, 1.0f, 0.0f);
		hg::Entity::Assemble(SID(FX_Explosion), pos, Quaternion::Identity);

		GetEntity()->Destroy();
	}
}

hg::Transform* TurretSkinnedMesh::GetMuzzle() const
{
	return &m_Animation->GetBoneTransform(kTurretJoint_Muzzle);
}

bool TurretSkinnedMesh::CanSeePlayer(hg::Entity* player) const
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

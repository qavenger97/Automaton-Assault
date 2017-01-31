#include "GamePch.h"

#include "Crate.h"
#include "Projectile.h"

IMPLEMENT_GAME_COMPONENT_TYPEID(Crate)

void Crate::LoadFromXML(tinyxml2::XMLElement* data)
{
	m_SpawnEntity = WSID("");
	const char* spawnedEnt = data->Attribute("spawn");

	if (spawnedEnt)
		m_SpawnEntity = WSID(spawnedEnt);
}

void Crate::Update()
{
	hg::DynamicCollider* dynCol = GetEntity()->GetComponent<hg::DynamicCollider>();
	float dist = m_VelocityY * hg::g_Time.Delta();

	float offsetY = abs(dynCol->GetLocalAabbShape().pMin.y);
	Vector3 start = GetEntity()->GetPosition();
	start.y -= offsetY + 0.01f;
	Vector3 end = start + Vector3(0, -1, 0) * dist;

	hg::Ray ray(start, end);
	Vector3 hitPos;

	if (hg::g_Physics.RayCast(ray, nullptr, &hitPos))
	{
		GetEntity()->GetTransform()->SetPosition(hitPos + Vector3(0, offsetY + 0.01f, 0));
		m_VelocityY = 0.0f;
	}
	else
	{
		GetEntity()->GetTransform()->Translate(Vector3(0, -1, 0) * dist);
		m_VelocityY += 10.0f * hg::g_Time.Delta();
	}
}

hg::IComponent* Crate::MakeCopyDerived() const
{
	Crate* cpy = (Crate*)IComponent::Create(SID(Crate));

	cpy->m_SpawnEntity = m_SpawnEntity;

	return cpy;
}

void Crate::OnMessage(hg::Message* msg)
{
	switch (msg->GetType())
	{
	case GameMessageType::kDeath:
		{
			SpawnPresetEntity();

			// Play crate break sound
			//GetEntity()->GetComponent<hg::AudioSource>()->PostAudioEvent(AK::EVENTS::PLAY_CRATE_BREAK);
			hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_CRATE_BREAK, GetEntity()->GetPosition());

			// Destroy crate
			GetEntity()->Destroy();
		}
		break;
	}
}

void Crate::SpawnPresetEntity()
{
	if (m_SpawnEntity == WSID(""))
	{
		static StrID items[] = { {SID(MedPack)}, {SID(AmmoPack)}, {SID(Pickup_Ricochet)}, {SID(Pickup_Shield)} };
		float chance = Math::RNG::Range(0.0f, 1.0f);
		if (chance <= 0.2f)
		{
			int rnd = Math::RNG::Range(0, 99);
			XMVECTOR pos = GetEntity()->GetTransform()->GetWorldPosition();
			int i = ((rnd <= 49)<<1) + (rnd & 1);
			hg::Entity::Assemble(items[i], XMVectorSetY(pos, 1), GetEntity()->GetTransform()->GetWorldRotation());
		}
		return;
	}

	hg::Entity* ent = hg::Entity::Assemble((m_SpawnEntity), GetEntity()->GetTransform()->GetWorldPosition(), GetEntity()->GetTransform()->GetWorldRotation());

	// Set spawned entity to use spawner's transform
	hg::Transform& assembledTrans = *ent->GetTransform();
	hg::Transform& spawnerTrans = *GetEntity()->GetTransform();

	Vector3 scale = assembledTrans.GetWorldScale();
	assembledTrans = spawnerTrans;
	assembledTrans.SetScale(scale);

	// Drop entity onto ground
	hg::DynamicCollider* dynCol = ent->GetComponent<hg::DynamicCollider>();
	if (dynCol)
	{
		ent->GetTransform()->SetPosition(dynCol->GetGroundedPosition());
	}
}

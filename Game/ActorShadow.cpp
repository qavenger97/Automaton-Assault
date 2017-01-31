#include "GamePch.h"

#include "ActorShadow.h"
#include "Common.h"

IMPLEMENT_GAME_COMPONENT_TYPEID(ActorShadow)

hg::IComponent* ActorShadow::MakeCopyDerived() const
{
	ActorShadow* copy = (ActorShadow*)IComponent::Create(SID(ActorShadow));

	return copy;
}

void ActorShadow::Start()
{
	m_Player = nullptr;
	m_ShadowMesh = nullptr;
}

void ActorShadow::Update()
{
	if (!m_Player)
	{
		m_Player = hg::Entity::FindByTag(SID(Player))->GetTransform();

		if (!m_Player)
			return;
	}

	if (!m_ShadowMesh)
	{
		m_ShadowMesh = GetEntity()->GetComponent<hg::MeshRenderer>();
	}

	Vector3 start = m_Player->GetWorldPosition();
	hg::Ray ray(start, -G_UP, 5.0f);
	Vector3 hitPos;

	hg::Entity* hitEnt;

	if (hg::g_Physics.RayCast(ray, &hitEnt, &hitPos, nullptr, COLLISION_DEFAULT_MASK))
	{
		m_ShadowMesh->SetEnabled(true);
		hitPos.y += 0.01f;
		GetEntity()->GetTransform()->SetPosition(hitPos);

		//hg::DebugRenderer::DrawLine(start, hitPos);
		//hg::DebugRenderer::DrawLine(start, GetEntity()->GetPosition());
	}
	else
	{
		m_ShadowMesh->SetEnabled(false);
	}
}

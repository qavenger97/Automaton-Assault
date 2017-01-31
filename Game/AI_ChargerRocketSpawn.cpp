#include "GamePch.h"

#include "AI_ChargerRocketSpawn.h"
#include "Component\Separation.h"

extern bool g_bDebugAIBehaviors;

void AI_ChargerRocketSpawn::Start()
{
	m_bHasSpawned = false;
}

void AI_ChargerRocketSpawn::Init( Hourglass::Entity* entity )
{
	entity->GetComponent<hg::Separation>()->SetEnabled( false );
}

Hourglass::IBehavior::Result AI_ChargerRocketSpawn::Update(Hourglass::Entity* entity)
{
	if (g_bDebugAIBehaviors)
	{
		hg::DevTextRenderer::DrawText_WorldSpace("AI_ChargerRocketSpawn", entity->GetPosition());

		Vector3 pos = entity->GetTransform()->GetWorldPosition();
		Vector3 v1 = entity->GetTransform()->Right();
		Vector3 v2 = entity->GetTransform()->Up();
		Vector3 v3 = entity->GetTransform()->Forward();
		hg::DebugRenderer::DrawLine(pos, pos + v1, Vector4(1, 0, 0, 1), Vector4(1, 0, 0, 1));
		hg::DebugRenderer::DrawLine(pos, pos + v2, Vector4(0, 1, 0, 1), Vector4(0, 1, 0, 1));
		hg::DebugRenderer::DrawLine(pos, pos + v3, Vector4(0, 0, 1, 1), Vector4(0, 0, 1, 1));
	}

	// Play spawn animation once
	if (!m_bHasSpawned)
	{
		m_bHasSpawned = true;
		entity->GetComponent<hg::Animation>()->Play(WSID("charger_rocket_spawn"), 1.0f);
		return IBehavior::kRUNNING;
	}

	if (entity->GetComponent<hg::Animation>()->IsPlaying(WSID("charger_rocket_spawn")))
	{
		return IBehavior::kRUNNING;
	}

	return IBehavior::kSUCCESS;
}

void AI_ChargerRocketSpawn::Shutdown( Hourglass::Entity * entity )
{
	entity->GetComponent<hg::Separation>()->SetEnabled( true );
}

Hourglass::IBehavior* AI_ChargerRocketSpawn::MakeCopy() const
{
	AI_ChargerRocketSpawn* copy = (AI_ChargerRocketSpawn*)IBehavior::Create(SID(AI_ChargerRocketSpawn));
	copy->m_bHasSpawned = false;
	return copy;
}

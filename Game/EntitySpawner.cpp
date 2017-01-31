#include "GamePch.h"

#include "EntitySpawner.h"

uint32_t EntitySpawner::s_TypeID = hg::ComponentFactory::GetGameComponentID();

void EntitySpawner::LoadFromXML(tinyxml2::XMLElement* data)
{
	const char* assembledName = data->Attribute("Assembled");

	if (assembledName)
		m_AssembledName = WSID(assembledName);

	m_SpawnOnStart = false;
	data->QueryBoolAttribute("SpawnOnStart", &m_SpawnOnStart);

	tinyxml2::XMLElement* targetXML = data->FirstChildElement("Target");

	m_bUseTarget = false;

	if (targetXML)
	{
		m_bUseTarget = true;
		targetXML->QueryFloatAttribute("x", &m_TargetPosition.x);
		targetXML->QueryFloatAttribute("y", &m_TargetPosition.y);
		targetXML->QueryFloatAttribute("z", &m_TargetPosition.z);
	}

	m_Delay = 0;
	data->QueryFloatAttribute("delay", &m_Delay);
}

void EntitySpawner::Init()
{
	m_TimerStarted = false;
	m_Spawned = false;
	m_Timer = 0.0f;
}

void EntitySpawner::Spawn()
{
	hg::Entity* ent = hg::Entity::Assemble(m_AssembledName, GetEntity()->GetTransform()->GetWorldPosition(), GetEntity()->GetTransform()->GetWorldRotation());

	// Set spawned entity to use spawner's transform
	hg::Transform& assembledTrans = *ent->GetTransform();
	hg::Transform& spawnerTrans = *GetEntity()->GetTransform();

	Vector3 scale = assembledTrans.GetWorldScale();
	assembledTrans = spawnerTrans;
	assembledTrans.SetScale( scale );

	// When using target position, spawn object at target position and set direction
	// as horizontal vector from spawner's position to target position.
	if (m_bUseTarget)
	{
		Vector3 lookAtVec = m_TargetPosition - Vector3(assembledTrans.GetWorldPosition());
		lookAtVec.y = 0;
		assembledTrans.SetPosition(m_TargetPosition);
		assembledTrans.SetRotation(hg::Transform::BuildLookAtRotationFromDirectionVector(lookAtVec));

		// Hack: for rocket spawned charger bot, replace its behavior tree
		hg::BehaviorTree* bhvTree = ent->GetComponent<hg::BehaviorTree>();
		if (bhvTree)
		{
			bhvTree->SetRoot(hg::IBehavior::AssembleRoot(SID(ChargerBotRocketSpawnAI)));
		}
	}

	// Drop entity onto ground
	hg::DynamicCollider* dynCol = ent->GetComponent<hg::DynamicCollider>();
	if (dynCol)
	{
		ent->GetTransform()->SetPosition(dynCol->GetGroundedPosition());
	}
}

void EntitySpawner::Update()
{
	if (m_SpawnOnStart && !m_Spawned)
	{
		m_TimerStarted = true;
	}

	if (m_TimerStarted)
	{
		m_Timer += hg::g_Time.Delta();
		if (m_Timer > m_Delay)
		{
			if (!m_Spawned)
			{
				Spawn();				
				m_Spawned = true;
			}
		}
	}
}

hg::IComponent* EntitySpawner::MakeCopyDerived() const
{
	EntitySpawner* copy = (EntitySpawner*)IComponent::Create(SID(EntitySpawner));
	copy->m_AssembledName = m_AssembledName;
	copy->m_Delay = m_Delay;
	copy->m_bUseTarget = m_bUseTarget;
	copy->m_TargetPosition = m_TargetPosition;

	copy->m_SpawnOnStart = m_SpawnOnStart;
	copy->m_TimerStarted = false;
	copy->m_Spawned = false;
	copy->m_Timer = 0.0f;

	return copy;
}

void EntitySpawner::OnEvent(const StrID& eventName)
{
	if (eventName == SID(Spawn))
	{
		if (!m_TimerStarted)
		{
			m_TimerStarted = true;
		}
	}
}

#include "GamePch.h"

#include "AssembledEntitySet.h"
#include "Projectile.h"
#include "GunLogic.h"
#include "PlayerComp.h"
#include "ThirdPersonCameraComponent.h"

void AssembledEntitySet::Init()
{
	
}

void AssembledEntitySet::LoadSceneEntities()
{
	InitTestRobotAssembler();
}

void AssembledEntitySet::Shutdown()
{
	CleanupTestRobotAssembler();
}

//hg::Entity* AssembledEntitySet::InitPlayer()
//{
//	hg::Entity* player = hg::Entity::Create();
//
//	player->SetName(SID(Player));
//	player->SetTag(SID(Player));
//
//	PlayerComp* pc = (PlayerComp*)hg::IComponent::Create(SID(PlayerComp));
//	pc->SetEnabled(true);
//	pc->SetAlive(true);
//	pc->SetSpeed(5.0f);
//	player->AttachComponent(pc);
//
//	hg::PrimitiveRenderer* pr = (hg::PrimitiveRenderer*)hg::IComponent::Create(SID(PrimitiveRenderer));
//	pr->CreateColoredBox(Vector4(0.7f, 0.4f, 0.4f, 1.0f));
//	pr->SetEnabled(true);
//	player->AttachComponent(pr);
//
//	player->AddComponent(SID(DynamicCollider));
//	hg::DynamicCollider* dynamicCollider = player->GetComponent<hg::DynamicCollider>();
//	dynamicCollider->SetAabb(hg::Aabb(Vector3(-1, -1, -1), Vector3(1, 1, 1)));
//	dynamicCollider->SetEnabled(true);
//
//	player->GetTransform()->SetPosition(5.0f, 0.0f, 0.0f);
//
//	player->Init();
//	hg::AssembledEntityReg reg(SID(Player), player);
//	return player;
//}

void AssembledEntitySet::InitTestRobotAssembler()
{
	m_TestBot = new hg::Entity;

	m_TestBot->SetName( SID( Bot ) );
	m_TestBot->SetTag( SID( Enemy ) );

	/*hg::PrimitiveRenderer*/ pr = new hg::PrimitiveRenderer;
	//pr = m_TestBot->GetComponent<hg::PrimitiveRenderer>();
	pr->CreateColoredBox( Vector4( 0.45f, 0.35f, 0.50f, 1.0f ) );
	pr->SetEnabled( true );
	pr->SetAlive( true );
	m_TestBot->AttachComponent( pr );

	hg::DynamicCollider* dynamicCollider = new hg::DynamicCollider;
	dynamicCollider->SetAabb( hg::Aabb( Vector3( -0.1f, -0.1f, -0.1f ), Vector3( 0.1f, 0.1f, 0.1f ) ) );
	dynamicCollider->SetEnabled( true );
	dynamicCollider->SetAlive( true );
	m_TestBot->AttachComponent( dynamicCollider );

	// Create Behavior Tree
	hg::BehaviorTree* bt = new hg::BehaviorTree;

	// Assemble the root of the chosen behavior tree, by name
	hg::IBehavior* root = hg::IBehavior::AssembleRoot( SID( TestBot ) );
	bt->SetRoot( root );
	bt->SetEnabled( true );
	bt->SetAlive( true );

	m_TestBot->AttachComponent( bt );

	hg::AssembledEntityReg reg( SID( TestBot ), m_TestBot );
}

void AssembledEntitySet::InitBulletAssembler()
{
	m_bullet = new hg::Entity;
	m_bullet->SetName(SID(Projectile));
	m_bullet->SetTag(SID(Projectile));
	Projectile* proj = new Projectile;
	proj->SetAlive(true);
	proj->SetEnabled(true);
	proj->SetSpeed(30);
	proj->SetLifeTime(2);
	m_bullet->AttachComponent(proj);

	hg::Light* light = new hg::Light;
	light->Init(hg::kLightType_Point, Color(1.0f, 0.5f, 0.0f), 3.0f, 2.0f);
	light->SetAlive(true);
	light->SetEnabled(true);
	m_bullet->AttachComponent(light);

	hg::AssembledEntityReg reg(SID(Projectile), m_bullet);
}

void AssembledEntitySet::CleanupTestRobotAssembler()
{
	// Components must all be retrieved before deletion,
	// Cannot retrieve and delete one at a time
	if (m_TestBot)
	{
		const unsigned int ncomps = 3;
		hg::IComponent* comps[ncomps] =
		{
			m_TestBot->GetComponent<hg::BehaviorTree>(),
			m_TestBot->GetComponent<hg::DynamicCollider>(),
			m_TestBot->GetComponent<hg::PrimitiveRenderer>()
		};

		//delete pr;

		pr = nullptr;

		for (unsigned int i = 0; i < ncomps; ++i)
		{
			delete comps[i];
			comps[i] = nullptr;
		}

		delete m_TestBot;
	}
}

void AssembledEntitySet::CleanupBulletAssembler()
{
	if (m_bullet)
	{
		hg::IComponent* comps[] =
		{
			m_bullet->GetComponent<Projectile>(),
			m_bullet->GetComponent<hg::Light>(),
		};

		int size = sizeof(comps) / sizeof(hg::IComponent*);

		for (int i = 0; i < size; ++i)
		{
			delete comps[i];
			comps[i] = nullptr;
		}

		delete m_bullet;
	}
}

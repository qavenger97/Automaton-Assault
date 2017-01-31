#include "GamePch.h"

#include "AutomatonAssault.h"
#include "Common.h"

BehaviorSet			AutomatonAssault::m_BehaviorSet;
ComponentSet			AutomatonAssault::m_ComponentSet;
AssembledEntitySet	AutomatonAssault::m_AssEntitySet;

void AutomatonAssault::Init()
{
	m_ComponentSet.Init();
	m_BehaviorSet.Init();
	m_AssEntitySet.Init();	
}

void AutomatonAssault::Start()
{
	m_BehaviorSet.Start();
}
	
void AutomatonAssault::CreateScene()
{
	m_AssEntitySet.Shutdown();
	m_AssEntitySet.LoadSceneEntities();

	hg::Entity* camera = hg::Entity::FindByName(SID(Game Camera));
	camera->GetTransform()->SetRotation(XMConvertToRadians(60), 0, 0);
	ThirdPersonCameraComponent* tpc = camera->GetComponent<ThirdPersonCameraComponent>();
	tpc->Init();
	hg::Transform* trans;// = camera->GetTransform();
	//trans->Init();
	//camera->Init();
	//camera->GetTransform()->SetRotation( XMConvertToRadians( 60 ), 0, 0 );
	hg::g_AudioSystem.RegisterListener(camera, "Main Camera");

	//ThirdPersonCameraComponent* cp = camera->GetComponent<ThirdPersonCameraComponent>();
	//cp->AttachTo(player);
	//cp->SetSpeed(2);
	//UnitTest_CollisionGroups();
	
	//hg::g_Physics.SetCollisionBetweenGroups(kColGroup_Default, kColGroup_Enemy, false);
	hg::g_Physics.SetCollisionBetweenGroups(kColGroup_Player, kColGroup_EnemyObstacle, false);

	// Bullet blocker volume will not collide with any other group
	for (int i = 0; i < kMaxColGroup; i++)
	{
		hg::g_Physics.SetCollisionBetweenGroups(i, kColGroup_BulletBlocker, false);
	}

	hg::Entity* player = hg::Entity::FindByName(SID(Player));
	hg::Transform* gunPos = player->GetTransform()->FindChild(SID(Upper))->FindChild(SID(GunPos));
	trans = player->GetTransform()->FindChild(SID(Upper))->FindChild(SID(MachineGun));
	PlayerComp* ply = player->GetComponent<PlayerComp>();
	ply->SetGun(trans->GetEntity(), 0);
	hg::g_AudioSystem.RegisterEntity(player, "Player");

	GunLogic* gl = trans->GetEntity()->GetComponent<GunLogic>();
	GunLogic::ConfigureMachineGun(gl);
	gl->SetSpawnPoint(gunPos);

	trans = player->GetTransform()->FindChild(SID(Upper))->FindChild(SID(ShotGun));
	ply->SetGun(trans->GetEntity(), 1);
	ply->PostInit();
	gl = trans->GetEntity()->GetComponent<GunLogic>();
	GunLogic::ConfigureShotGun(gl);
	gl->SetSpawnPoint(gunPos);

	// Drop player onto ground
	hg::Entity* iko = hg::Entity::FindByName(SID(IKO));
	if(iko)
		iko->GetComponent<IKComponent>()->SetTarget(player);
	hg::DynamicCollider* dc = player->GetComponent<hg::DynamicCollider>();
	dc->SetEnabled(false);
	Vector3 hit;
	if (hg::g_Physics.RayCast(hg::Ray(player->GetTransform()->GetWorldPosition() + G_UP, -G_UP, 100), nullptr, &hit, nullptr, ~COLLISION_ENEMY_ONLY_OBSTACLE_MASK))
	{
		hit.y += 1.0f;
		player->GetTransform()->SetPosition(hit);
	}

	/*IKComponent* ik = player->GetComponent<IKComponent>();
	ik->SetTarget(player);*/
	dc->SetEnabled(true);
	MainMenu* mm = hg::Entity::FindByName(SID(MainMenu))->GetComponent<MainMenu>();
	mm->InitGame();
	mm->SetState(MainMenu::GameState::MAIN_MENU);

	hg::g_AudioSystem.PostEvent(AK::EVENTS::STOP_GAMEMUSIC);
	hg::g_AudioSystem.PostEvent(AK::EVENTS::STOP_BOSSMUSIC);
	hg::g_AudioSystem.PostEvent(AK::EVENTS::STOP_BOSSMUSICINTRO);
	hg::g_AudioSystem.SetState(AK::STATES::ATMOSPHERE::GROUP, AK::STATES::ATMOSPHERE::STATE::SAFE);
	hg::g_AudioSystem.SetState(AK::STATES::ENEMY::GROUP, AK::STATES::ENEMY::STATE::DEFAULT);
	hg::g_AudioSystem.PostEvent(AK::EVENTS::PLAY_GAMEMUSIC);
}

void AutomatonAssault::Shutdown()
{
	m_AssEntitySet.Shutdown();
}

void AutomatonAssault::InitTestRobot(float x, float y, float z, float scale, float speed)
{
	// Assemble a new robot
	hg::Entity* bot = hg::Entity::Assemble( SID( TestBot ));
	hg::Transform* trans = bot->GetTransform();
	trans->SetPosition( x, y, z );
	trans->SetScale( scale, scale, scale );

	bot->Init();
}

void AutomatonAssault::UnitTest_CollisionGroups()
{
	// Should all initialized to true
	assert(hg::g_Physics.GetCollisionBetweenGroups(0, 0));
	assert(hg::g_Physics.GetCollisionBetweenGroups(0, 1));
	assert(hg::g_Physics.GetCollisionBetweenGroups(0, 2));
	assert(hg::g_Physics.GetCollisionBetweenGroups(0, 15));
	assert(hg::g_Physics.GetCollisionBetweenGroups(1, 1));
	assert(hg::g_Physics.GetCollisionBetweenGroups(1, 15));
	assert(hg::g_Physics.GetCollisionBetweenGroups(15, 1));
	assert(hg::g_Physics.GetCollisionBetweenGroups(15, 15));

	// Set group collision and check result
	hg::g_Physics.SetCollisionBetweenGroups(0, 1, false);
	assert(hg::g_Physics.GetCollisionBetweenGroups(0, 1) == false);

	// Reversed order should also work
	assert(hg::g_Physics.GetCollisionBetweenGroups(1, 0) == false);

	hg::g_Physics.SetCollisionBetweenGroups(0, 1, true);

	// Should work on same group
	hg::g_Physics.SetCollisionBetweenGroups(15, 15, false);
	assert(hg::g_Physics.GetCollisionBetweenGroups(15, 15) == false);

	hg::g_Physics.SetCollisionBetweenGroups(15, 15, true);
	assert(hg::g_Physics.GetCollisionBetweenGroups(15, 15));
}

#include "GamePch.h"

#include "ComponentSet.h"

void ComponentSet::Init()
{
	//hg::ComponentRegister<PlayerComp> playerReg( SID(PlayerComp), &m_PlayerPool, hg::CompRegFlags::kAutoUpdate );
	//m_PlayerPool.Init( m_PlayerComp, 2 );

	INIT_GAME_COMPONENT_POOL(PlayerComp,					hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(ThirdPersonCameraComponent,	hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(Projectile,					hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(LevelExit,						hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(GunLogic,						hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(EntitySpawner,					hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(Health,						hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(MainMenu,						hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(SpinMotor,						hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(Gate,							hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(Terminal,						hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(RadialGravity,					hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(ShiftingPlatform,				hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(RotatingRoom,					hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(UnitShield,					hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(GrapplerComp,					hg::CompRegFlags::kNone);
	INIT_GAME_COMPONENT_POOL(IKObject,						hg::CompRegFlags::kNone);
	INIT_GAME_COMPONENT_POOL(IKComponent,					hg::CompRegFlags::kAutoUpdate);

	INIT_GAME_COMPONENT_POOL(EntityGroup,					hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(ChargerComp,					hg::CompRegFlags::kNone);
	INIT_GAME_COMPONENT_POOL(Crate,							hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(Turret,						hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(TurretSkinnedMesh,				hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(Laser,							hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(ActorShadow,					hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(CameraShakeEffect,				hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(Pickup,						hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(RotationalNoise,				hg::CompRegFlags::kAutoUpdate); 
	INIT_GAME_COMPONENT_POOL(PowerUpShield,					hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(AudioEventReceiver,			hg::CompRegFlags::kNone);
	INIT_GAME_COMPONENT_POOL(BossAgent,						hg::CompRegFlags::kAutoUpdate);
	INIT_GAME_COMPONENT_POOL(BossComp,						hg::CompRegFlags::kAutoUpdate);
}
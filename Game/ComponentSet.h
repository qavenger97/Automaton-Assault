#pragma once

#include "PlayerComp.h"
#include "ThirdPersonCameraComponent.h"
#include "Projectile.h"
#include "EntitySpawner.h"
#include "LevelExit.h"
#include "GunLogic.h"
#include "HealthModule.h"
#include "MainMenu.h"
#include "SpinMotor.h"
#include "RadialGravity.h"
#include "ShiftingPlatform.h"
#include "GrapplerComp.h"
#include "RotatingRoom.h"
#include "UnitShield.h"
#include "Gate.h"
#include "IKObject.h"
#include "Terminal.h"
#include "Pickup.h"
#include "PowerUpShield.h"
#include "IKComponent.h"
#include "EntityGroup.h"
#include "ChargerComp.h"
#include "Crate.h"
#include "Turret.h"
#include "TurretSkinnedMesh.h"
#include "Laser.h"
#include "ActorShadow.h"
#include "CameraShakeEffect.h"
#include "RotationalNoise.h"
#include "AudioEventReceiver.h"
#include "BossComp.h"
#include "BossAgent.h"

#define DECLARE_GAME_COMPONENT_POOL(type,size)		\
	type m_##type##s[size];							\
	hg::ComponentPool<type> m_##type##Pool;

#define INIT_GAME_COMPONENT_POOL(type,flag)									\
	hg::ComponentRegister<type> ##type##Reg( SID(type), &m_##type##Pool, flag );	\
	m_##type##Pool.Init(m_##type##s, sizeof(m_##type##s) / sizeof(type));


class ComponentSet
{
public:

	void Init();

private:
	//PlayerComp m_PlayerComp[2];
	//hg::ComponentPool<PlayerComp> m_PlayerPool;

	DECLARE_GAME_COMPONENT_POOL(PlayerComp, 1);
	DECLARE_GAME_COMPONENT_POOL(ThirdPersonCameraComponent, 1);
	DECLARE_GAME_COMPONENT_POOL(Projectile, 2048);
	DECLARE_GAME_COMPONENT_POOL(LevelExit, 1);
	DECLARE_GAME_COMPONENT_POOL(GunLogic, 128);
	DECLARE_GAME_COMPONENT_POOL(EntitySpawner, 512);
	DECLARE_GAME_COMPONENT_POOL(Health, 2048);
	DECLARE_GAME_COMPONENT_POOL(MainMenu, 1);
	DECLARE_GAME_COMPONENT_POOL(SpinMotor, 512);
	DECLARE_GAME_COMPONENT_POOL(Gate, 128);
	DECLARE_GAME_COMPONENT_POOL(Terminal, 128);
	DECLARE_GAME_COMPONENT_POOL(ShiftingPlatform, 3);
	DECLARE_GAME_COMPONENT_POOL(RadialGravity, 1);
	DECLARE_GAME_COMPONENT_POOL(GrapplerComp, 3);
	DECLARE_GAME_COMPONENT_POOL(RotatingRoom, 1);
	DECLARE_GAME_COMPONENT_POOL(IKObject, 1024);
	DECLARE_GAME_COMPONENT_POOL(IKComponent, 128);
	DECLARE_GAME_COMPONENT_POOL(UnitShield, 128);
	DECLARE_GAME_COMPONENT_POOL(EntityGroup, 1);
	DECLARE_GAME_COMPONENT_POOL(ChargerComp, 128);
	DECLARE_GAME_COMPONENT_POOL(Crate, 1024);
	DECLARE_GAME_COMPONENT_POOL(Turret, 128);
	DECLARE_GAME_COMPONENT_POOL(TurretSkinnedMesh, 128);
	DECLARE_GAME_COMPONENT_POOL(Laser, 64);
	DECLARE_GAME_COMPONENT_POOL(ActorShadow, 1024);
	DECLARE_GAME_COMPONENT_POOL(CameraShakeEffect, 64);
	DECLARE_GAME_COMPONENT_POOL(Pickup, 128);
	DECLARE_GAME_COMPONENT_POOL(PowerUpShield, 2);
	DECLARE_GAME_COMPONENT_POOL(RotationalNoise, 16);
	DECLARE_GAME_COMPONENT_POOL(AudioEventReceiver, 64);
	DECLARE_GAME_COMPONENT_POOL(BossComp, 32);
	DECLARE_GAME_COMPONENT_POOL(BossAgent, 1);
};


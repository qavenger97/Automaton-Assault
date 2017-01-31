#pragma once

#include "ImplodeExplode.h"
#include "Chase.h"
#include "BhvAIAttack.h"
#include "AI_Attack.h"
#include "AI_Die.h"
#include "AI_Chase.h"
#include "AI_HasHP.h"
#include "AI_IsInAtkRange.h"
#include "AI_Track.h"
#include "Temp_PlayerDie.h"
#include "AI_ChargerRocketSpawn.h"

#include "AI_ExtendClaw.h"
#include "AI_Grapple.h"
#include "AI_MoveToLoc.h"
#include "AI_RadialPatrol.h"
#include "AI_SpinClaw.h"
#include "AI_TogglePlayerMovement.h"
#include "AI_ToggleShield.h"

#include "AI_EnemyGrouping.h"
#include "AI_IsPlayerAlone.h"
#include "AI_ChargeUp.h"
#include "AI_Lunge.h"
#include "AI_SpiderBotBehavior.h"
#include "AI_KeypressToContinue.h"
#include "AI_Tunnel.h"
#include "AI_Surface.h"
#include "AI_ToggleWurmDoor.h"
#include "AI_CarrierSpawnEnemies.h"
#include "AI_CarrierReleaseEnemies.h"
#include "AI_Rise.h"
#include "AI_ChargeBeam.h"
#include "AI_TurnTo.h"
#include "AI_FireLaser.h"
#include "AI_LaserBurnStationary.h"
#include "AI_Teleport.h"
#include "AI_LaserChase.h"
#include "AI_AlignBossHead.h"
#include "AI_PositionCarrier.h"
#include "AI_PlayAudioEvent.h"
#include "AI_ToggleLaserHP.h"

#define DECLARE_GAME_BEHAVIOR_POOL(type,size)		\
	type m_##type##s[size];							\
	hg::BehaviorPool<type> m_##type##Pool;

#define INIT_GAME_BEHAVIOR_POOL(type)									    \
	hg::BehaviorRegister<type> ##type##Reg( SID(type), &m_##type##Pool);	\
	m_##type##Pool.Init(m_##type##s, sizeof(m_##type##s) / sizeof(type));

class BehaviorSet: Hourglass::BehaviorFactory
{
public:

	void Init();	

	void Start();

private:

	template<typename T>
	void BehaviorPoolInit( StrID name, T * data, Hourglass::BehaviorPool<T>* pool, unsigned int size );

private:

	DECLARE_GAME_BEHAVIOR_POOL(AI_ExtendClaw,6)
	DECLARE_GAME_BEHAVIOR_POOL(AI_Grapple,6)
	DECLARE_GAME_BEHAVIOR_POOL(AI_MoveToLoc,6)
	DECLARE_GAME_BEHAVIOR_POOL(AI_RadialPatrol,6)
	DECLARE_GAME_BEHAVIOR_POOL(AI_SpinClaw,6)
	DECLARE_GAME_BEHAVIOR_POOL(AI_TogglePlayerMovement,27)
	DECLARE_GAME_BEHAVIOR_POOL(AI_ToggleShield,256)
	DECLARE_GAME_BEHAVIOR_POOL(AI_EnemyGrouping,256)
	DECLARE_GAME_BEHAVIOR_POOL(AI_IsPlayerAlone,256)
	DECLARE_GAME_BEHAVIOR_POOL(AI_ChargeUp,256)
	DECLARE_GAME_BEHAVIOR_POOL(AI_Lunge,256)
	DECLARE_GAME_BEHAVIOR_POOL(AI_SpiderBotBehavior, 64)
	DECLARE_GAME_BEHAVIOR_POOL(AI_KeypressToContinue, 32)
	DECLARE_GAME_BEHAVIOR_POOL(AI_Tunnel, 32)
	DECLARE_GAME_BEHAVIOR_POOL(AI_Surface, 32)
	DECLARE_GAME_BEHAVIOR_POOL(AI_ToggleWurmDoor, 32)
	DECLARE_GAME_BEHAVIOR_POOL(AI_CarrierSpawnEnemies, 32)
	DECLARE_GAME_BEHAVIOR_POOL(AI_CarrierReleaseEnemies, 32);
	DECLARE_GAME_BEHAVIOR_POOL(AI_Rise, 32);
	DECLARE_GAME_BEHAVIOR_POOL(AI_ChargeBeam, 32);
	DECLARE_GAME_BEHAVIOR_POOL(AI_TurnTo, 32);
	DECLARE_GAME_BEHAVIOR_POOL(AI_FireLaser, 32);
	DECLARE_GAME_BEHAVIOR_POOL(AI_LaserBurnStationary, 32)
	DECLARE_GAME_BEHAVIOR_POOL(AI_Teleport, 32)
	DECLARE_GAME_BEHAVIOR_POOL(AI_LaserChase, 32)
	DECLARE_GAME_BEHAVIOR_POOL(AI_AlignBossHead, 32)
	DECLARE_GAME_BEHAVIOR_POOL(AI_PositionCarrier, 32)
	DECLARE_GAME_BEHAVIOR_POOL(AI_PlayAudioEvent, 64)
	DECLARE_GAME_BEHAVIOR_POOL(AI_ToggleLaserHP, 16)
	
	static const int s_kMaxMoveInCircles = 256;
	ImplodeExplode m_MoveInCircles[s_kMaxMoveInCircles];
	Hourglass::BehaviorPool<ImplodeExplode> m_MoveInCirclesPool;

	static const int s_kMaxEnemyChasers = 256;
	Chase m_EnemyChasers[s_kMaxEnemyChasers];
	Hourglass::BehaviorPool<Chase> m_EnemyChasersPool;

	static const int s_kMaxEnemyAttackers = 256;
	BhvAIAttack m_EnemyAttackers[s_kMaxEnemyAttackers];
	Hourglass::BehaviorPool<BhvAIAttack> m_EnemyAttackersPool;

	static const int s_kMaxAIAttack = 256;
	AI_Attack m_AIAttack[s_kMaxAIAttack];
	Hourglass::BehaviorPool<AI_Attack> m_AIAttackPool;

	static const int s_kMaxAIChase = 256;
	AI_Chase m_AIChase[s_kMaxAIChase];
	Hourglass::BehaviorPool<AI_Chase> m_AIChasePool;

	static const int s_kMaxAIDie = 256;
	AI_Die m_AIDie[s_kMaxAIDie];
	Hourglass::BehaviorPool<AI_Die> m_AIDiePool;

	static const int s_kMaxAIHasHP = 256;
	AI_HasHP m_AIHasHP[s_kMaxAIHasHP];
	Hourglass::BehaviorPool<AI_HasHP> m_AIHasHPPool;

	static const int s_kMaxAIIsInAtkRange = 256;
	AI_IsInAtkRange m_AIIsInAtkRange[s_kMaxAIIsInAtkRange];
	Hourglass::BehaviorPool<AI_IsInAtkRange> m_AIIsInAtkRangePool;

	static const int s_kMaxAITrack = 256;
	AI_Track m_AITrack[s_kMaxAITrack];
	Hourglass::BehaviorPool<AI_Track> m_AITrackPool;

	static const int s_kMaxTempPlayerDie = 4;
	Temp_PlayerDie m_TempPlayerDie[s_kMaxTempPlayerDie];
	Hourglass::BehaviorPool<Temp_PlayerDie> m_TempPlayerDiePool;

	static const int s_kMaxAIChargerRocketSpawn = 256;
	AI_ChargerRocketSpawn m_AIChargerRocketSpawn[s_kMaxAIChargerRocketSpawn];
	Hourglass::BehaviorPool<AI_ChargerRocketSpawn> m_AIChargerRocketSpawnPool;
};

template<typename T>
inline void BehaviorSet::BehaviorPoolInit( StrID name, T* data, Hourglass::BehaviorPool<T>* pool, unsigned int size )
{
	Hourglass::BehaviorRegister<T> reg( name, pool );
	pool->Init( data, size );
}
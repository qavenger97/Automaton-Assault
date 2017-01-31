#include "GamePch.h"

#include "BehaviorSet.h"

bool g_bDebugAIBehaviors = false;

void BehaviorSet::Init()
{
	INIT_GAME_BEHAVIOR_POOL(AI_ExtendClaw)
	INIT_GAME_BEHAVIOR_POOL(AI_Grapple)
	INIT_GAME_BEHAVIOR_POOL(AI_MoveToLoc)
	INIT_GAME_BEHAVIOR_POOL(AI_RadialPatrol)
	INIT_GAME_BEHAVIOR_POOL(AI_SpinClaw)
	INIT_GAME_BEHAVIOR_POOL(AI_TogglePlayerMovement)
	INIT_GAME_BEHAVIOR_POOL(AI_ToggleShield)
	INIT_GAME_BEHAVIOR_POOL(AI_EnemyGrouping)
	INIT_GAME_BEHAVIOR_POOL(AI_IsPlayerAlone)
	INIT_GAME_BEHAVIOR_POOL(AI_ChargeUp)
	INIT_GAME_BEHAVIOR_POOL(AI_Lunge)
	INIT_GAME_BEHAVIOR_POOL(AI_SpiderBotBehavior)
	INIT_GAME_BEHAVIOR_POOL(AI_KeypressToContinue)
	INIT_GAME_BEHAVIOR_POOL(AI_Tunnel)
	INIT_GAME_BEHAVIOR_POOL(AI_Surface)
	INIT_GAME_BEHAVIOR_POOL(AI_ToggleWurmDoor)
	INIT_GAME_BEHAVIOR_POOL(AI_CarrierSpawnEnemies)
	INIT_GAME_BEHAVIOR_POOL(AI_CarrierReleaseEnemies)
	INIT_GAME_BEHAVIOR_POOL(AI_Rise)
	INIT_GAME_BEHAVIOR_POOL(AI_ChargeBeam)
	INIT_GAME_BEHAVIOR_POOL(AI_TurnTo)
	INIT_GAME_BEHAVIOR_POOL(AI_FireLaser)
	INIT_GAME_BEHAVIOR_POOL(AI_LaserBurnStationary)
	INIT_GAME_BEHAVIOR_POOL(AI_Teleport)
	INIT_GAME_BEHAVIOR_POOL(AI_LaserChase)
	INIT_GAME_BEHAVIOR_POOL(AI_AlignBossHead)
	INIT_GAME_BEHAVIOR_POOL(AI_PositionCarrier)
	INIT_GAME_BEHAVIOR_POOL(AI_PlayAudioEvent)
	INIT_GAME_BEHAVIOR_POOL(AI_ToggleLaserHP)

	BehaviorPoolInit( SID(ImplodeExplode), m_MoveInCircles, &m_MoveInCirclesPool, s_kMaxMoveInCircles );
	BehaviorPoolInit( SID(Chase), m_EnemyChasers, &m_EnemyChasersPool, s_kMaxEnemyChasers );
	BehaviorPoolInit( SID(BhvAIAttack), m_EnemyAttackers, &m_EnemyAttackersPool, s_kMaxEnemyAttackers );
	BehaviorPoolInit( SID(AI_Attack), m_AIAttack, &m_AIAttackPool, s_kMaxAIAttack );
	BehaviorPoolInit( SID(AI_Chase), m_AIChase, &m_AIChasePool, s_kMaxAIChase );
	BehaviorPoolInit( SID(AI_Die), m_AIDie, &m_AIDiePool, s_kMaxAIDie );
	BehaviorPoolInit( SID(AI_HasHP), m_AIHasHP, &m_AIHasHPPool, s_kMaxAIHasHP );
	BehaviorPoolInit( SID(AI_IsInAtkRange), m_AIIsInAtkRange, &m_AIIsInAtkRangePool, s_kMaxAIIsInAtkRange );
	BehaviorPoolInit( SID(AI_Track), m_AITrack, &m_AITrackPool, s_kMaxAITrack );
	BehaviorPoolInit( SID(Temp_PlayerDie), m_TempPlayerDie, &m_TempPlayerDiePool, s_kMaxTempPlayerDie );
	BehaviorPoolInit( SID(AI_ChargerRocketSpawn), m_AIChargerRocketSpawn, &m_AIChargerRocketSpawnPool, s_kMaxAIChargerRocketSpawn );

	hg::g_DevMenu.AddMenuVarBool("Debug AI: ", &g_bDebugAIBehaviors);
}

void BehaviorSet::Start()
{
	m_MoveInCirclesPool.Start();
	m_EnemyChasersPool.Start();
	m_EnemyAttackersPool.Start();
	m_AIAttackPool.Start();
	m_AIChasePool.Start();
	m_AIDiePool.Start();
	m_AIHasHPPool.Start();
	m_AIIsInAtkRangePool.Start();
	m_AITrackPool.Start();
	m_TempPlayerDiePool.Start();
	m_AIChargerRocketSpawnPool.Start();
}


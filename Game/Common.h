#pragma once

enum ECollisionGroup
{
	kColGroup_Default,
	kColGroup_Enemy,
	kColGroup_Unwalkable,
	kColGroup_Player,
	kColGroup_EnemyObstacle,			// Obstacles that block enemy sight and movement
	kColGroup_Breakable,
	kColGroup_BulletBlocker,
	kColGroup_BlockPlayerAllowBullet,
	kMaxColGroup
};

enum ECollisionGroupMask
{
	COLLISION_DEFAULT_MASK =					(1 << kColGroup_Default),
	COLLISION_ENEMY_MASK =						(1 << kColGroup_Enemy),
	COLLISION_UNWALKABLE_MASK =					(1 << kColGroup_Unwalkable),
	COLLISION_PLAYER_MASK =						(1 << kColGroup_Player),
	COLLISION_ENEMY_ONLY_OBSTACLE_MASK =		(1 << kColGroup_EnemyObstacle),
	COLLISION_BREAKABLE_MASK =					(1 << kColGroup_Breakable),
	COLLISION_BULLET_BLOCKER_MASK =				(1 << kColGroup_BulletBlocker),
	COLLISION_BLOCK_PLAYER_ALLOW_BULLET_MASK =	(1 << kColGroup_BlockPlayerAllowBullet)
};

// Mask of colliders that can be hit by bullets
#define COLLISION_BULLET_HIT_MASK (COLLISION_DEFAULT_MASK|COLLISION_ENEMY_MASK|COLLISION_PLAYER_MASK|COLLISION_BREAKABLE_MASK|COLLISION_BULLET_BLOCKER_MASK)

#include "GamePch.h"

#include "GameUtils.h"
#include "Common.h"



bool GameUtils::AI_CanSeePlayer(hg::Entity* ai, hg::Entity* player, const Vector3& eyeOffset /*= Vector3(0, 0, 0)*/)
{
	Vector3 start = ai->GetPosition() + eyeOffset;
	hg::Ray ray(start, player->GetPosition());
	hg::Entity* hitEnt;

	static const uint16_t kEnemyVisionMask = COLLISION_DEFAULT_MASK | COLLISION_UNWALKABLE_MASK | COLLISION_PLAYER_MASK;

	if (hg::g_Physics.RayCast(ray, &hitEnt, nullptr, nullptr, kEnemyVisionMask))
	{
		if (hitEnt->GetTag() == SID(Player))
		{
			return true;
		}
	}

	return false;
}

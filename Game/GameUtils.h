#pragma once

namespace GameUtils
{
	// Test if ai can see player
	bool AI_CanSeePlayer(hg::Entity* ai, hg::Entity* player, const Vector3& eyeOffset = Vector3(0, 0, 0));
}
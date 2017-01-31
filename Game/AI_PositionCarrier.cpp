#include "GamePch.h"

#include "AI_PositionCarrier.h"

void AI_PositionCarrier::Init( hg::Entity* entity )
{
	hg::Entity* player = hg::Entity::FindByTag(SID(Player));
	Vector3 playerPos = player->GetPosition();

	hg::Entity* boss = hg::Entity::FindByName(SID(FinalBossRadial));

	const float top = 81.0f;
	const float middle = 70.0f;
	const float bottom = 57.0f;
	const float left = 209.0f;
	const float right = 227.0f;
	const float offset = 3.0f;
	const float centerDivider = 218.0f;
	const float topCenterSpot = 221.0f;

	Vector3 bossPos = boss->GetPosition();
	if (playerPos.z < bottom - offset)
		bossPos.z = bottom;
	else if (playerPos.z < middle - offset)
		bossPos.z = middle;
	else
	{
		bossPos.z = top;
		bossPos.x = topCenterSpot;
		boss->GetTransform()->SetPosition( bossPos );
		return;
	}

	if (playerPos.x < centerDivider)
		bossPos.x = left;
	else
		bossPos.x = right;

	boss->GetTransform()->SetPosition( bossPos );
}

Hourglass::IBehavior::Result AI_PositionCarrier::Update( Hourglass::Entity* entity )
{
	return IBehavior::kSUCCESS;
}

Hourglass::IBehavior* AI_PositionCarrier::MakeCopy() const
{
	AI_PositionCarrier* copy = (AI_PositionCarrier*)IBehavior::Create( SID(AI_PositionCarrier) );
	// TODO: copy data members for assembled behaviors

	return copy;
}

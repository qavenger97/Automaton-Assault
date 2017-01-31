#include "GamePch.h"

#include "AI_CarrierSpawnEnemies.h"

void AI_CarrierSpawnEnemies::Init( hg::Entity * entity )
{
	StrID botSID = SID(CarrierChargerBot);
	hg::Transform* carrier = hg::Entity::FindByTag( SID(RadialCarrier) )->GetTransform();

	Quaternion rot = Quaternion::CreateFromYawPitchRoll( -XM_PIDIV2, 0.0f, 0.0f );
	rot *= carrier->GetWorldRotation();

	for (unsigned int col = 0; col < 4; ++col)
	{
		for (unsigned int row = 0; row < 3; ++row)
		{
			Vector3 pos = { -3.7f + col * 0.75f, -1.1f, -0.7f + row * 0.7f };
			pos += carrier->GetWorldPosition();
			hg::Entity* enemy = hg::Entity::Assemble( botSID, pos, rot );
			enemy->GetComponent<hg::Motor>()->SetEnabled( false );
			enemy->GetComponent<hg::BehaviorTree>()->SetEnabled( false );
			enemy->GetTransform()->LinkTo( carrier );
		}		
	}	
}

hg::IBehavior::Result AI_CarrierSpawnEnemies::Update( hg::Entity * entity )
{
	return hg::IBehavior::kSUCCESS;
}

hg::IBehavior * AI_CarrierSpawnEnemies::MakeCopy() const
{
	AI_CarrierSpawnEnemies* copy = (AI_CarrierSpawnEnemies*)IBehavior::Create( SID(AI_CarrierSpawnEnemies) );
	return copy;
}

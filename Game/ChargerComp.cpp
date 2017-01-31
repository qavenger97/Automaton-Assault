#include "GamePch.h"

#include "ChargerComp.h"
#include "Projectile.h"

uint32_t ChargerComp::s_TypeID = hg::ComponentFactory::GetGameComponentID();


void ChargerComp::Init()
{
	m_Lunging = false;
}

void ChargerComp::OnCollision( Hourglass::Entity* other )
{
	if (m_Lunging)
	{
		DamageMessage d(kDmgType_ChargerBotMelee);
		other->SendMsg( &d );
		m_Lunging = false;
	}
}

hg::IComponent* ChargerComp::MakeCopyDerived() const
{
	ChargerComp* copy = (ChargerComp*)IComponent::Create( SID(ChargerComp) );
	return copy;
}

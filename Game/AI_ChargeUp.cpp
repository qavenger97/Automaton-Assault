#include "GamePch.h"

#include "AI_ChargeUp.h"

void AI_ChargeUp::LoadFromXML( tinyxml2::XMLElement* data )
{
	const char* anim = data->Attribute( "anim" );
	m_Anim = WSID(anim);

	m_Duration = 2.0f;
	data->QueryFloatAttribute( "duration", &m_Duration );
}

void AI_ChargeUp::Init( Hourglass::Entity* entity )
{
	m_Timer = 0.0f;
	entity->GetComponent<hg::Animation>()->Play( m_Anim, 1.0f );
}

Hourglass::IBehavior::Result AI_ChargeUp::Update( Hourglass::Entity* entity )
{
	m_Timer += hg::g_Time.Delta();
	return (m_Timer < m_Duration) ? kRUNNING : kSUCCESS;
}

Hourglass::IBehavior* AI_ChargeUp::MakeCopy() const
{
	AI_ChargeUp* copy = (AI_ChargeUp*)IBehavior::Create( SID(AI_ChargeUp) );
	copy->m_Anim = m_Anim;
	copy->m_Duration = m_Duration;

	return copy;
}

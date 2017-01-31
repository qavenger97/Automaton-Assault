#include "GamePch.h"

#include "AI_KeypressToContinue.h"

void AI_KeypressToContinue::LoadFromXML( tinyxml2::XMLElement* data )
{
	const char* keyStr = data->Attribute( "key" );
	m_Key = (keyStr) ? keyStr[0] : 'O';
}

Hourglass::IBehavior::Result AI_KeypressToContinue::Update( Hourglass::Entity* entity )
{
	if (hg::g_Input.IsKeyDown( m_Key ))
		return kSUCCESS;
	else
		return kRUNNING;
}

Hourglass::IBehavior* AI_KeypressToContinue::MakeCopy() const
{
	AI_KeypressToContinue* copy = (AI_KeypressToContinue*)IBehavior::Create( SID(AI_KeypressToContinue) );
	copy->m_Key = m_Key;
	return copy;
}

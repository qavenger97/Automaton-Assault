#include "GamePch.h"

#include "ImplodeExplode.h"

void ImplodeExplode::LoadFromXML( tinyxml2::XMLElement* data )
{
	data->QueryFloatAttribute( "speed", &m_CircleSpeed );
	data->QueryIntAttribute( "test", &m_Test );
}

void ImplodeExplode::Init( Hourglass::Entity* entity )
{
	hg::Entity* player = hg::Entity::FindByName( SID( Player ) );
	m_player = player->GetTransform();
	m_dynColl = entity->GetComponent<hg::DynamicCollider>();
	m_Time = 0.0f;	
}

Hourglass::IBehavior::Result ImplodeExplode::Update( Hourglass::Entity* entity )
{
	m_Time += Hourglass::g_Time.Delta();

	XMVECTOR direction;

	hg::Transform& trans = *entity->GetTransform();
	XMVECTOR pos = trans.GetWorldPosition();
	XMVECTOR playerPos = m_player->GetWorldPosition();

	if (m_Time >= 2.0f)
	{
		return Hourglass::IBehavior::kSUCCESS;
	}
	else if(m_Test == 0)
	{
		direction = XMVector3Normalize( playerPos - pos );
	}
	else
	{		
		direction = XMVector3Normalize( pos - playerPos );
	}

	direction *= m_CircleSpeed * Hourglass::g_Time.Delta();

	Vector3 dirVec3;
	XMStoreFloat3( &dirVec3, direction );
	m_dynColl->Move(dirVec3);

	return Hourglass::IBehavior::kRUNNING;
}

Hourglass::IBehavior* ImplodeExplode::MakeCopy() const
{
	ImplodeExplode* copy = (ImplodeExplode*)IBehavior::Create( SID(ImplodeExplode) );
	copy->SetCircleSpeed( m_CircleSpeed );
	copy->SetTest( m_Test );
	return copy;
}